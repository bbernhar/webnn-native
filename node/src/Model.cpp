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

#include <iostream>

#include "Compilation.h"

class CompilationAsyncWorker : public Napi::AsyncWorker {
  public:
    CompilationAsyncWorker(Napi::Env& env,
                           Napi::Promise::Deferred& deferred,
                           WebnnModel model,
                           std::vector<std::string> outputName)
        : Napi::AsyncWorker(env),
          mEnv(env),
          mDeferred(deferred),
          mModel(model),
          mOutputName(std::move(outputName)) {
    }

    ~CompilationAsyncWorker(){};

    void Execute() {
        webnnModelCompile(
            mModel,
            [](WebnnCompileStatus status, WebnnCompilation compilation, char const* message,
               void* userData) {
                CompilationAsyncWorker* asyncWorker =
                    reinterpret_cast<CompilationAsyncWorker*>(userData);
                asyncWorker->SetCompilation(compilation);
            },
            reinterpret_cast<void*>(this), nullptr);
    }

    void OnOK() {
        if (mCompilation == nullptr) {
            return mDeferred.Resolve(Env().Null());
        }
        Napi::Object obj = Compilation::constructor.New({});
        Compilation* unwapper = Napi::ObjectWrap<Compilation>::Unwrap(obj);
        unwapper->SetCompilation(mCompilation);
        unwapper->SetOutputName(std::move(mOutputName));
        mDeferred.Resolve(obj);
    }

  private:
    void SetCompilation(WebnnCompilation compilation) {
        mCompilation = compilation;
    }

    Napi::Env mEnv;
    Napi::Promise::Deferred mDeferred;
    WebnnModel mModel;
    std::vector<std::string> mOutputName;
    WebnnCompilation mCompilation;
};

Napi::FunctionReference Model::constructor;

Model::Model(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Model>(info) {
    Napi::Object obj = info[0].As<Napi::Object>();
    Napi::Array propertyNames = obj.GetPropertyNames();
    for (size_t j = 0; j < propertyNames.Length(); ++j) {
        std::string name = propertyNames.Get(j).As<Napi::String>().Utf8Value();
        mOutputName.push_back(name);
    }
}

Model::~Model() {
    if (mModel) {
        webnnModelRelease(mModel);
    }
}

void Model::SetModel(WebnnModel model) {
    mModel = model;
}

WebnnModel Model::GetModel() {
    return mModel;
}

Napi::Value Model::Compile(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto deferred = Napi::Promise::Deferred::New(env);
    CompilationAsyncWorker* mCompilationWorker =
        new CompilationAsyncWorker(env, deferred, mModel, std::move(mOutputName));
    mCompilationWorker->Queue();

    return deferred.Promise();
}

Napi::Object Model::Initialize(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func =
        DefineClass(env, "Model", {InstanceMethod("compile", &Model::Compile, napi_enumerable)});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Model", func);
    return exports;
}
