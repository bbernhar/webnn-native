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

#include "webnn_native/Graph.h"

#include <string>

#include "common/Assert.h"
#include "common/RefCounted.h"

namespace webnn_native {

    GraphBase::GraphBase(ContextBase* context) : ObjectBase(context) {
    }

    void GraphBase::Compute(NamedInputsBase* inputs,
                            MLComputeGraphCallback callback,
                            void* userdata,
                            NamedOutputsBase* outputs) {
        ComputeImpl(inputs, callback, userdata, outputs);
    }

    MaybeError GraphBase::AddConstant(const op::Constant* constant) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddInput(const op::Input* input) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddOutput(const std::string& name, const OperandBase* output) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddBinary(const op::Binary* binary) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddConv2d(const op::Conv2d* conv2d) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddPool2d(const op::Pool2d* pool2d) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddReshape(const op::Reshape* relu) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddTranspose(const op::Transpose* transpose) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddUnary(const op::Unary* unary) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddBatchNorm(const op::BatchNorm* batchNorm) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddLeakyRelu(const op::LeakyRelu* unary) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddConcat(const op::Concat* concat) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddGemm(const op::Gemm* gemm) {
        UNREACHABLE();
    }

    MaybeError GraphBase::AddClamp(const op::Clamp* clamp) {
        UNREACHABLE();
    }

    MaybeError GraphBase::Finish() {
        UNREACHABLE();
    }

    void GraphBase::Compile(BuildGraphCallbackDelgate delgate) {
        CompileImpl(delgate);
    }

}  // namespace webnn_native
