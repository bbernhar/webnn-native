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

#include "NeuralNetworkContext.h"

#include <webnn/webnn_proc.h>
#include <webnn_native/WebnnNative.h>

#include "ModelBuilder.h"

Napi::FunctionReference NeuralNetworkContext::constructor;

NeuralNetworkContext::NeuralNetworkContext(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<NeuralNetworkContext>(info) {
    WebnnProcTable backendProcs = webnn_native::GetProcs();
    webnnProcSetProcs(&backendProcs);
    mContext = webnn_native::CreateNeuralNetworkContext();
    if (mContext == nullptr) {
        Napi::Env env = info.Env();
        Napi::Error::New(env, "Failed to create neural network context")
            .ThrowAsJavaScriptException();
        return;
    }
}

NeuralNetworkContext::~NeuralNetworkContext() {
    webnnNeuralNetworkContextRelease(mContext);
}

WebnnNeuralNetworkContext NeuralNetworkContext::GetContext() {
    return mContext;
}

Napi::Value NeuralNetworkContext::CreateModelBuilder(const Napi::CallbackInfo& info) {
    std::vector<napi_value> args = {info.This().As<Napi::Value>()};
    Napi::Object modelBuilder = ModelBuilder::constructor.New(args);

    return modelBuilder;
}

Napi::Object NeuralNetworkContext::Initialize(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func =
        DefineClass(env, "NeuralNetworkContext",
                    {InstanceMethod("createModelBuilder", &NeuralNetworkContext::CreateModelBuilder,
                                    napi_enumerable)});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("NeuralNetworkContext", func);
    return exports;
}
