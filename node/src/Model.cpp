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

#include "Model.h"

#include "Utils.h"

#include "Compilation.h"

namespace node {

    class CompileAsyncWorker : public Napi::AsyncWorker {
      public:
        CompileAsyncWorker(Napi::Env& env,
                           Napi::Promise::Deferred& deferred,
                           webnn::Model model,
                           const std::vector<std::string>& outputNames)
            : Napi::AsyncWorker(env),
              mEnv(env),
              mDeferred(deferred),
              mModel(model),
              mOutputNames(outputNames) {
        }

        ~CompileAsyncWorker() = default;

        void Execute() {
            mModel.Compile(
                [](WebnnCompileStatus status, WebnnCompilation compilation, char const* message,
                   void* userData) {
                    CompileAsyncWorker* asyncWorker =
                        reinterpret_cast<CompileAsyncWorker*>(userData);
                    asyncWorker->SetCompilation(status, compilation, message);
                },
                reinterpret_cast<void*>(this), nullptr);
        }

        void OnOK() {
            if (mStatus != webnn::CompileStatus::Success) {
                return mDeferred.Reject(Napi::Value::From(mEnv, mMessage));
            }
            Napi::Object object = Compilation::constructor.New({});
            Compilation* compilation = Napi::ObjectWrap<Compilation>::Unwrap(object);
            compilation->mImpl = mCompilation;
            compilation->mOutputNames = std::move(mOutputNames);
            mDeferred.Resolve(object);
        }

        void SetCompilation(WebnnCompileStatus status,
                            WebnnCompilation compilation,
                            char const* message) {
            mStatus = static_cast<webnn::CompileStatus>(status);
            mCompilation = mCompilation.Acquire(compilation);
            if (message) {
                mMessage = std::string(message);
            }
        }

      private:
        Napi::Env mEnv;
        Napi::Promise::Deferred mDeferred;
        webnn::Model mModel;
        std::vector<std::string> mOutputNames;
        webnn::CompileStatus mStatus;
        std::string mMessage;
        webnn::Compilation mCompilation;
    };

    Napi::FunctionReference Model::constructor;

    Model::Model(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Model>(info) {
    }

    webnn::Model Model::GetImpl() {
        return mImpl;
    }

    Napi::Value Model::Compile(const Napi::CallbackInfo& info) {
        // Promise<Compilation> compile(optional CompilationOptions options = {});
        WEBNN_NODE_ASSERT(info.Length() == 0 || info.Length() == 1,
                          "The number of arguments is invalid.");

        // FIXME: handle CompilationOptions.

        Napi::Env env = info.Env();
        auto deferred = Napi::Promise::Deferred::New(env);
        CompileAsyncWorker* worker = new CompileAsyncWorker(env, deferred, mImpl, mOutputNames);
        worker->Queue();
        return deferred.Promise();
    }

    Napi::Object Model::Initialize(Napi::Env env, Napi::Object exports) {
        Napi::HandleScope scope(env);
        Napi::Function func = DefineClass(
            env, "Model", {InstanceMethod("compile", &Model::Compile, napi_enumerable)});
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        exports.Set("Model", func);
        return exports;
    }

}  // namespace node
