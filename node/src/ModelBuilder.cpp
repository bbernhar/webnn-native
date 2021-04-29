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
#include "ops/Pool2d.h"
#include "ops/Reshape.h"
#include "ops/Transpose.h"

Napi::FunctionReference node::ModelBuilder::constructor;

#define BUILD_BINARY(op)                                                          \
    WEBNN_NODE_ASSERT(info.Length() == 2, "The number of arguments is invalid."); \
    webnn::Operand a;                                                             \
    WEBNN_NODE_ASSERT(GetOperand(info[0], a), "The a parameter is invalid.");     \
    webnn::Operand b;                                                             \
    WEBNN_NODE_ASSERT(GetOperand(info[1], b), "The a parameter is invalid.");     \
    Napi::Object object = Operand::constructor.New({});                           \
    Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);                 \
    operand->SetImpl(mImpl.op(a, b));                                             \
    return object;

#define BUILD_UNARY(op)                                                               \
    WEBNN_NODE_ASSERT(info.Length() == 1, "The number of arguments is invalid.");     \
    webnn::Operand input;                                                             \
    WEBNN_NODE_ASSERT(GetOperand(info[0], input), "The input parameter is invalid."); \
    Napi::Object object = Operand::constructor.New({});                               \
    Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);                     \
    operand->SetImpl(mImpl.op(input));                                                \
    return object;

namespace node {

    ModelBuilder::ModelBuilder(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<ModelBuilder>(info) {
        Napi::Object object = info[0].As<Napi::Object>();
        NeuralNetworkContext* context = Napi::ObjectWrap<NeuralNetworkContext>::Unwrap(object);
        mImpl = context->GetImpl().CreateModelBuilder();
    }

    Napi::Value ModelBuilder::Constant(const Napi::CallbackInfo& info) {
        return op::Constant::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::Input(const Napi::CallbackInfo& info) {
        return op::Input::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::Add(const Napi::CallbackInfo& info) {
        BUILD_BINARY(Add);
    }

    Napi::Value ModelBuilder::Mul(const Napi::CallbackInfo& info) {
        BUILD_BINARY(Mul);
    }

    Napi::Value ModelBuilder::Matmul(const Napi::CallbackInfo& info) {
        BUILD_BINARY(Matmul);
    }

    Napi::Value ModelBuilder::BatchNorm(const Napi::CallbackInfo& info) {
        return op::BatchNorm::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::Conv2d(const Napi::CallbackInfo& info) {
        return op::Conv2d::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::Concat(const Napi::CallbackInfo& info) {
        return op::Concat::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::Gemm(const Napi::CallbackInfo& info) {
        return op::Gemm::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::Clamp(const Napi::CallbackInfo& info) {
        return op::Clamp::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::MaxPool2d(const Napi::CallbackInfo& info) {
        return op::Pool2d::Build(info, mImpl, op::Pool2dType::kMaxPool2d);
    }

    Napi::Value ModelBuilder::AveragePool2d(const Napi::CallbackInfo& info) {
        return op::Pool2d::Build(info, mImpl, op::Pool2dType::kAveragePool2d);
    }

    Napi::Value ModelBuilder::Relu(const Napi::CallbackInfo& info) {
        BUILD_UNARY(Relu);
    }

    Napi::Value ModelBuilder::Softmax(const Napi::CallbackInfo& info) {
        BUILD_UNARY(Softmax);
    }

    Napi::Value ModelBuilder::LeakyRelu(const Napi::CallbackInfo& info) {
        return op::LeakyRelu::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::Reshape(const Napi::CallbackInfo& info) {
        return op::Reshape::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::Transpose(const Napi::CallbackInfo& info) {
        return op::Transpose::Build(info, mImpl);
    }

    Napi::Value ModelBuilder::CreateModel(const Napi::CallbackInfo& info) {
        // Model createModel(NamedOperands outputs);
        WEBNN_NODE_ASSERT(info.Length() == 1, "The number of arguments is invalid.");
        webnn::NamedOperands namedOperands;
        std::vector<std::string> names;
        WEBNN_NODE_ASSERT(GetNamedOperands(info[0], namedOperands, names),
                          "The outputs parameter is invalid.");
        Napi::Object object = Model::constructor.New({});
        Model* model = Napi::ObjectWrap<Model>::Unwrap(object);
        model->mImpl = mImpl.CreateModel(namedOperands);
        model->mOutputNames = std::move(names);
        return object;
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
             InstanceMethod("matmul", &ModelBuilder::Matmul, napi_enumerable),
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

}  // namespace node
