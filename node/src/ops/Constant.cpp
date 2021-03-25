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

#include <iostream>

#include "Utils.h"

namespace op {

    Constant::Constant(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder)
        : OperandBase(), mDescriptor({}), mDimensions({}), mValue(nullptr), mSize(0) {
        Napi::Env env = info.Env();
        Napi::Value scalar = info[0].As<Napi::Value>();
        // scalar
        if (scalar.IsNumber()) {
            if (info.Length() == 2 && info[1].IsString()) {
                Napi::Value scalaType = info[1].As<Napi::Value>();
                mDescriptor.type = OperandType(scalaType);
            } else {
                mDescriptor.type = WebnnOperandType_Float32;
            }
            mDimensions = {1};
            mDescriptor.dimensions = mDimensions.data();
            mDescriptor.dimensionsCount = mDimensions.size();
            Napi::Number number = scalar.As<Napi::Number>();

            if (mDescriptor.type == WebnnOperandType_Float32) {
                mScalarValue.floatValue = number.FloatValue();
                mValue = &mScalarValue.floatValue;
                mSize = sizeof(float);
            } else if (mDescriptor.type == WebnnOperandType_Int32) {
                mScalarValue.int32Value = number.Int32Value();
                mValue = &mScalarValue.int32Value;
                mSize = sizeof(int32_t);
            } else if (mDescriptor.type == WebnnOperandType_Uint32) {
                mScalarValue.uint32Value = number.Uint32Value();
                mValue = &mScalarValue.uint32Value;
                mSize = sizeof(uint32_t);
            } else {
                WEBNN_THROW(env, "The type of operand descriptor isn't expected.");
                return;
            }
        } else {
            // operandDescriptor
            Napi::Object constantDescriptor = info[0].As<Napi::Object>();
            mDescriptor.type = OperandType(constantDescriptor.Get("type"));
            if (mDescriptor.type != WebnnOperandType_Float32) {
                WEBNN_THROW(env, "The type of operand descriptor isn't expected.");
                return;
            }

            // dimensions
            if (constantDescriptor.Has("dimensions")) {
                Napi::Value value = constantDescriptor.Get("dimensions");
                mDimensions = GetDimensions(value);
                if (mDimensions.size() == 0) {
                    WEBNN_THROW(info.Env(), "Failed to get dimensions.");
                    return;
                }
                mDescriptor.dimensions = mDimensions.data();
                mDescriptor.dimensionsCount = mDimensions.size();
            }

            // constant value
            Napi::Value value = info[1].As<Napi::Value>();
            if (!value.IsTypedArray()) {
                WEBNN_THROW(env, "The value must be typed array.");
                return;
            }
            // Keep the constant reference.
            mConstantReference = Napi::Persistent(value.As<Napi::Object>());
            Napi::TypedArray arr = value.As<Napi::TypedArray>();
            Napi::ArrayBuffer buffer = arr.ArrayBuffer();
            size_t product = 1;
            mSize = arr.ByteLength();
            switch (mDescriptor.type) {
                case WebnnOperandType_Float32:
                    mValue = buffer.Data();
                    product = sizeof(float);
                    break;
                case WebnnOperandType_Int32:
                    mValue = buffer.Data();
                    product = sizeof(int32_t);
                    break;
                case WebnnOperandType_Uint32:
                    mValue = buffer.Data();
                    product = sizeof(uint32_t);
                    break;
                default:
                    // Unsupport Float16
                    WEBNN_THROW(env, "The type of operand descriptor isn't expected.");
                    return;
            }

            for (auto dim : mDimensions) {
                product *= dim;
            }
            if (product != mSize) {
                WEBNN_THROW(env, "invalid value length.");
                return;
            }
        }
        OperandBase::SetOperand(
            webnnModelBuilderConstant(modelBuilder, &mDescriptor, mValue, mSize));
    }

}  // namespace op
