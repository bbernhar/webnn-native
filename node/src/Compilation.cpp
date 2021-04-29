// Copyright 2021 The WebNN-native Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Compilation.h"

#include <map>

#include "Utils.h"

namespace node {

    struct Input {
      public:
        void const* buffer;
        size_t size;
        std::vector<int32_t> dimensions;

        const webnn::Input* AsPtr() {
            mInput.buffer = buffer;
            mInput.size = size;
            if (!dimensions.empty()) {
                mInput.dimensions = dimensions.data();
                mInput.dimensionsCount = dimensions.size();
            }
            return &mInput;
        }

      private:
        webnn::Input mInput;
    };

    struct Output {
      public:
        void* buffer = nullptr;
        size_t size;
        std::vector<int32_t> dimensions;

        const webnn::Output* AsPtr() {
            mOutput.buffer = buffer;
            mOutput.size = size;
            if (!dimensions.empty()) {
                mOutput.dimensions = dimensions.data();
                mOutput.dimensionsCount = dimensions.size();
            }
            return &mOutput;
        }

      private:
        webnn::Output mOutput;
    };

    // Hold Promise::Deferred with AsyncWorker.
    class ComputeAsyncWorker : public Napi::AsyncWorker {
      public:
        ComputeAsyncWorker(Napi::Env& env,
                           Napi::Promise::Deferred& deferred,
                           webnn::Compilation compilation,
                           std::map<std::string, Input> inputs,
                           std::map<std::string, Output> outputs,
                           std::vector<std::string> outputNames)
            : Napi::AsyncWorker(env),
              mEnv(env),
              mDeferred(deferred),
              mCompilation(compilation),
              mInputs(std::move(inputs)),
              mOutputs(std::move(outputs)),
              mOutputNames(std::move(outputNames)) {
        }

        ~ComputeAsyncWorker() = default;

        void Execute() {
            webnn::NamedInputs namedInputs = webnn::CreateNamedInputs();
            for (auto& input : mInputs) {
                namedInputs.Set(input.first.data(), input.second.AsPtr());
            }
            webnn::NamedOutputs namedOutputs =
                mOutputs.empty() ? nullptr : webnn::CreateNamedOutputs();
            for (auto& output : mOutputs) {
                namedOutputs.Set(output.first.data(), output.second.AsPtr());
            }

            mCompilation.Compute(
                namedInputs,
                [](WebnnComputeStatus status, WebnnNamedResults results, char const* message,
                   void* userdata) {
                    ComputeAsyncWorker* computeWorker =
                        reinterpret_cast<ComputeAsyncWorker*>(userdata);
                    computeWorker->SetResults(status, results, message);
                },
                reinterpret_cast<void*>(this), namedOutputs);
        }

        void OnOK() {
            if (mStatus != webnn::ComputeStatus::Success) {
                return mDeferred.Reject(Napi::Value::From(mEnv, mMessage));
            }
            Napi::Object jsResults = Napi::Object::New(mEnv);
            for (auto& name : mOutputNames) {
                webnn::Result result = mNamedResults.Get(name.data());
                Napi::Object jsOutput = Napi::Object::New(mEnv);
                Napi::ArrayBuffer arrayBuffer = Napi::ArrayBuffer::New(
                    mEnv, const_cast<void*>(result.Buffer()), result.BufferSize());
                // FIXME: handle other data types
                Napi::Float32Array float32Array = Napi::Float32Array::New(
                    mEnv, result.BufferSize() / sizeof(float), arrayBuffer, 0);
                jsOutput.Set("buffer", float32Array);
                if (result.Dimensions()) {
                    Napi::Array jsDimensions = Napi::Array::New(mEnv, result.DimensionsSize());
                    for (size_t i = 0; i < result.DimensionsSize(); ++i) {
                        jsDimensions[i] = Napi::Number::New(mEnv, result.Dimensions()[i]);
                    }
                    jsOutput.Set("dimensions", jsDimensions);
                }
                jsResults.Set(name, jsOutput);
            }
            mDeferred.Resolve(jsResults);
        }

        void SetResults(WebnnComputeStatus status, WebnnNamedResults results, char const* message) {
            mStatus = static_cast<webnn::ComputeStatus>(status);
            mNamedResults = mNamedResults.Acquire(results);
            if (message) {
                mMessage = std::string(message);
            }
        }

      private:
        Napi::Env mEnv;
        Napi::Promise::Deferred mDeferred;
        webnn::Compilation mCompilation;
        webnn::ComputeStatus mStatus;
        std::string mMessage;
        std::map<std::string, Input> mInputs;
        std::map<std::string, Output> mOutputs;
        std::vector<std::string> mOutputNames;
        webnn::NamedResults mNamedResults;
    };

    template <class T>
    bool GetNamedResources(const Napi::Value& jsValue, std::map<std::string, T>& namedResources) {
        if (!jsValue.IsObject()) {
            return false;
        }
        Napi::Object jsResources = jsValue.As<Napi::Object>();
        Napi::Array names = jsResources.GetPropertyNames();
        for (size_t i = 0; i < names.Length(); ++i) {
            std::string name = names.Get(i).As<Napi::String>().Utf8Value();
            Napi::Object jsResource = jsResources.Get(name).As<Napi::Object>();
            // dictionary Input {
            //   required ArrayBufferView buffer;
            //   sequence<long> dimensions;
            // };
            // dictionary Output {
            //   ArrayBufferView buffer;
            //   sequence<long> dimensions;
            // };
            T resource;
            if (resource.buffer != nullptr && !jsResource.Has("buffer")) {
                // Input buffer is required.
                return false;
            }
            if (jsResource.Has("buffer")) {
                if (!jsResource.Get("buffer").IsTypedArray()) {
                    return false;
                }

                // FIXME: validate the type of typed array.

                Napi::TypedArray jsTypedArray = jsResource.Get("buffer").As<Napi::TypedArray>();
                resource.buffer = reinterpret_cast<void*>(
                    reinterpret_cast<int8_t*>(jsTypedArray.ArrayBuffer().Data()) +
                    jsTypedArray.ByteOffset());
                resource.size = jsTypedArray.ByteLength();
            }
            if (jsResource.Has("dimensions")) {
                if (!GetInt32Array(jsResource.Get("dimensions"), resource.dimensions)) {
                    return false;
                }
            }
            namedResources[name] = resource;
        }
        return true;
    }

    Napi::FunctionReference Compilation::constructor;

    Compilation::Compilation(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Compilation>(info) {
    }

    Napi::Value Compilation::Compute(const Napi::CallbackInfo& info) {
        // Promise<NamedOutputs> compute(NamedInputs inputs, optional NamedOutputs outputs = {});
        WEBNN_NODE_ASSERT(info.Length() == 1 || info.Length() == 2,
                          "The number of arguments is invalid.");
        std::map<std::string, Input> inputs;
        WEBNN_NODE_ASSERT(GetNamedResources<Input>(info[0], inputs),
                          "The inputs parameter is invalid.");

        std::map<std::string, Output> outputs;
        if (info.Length() > 1) {
            WEBNN_NODE_ASSERT(GetNamedResources<Output>(info[1], outputs),
                              "The outputs parameter is invalid.");
        }
        Napi::Env env = info.Env();
        auto deferred = Napi::Promise::Deferred::New(env);
        ComputeAsyncWorker* worker = new ComputeAsyncWorker(env, deferred, mImpl, std::move(inputs),
                                                            std::move(outputs), mOutputNames);
        worker->Queue();
        return deferred.Promise();
    }

    Napi::Object Compilation::Initialize(Napi::Env env, Napi::Object exports) {
        Napi::HandleScope scope(env);
        Napi::Function func =
            DefineClass(env, "Compilation",
                        {InstanceMethod("compute", &Compilation::Compute, napi_enumerable)});
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        exports.Set("Compilation", func);
        return exports;
    }

}  // namespace node
