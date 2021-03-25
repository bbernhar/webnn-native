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

#include "ops/Input.h"

#include <iostream>
#include <unordered_map>

#include "Utils.h"

namespace op {

    Input::Input(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder)
        : OperandBase(), mDescriptor({}) {
        Napi::Env env = info.Env();
        if (info.Length() != 2) {
            WEBNN_THROW(env, "Input operation need two arguments.");
            return;
        }
        // name
        if (!info[0].IsString()) {
            WEBNN_THROW(env, "The first argument must be String.");
            return;
        }
        mName = info[0].As<Napi::String>().Utf8Value();

        // operandDescriptor
        if (!info[1].IsObject()) {
            WEBNN_THROW(env, "The second argument must be Object.");
            return;
        }
        Napi::Object inputDescriptor = info[1].As<Napi::Object>();
        mDescriptor.type = OperandType(inputDescriptor.Get("type"));
        if (mDescriptor.type == WebnnOperandType_Force32) {
            WEBNN_THROW(env, "The type of operand descriptor isn't expected.");
            return;
        }
        // dimensions
        if (inputDescriptor.Has("dimensions")) {
            Napi::Value value = inputDescriptor.Get("dimensions");
            mDimensions = GetDimensions(value);
            if (mDimensions.size() == 0) {
                WEBNN_THROW(info.Env(), "Failed to get dimensions.");
                return;
            }
            mDescriptor.dimensions = mDimensions.data();
            mDescriptor.dimensionsCount = mDimensions.size();
        }

        OperandBase::SetOperand(webnnModelBuilderInput(modelBuilder, mName.c_str(), &mDescriptor));
    }

}  // namespace op
