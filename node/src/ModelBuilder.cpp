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

#include "ModelBuilder.h"

#include <iostream>
#include <vector>

#include "Model.h"
#include "NeuralNetworkContext.h"
#include "Operand.h"
#include "Utils.h"
#include "ops/BatchNorm.h"
#include "ops/Clamp.h"
#include "ops/Concat.h"
#include "ops/Constant.h"
#include "ops/Conv2d.h"
#include "ops/Gemm.h"
#include "ops/Input.h"
#include "ops/LeakyRelu.h"
#include "ops/OperandBase.h"
#include "ops/Pool2d.h"
#include "ops/Reshape.h"
#include "ops/Transpose.h"

Napi::FunctionReference ModelBuilder::constructor;

WebnnNamedOperands GetNamedOperands(Napi::Object obj) {
    WebnnNamedOperands namedOperands = webnnCreateNamedOperands();
    Napi::Array propertyNames = obj.GetPropertyNames();
    for (size_t j = 0; j < propertyNames.Length(); ++j) {
        std::string name = propertyNames.Get(j).As<Napi::String>().Utf8Value();
        Napi::Object item = obj.Get(name).As<Napi::Object>();
        if (!item.InstanceOf(Operand::constructor.Value())) {
            std::cout << "Expected 'Operand' for 'NamedOperand'" << std::endl;
            return namedOperands;
        }
        WebnnOperand operand = Napi::ObjectWrap<Operand>::Unwrap(item)->GetOperand();
        webnnNamedOperandsSet(namedOperands, name.data(), operand);
    }
    return namedOperands;
}

ModelBuilder::ModelBuilder(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<ModelBuilder>(info), mModelBuilder(nullptr) {
    Napi::Object context = info[0].As<Napi::Object>();
    NeuralNetworkContext* unwrapped = Napi::ObjectWrap<NeuralNetworkContext>::Unwrap(context);
    mModelBuilder = webnnNeuralNetworkContextCreateModelBuilder(unwrapped->GetContext());
}

ModelBuilder::~ModelBuilder() {
    if (mModelBuilder) {
        webnnModelBuilderRelease(mModelBuilder);
    }
}

Napi::Value ModelBuilder::Constant(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::Constant>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::Input(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::Input>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::Add(const Napi::CallbackInfo& info) {
    const auto& inputs = GetInputs(info);
    WEBNN_THROW_TWO_INPUT(inputs.size(), info.Env());

    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::OperandBase>(
        webnnModelBuilderAdd(mModelBuilder, inputs[0], inputs[1])));
    return object;
}

Napi::Value ModelBuilder::BatchNorm(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::BatchNorm>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::Mul(const Napi::CallbackInfo& info) {
    const auto& inputs = GetInputs(info);
    WEBNN_THROW_TWO_INPUT(inputs.size(), info.Env());

    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::OperandBase>(
        webnnModelBuilderMul(mModelBuilder, inputs[0], inputs[1])));
    return object;
}

Napi::Value ModelBuilder::MatMul(const Napi::CallbackInfo& info) {
    const auto& inputs = GetInputs(info);
    WEBNN_THROW_TWO_INPUT(inputs.size(), info.Env());

    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::OperandBase>(
        webnnModelBuilderMatmul(mModelBuilder, inputs[0], inputs[1])));
    return object;
}

Napi::Value ModelBuilder::Conv2d(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::Conv2d>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::Concat(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::Concat>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::Gemm(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::Gemm>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::Clamp(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::Clamp>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::MaxPool2d(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(
        std::make_shared<op::Pool2d>(info, mModelBuilder, op::Pool2dType::kMaxPool2d));
    return object;
}

Napi::Value ModelBuilder::AveragePool2d(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(
        std::make_shared<op::Pool2d>(info, mModelBuilder, op::Pool2dType::kAveragePool2d));
    return object;
}

Napi::Value ModelBuilder::Relu(const Napi::CallbackInfo& info) {
    const auto& inputs = GetInputs(info);
    WEBNN_THROW_ONE_INPUT(inputs.size(), info.Env());

    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(
        std::make_shared<op::OperandBase>(webnnModelBuilderRelu(mModelBuilder, inputs[0])));
    return object;
}

Napi::Value ModelBuilder::LeakyRelu(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::LeakyRelu>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::Reshape(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::Reshape>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::Softmax(const Napi::CallbackInfo& info) {
    const auto& inputs = GetInputs(info);
    WEBNN_THROW_ONE_INPUT(inputs.size(), info.Env());

    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(
        std::make_shared<op::OperandBase>(webnnModelBuilderSoftmax(mModelBuilder, inputs[0])));
    return object;
}

Napi::Value ModelBuilder::Transpose(const Napi::CallbackInfo& info) {
    Napi::Object object = Operand::constructor.New({});
    Operand* unwrapped = Napi::ObjectWrap<Operand>::Unwrap(object);
    unwrapped->SetOperand(std::make_shared<op::Transpose>(info, mModelBuilder));
    return object;
}

Napi::Value ModelBuilder::CreateModel(const Napi::CallbackInfo& info) {
    WebnnNamedOperands namedOperands = GetNamedOperands(info[0].As<Napi::Object>());
    std::vector<napi_value> args = {info[0].As<Napi::Value>()};
    Napi::Object model = Model::constructor.New(args);
    Model* unwrapped = Napi::ObjectWrap<Model>::Unwrap(model);
    unwrapped->SetModel(webnnModelBuilderCreateModel(mModelBuilder, namedOperands));

    return model;
}

Napi::Object ModelBuilder::Initialize(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(
        env, "ModelBuilder",
        {InstanceMethod("constant", &ModelBuilder::Constant, napi_enumerable),
         InstanceMethod("input", &ModelBuilder::Input, napi_enumerable),
         InstanceMethod("add", &ModelBuilder::Add, napi_enumerable),
         InstanceMethod("batchNormalization", &ModelBuilder::BatchNorm, napi_enumerable),
         InstanceMethod("mul", &ModelBuilder::Mul, napi_enumerable),
         InstanceMethod("matmul", &ModelBuilder::MatMul, napi_enumerable),
         InstanceMethod("concat", &ModelBuilder::Concat, napi_enumerable),
         InstanceMethod("conv2d", &ModelBuilder::Conv2d, napi_enumerable),
         InstanceMethod("clamp", &ModelBuilder::Clamp, napi_enumerable),
         InstanceMethod("gemm", &ModelBuilder::Gemm, napi_enumerable),
         InstanceMethod("maxPool2d", &ModelBuilder::MaxPool2d, napi_enumerable),
         InstanceMethod("averagePool2d", &ModelBuilder::AveragePool2d, napi_enumerable),
         InstanceMethod("relu", &ModelBuilder::Relu, napi_enumerable),
         InstanceMethod("leakyRelu", &ModelBuilder::LeakyRelu, napi_enumerable),
         InstanceMethod("reshape", &ModelBuilder::Reshape, napi_enumerable),
         InstanceMethod("softmax", &ModelBuilder::Softmax, napi_enumerable),
         InstanceMethod("transpose", &ModelBuilder::Transpose, napi_enumerable),
         InstanceMethod("createModel", &ModelBuilder::CreateModel, napi_enumerable)});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("ModelBuilder", func);
    return exports;
}
