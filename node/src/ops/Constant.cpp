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

#include "ops/Constant.h"

#include "Operand.h"
#include "Utils.h"

namespace node { namespace op {

    namespace {
        union Scalar {
            float floatValue;
            int32_t int32Value;
            uint32_t uint32Value;
        };
    }  // namespace

    Napi::Value Constant::Build(const Napi::CallbackInfo& info, ml::GraphBuilder builder) {
        //   Operand constant(OperandDescriptor desc, ArrayBufferView value);
        //   Operand constant(double value, optional OperandType type = "float32");
        WEBNN_NODE_ASSERT(info.Length() == 1 || info.Length() == 2,
                          "The number of arguments is invalid.");

        Napi::Object object = Operand::constructor.New({});
        OperandDescriptor desc;
        void* value;
        size_t size;
        Scalar scalar;
        if (info[0].IsNumber()) {
            // Operand constant(double value, optional OperandType type = "float32");
            if (info.Length() == 1) {
                desc.type = ml::OperandType::Float32;
            } else {
                WEBNN_NODE_ASSERT(GetOperandType(info[1], desc.type),
                                  "The type parameter is invalid.");
            }
            Napi::Number jsValue = info[0].As<Napi::Number>();
            if (desc.type == ml::OperandType::Float32) {
                scalar.floatValue = jsValue.FloatValue();
                value = &scalar.floatValue;
                size = sizeof(float);
            } else if (desc.type == ml::OperandType::Int32) {
                scalar.int32Value = jsValue.Int32Value();
                value = &scalar.int32Value;
                size = sizeof(int32_t);
            } else if (desc.type == ml::OperandType::Uint32) {
                scalar.uint32Value = jsValue.Uint32Value();
                value = &scalar.uint32Value;
                size = sizeof(uint32_t);
            } else {
                WEBNN_NODE_THROW_AND_RETURN("The operand type is not supported.");
            }
            desc.dimensions = {1};
        } else {
            WEBNN_NODE_ASSERT(GetOperandDescriptor(info[0], desc),
                              "The desc parameter is invalid.");
            WEBNN_NODE_ASSERT(GetBufferView(info[1], desc.type, desc.dimensions, value, size),
                              "The value parameter is invalid.");
            // Keep a reference of value.
            object.Set("value", info[1]);
        }

        Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);
        operand->SetImpl(builder.Constant(desc.AsPtr(), value, size));
        return object;
    }

}}  // namespace node::op
