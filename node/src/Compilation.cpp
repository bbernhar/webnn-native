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

#include <iostream>
#include <map>

#include "Model.h"

// Hold Promise::Deferred with AsyncWorker.
class ComputeAsyncWorker : public Napi::AsyncWorker {
  public:
    ComputeAsyncWorker(Napi::Env& env,
                       Napi::Promise::Deferred& deferred,
                       WebnnCompilation compilation,
                       std::map<std::string, WebnnInput> inputs,
                       std::map<std::string, WebnnOutput> outputs,
                       std::vector<std::string> outputNames)
        : Napi::AsyncWorker(env),
          mEnv(env),
          mDeferred(deferred),
          mCompilation(compilation),
          mInputs(std::move(inputs)),
          mOutputs(std::move(outputs)),
          mOutputNames(std::move(outputNames)) {
    }

    ~ComputeAsyncWorker() {
        if (mNamedResults) {
            webnnNamedResultsRelease(mNamedResults);
        }
    }

    void Execute() {
        WebnnNamedInputs namedInputs = webnnCreateNamedInputs();
        for (auto& input : mInputs) {
            webnnNamedInputsSet(namedInputs, input.first.data(), &input.second);
        }
        WebnnNamedOutputs namedOutputs = mOutputs.empty() ? nullptr : webnnCreateNamedOutputs();
        for (auto& output : mOutputs) {
            webnnNamedOutputsSet(namedOutputs, output.first.data(), &output.second);
        }

        webnnCompilationCompute(
            mCompilation, namedInputs,
            [](WebnnComputeStatus status, WebnnNamedResults results, char const* message,
               void* userdata) {
                ComputeAsyncWorker* computeWorker = reinterpret_cast<ComputeAsyncWorker*>(userdata);
                computeWorker->SetNamedResults(results);
            },
            reinterpret_cast<void*>(this), namedOutputs);
    }
    void OnOK() {
        if (mOutputs.empty() && mNamedResults != nullptr) {
            Napi::Object obj = Napi::Object::New(mEnv);
            for (auto& name : mOutputNames) {
                WebnnResult result = webnnNamedResultsGet(mNamedResults, name.data());
                obj.Set(name, OutputItem(mEnv, result));
                webnnResultRelease(result);
            }
            mDeferred.Resolve(obj);
        } else {
            mDeferred.Resolve(Env().Null());
        }
    }
    void SetNamedResults(WebnnNamedResults namedResults) {
        mNamedResults = namedResults;
    }

  private:
    Napi::Object OutputItem(const Napi::Env& env, WebnnResult result) {
        Napi::Object item = Napi::Object::New(env);
        size_t bufferSize = webnnResultBufferSize(result);
        Napi::ArrayBuffer buffer =
            Napi::ArrayBuffer::New(env, const_cast<void*>(webnnResultBuffer(result)), bufferSize);
        Napi::Float32Array outputBuffer =
            Napi::Float32Array::New(env, bufferSize / sizeof(float), buffer, 0);
        item.Set("buffer", outputBuffer);

        size_t dimensionsSize = webnnResultDimensionsSize(result);
        Napi::Array dimensions = Napi::Array::New(env, dimensionsSize);
        for (size_t i = 0; i < dimensionsSize; ++i) {
            dimensions[i] = Napi::Number::New(env, webnnResultDimensions(result)[i]);
        }
        item.Set("dimensions", dimensions);
        return item;
    }
    Napi::Env mEnv;
    Napi::Promise::Deferred mDeferred;
    WebnnCompilation mCompilation;
    std::map<std::string, WebnnInput> mInputs;
    std::map<std::string, WebnnOutput> mOutputs;
    std::vector<std::string> mOutputNames;
    WebnnNamedResults mNamedResults;
};

Napi::FunctionReference Compilation::constructor;

Compilation::Compilation(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Compilation>(info), mCompilation(nullptr) {
}

Compilation::~Compilation() {
    if (mCompilation) {
        webnnCompilationRelease(mCompilation);
    }
}

void Compilation::SetCompilation(WebnnCompilation Compilation) {
    mCompilation = Compilation;
}

WebnnCompilation Compilation::GetCompilation() {
    return mCompilation;
}

void Compilation::SetOutputName(std::vector<std::string> outputName) {
    mOutputName = std::move(outputName);
}

Napi::Value Compilation::Compute(const Napi::CallbackInfo& info) {
    // The WebnnInput struct need to be kept until compute.
    std::map<std::string, WebnnInput> inputs;
    if (info[0].IsObject()) {
        Napi::Object obj = info[0].As<Napi::Object>();
        inputs = GetNamedOperands<WebnnInput>(obj);
    }

    // The WebnnOutput struct need to be kept until compute.
    std::map<std::string, WebnnOutput> outputs;
    if (info.Length() > 1 && info[1].IsObject()) {
        Napi::Object obj = info[1].As<Napi::Object>();
        outputs = GetNamedOperands<WebnnOutput>(obj);
    }
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);

    mComputeWorker = new ComputeAsyncWorker(env, deferred, mCompilation, std::move(inputs),
                                            std::move(outputs), std::move(mOutputName));
    mComputeWorker->Queue();
    return deferred.Promise();
}

Napi::Object Compilation::Initialize(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "Compilation", {InstanceMethod("compute", &Compilation::Compute, napi_enumerable)});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Compilation", func);
    return exports;
}
