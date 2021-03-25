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

#include "Conv2d.h"

#include <napi.h>
#include <iostream>
#include <unordered_map>

#include "Utils.h"

namespace op {

    Conv2d::Conv2d(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder) : mOptions({}) {
        const auto& inputs = GetInputs(info);
        if (inputs.size() != 2) {
            WEBNN_THROW(info.Env(), "The operation need two inputs.");
            return;
        }

        // There is some option in the struct to parse.
        if (info.Length() == 3) {
            if (!info[2].IsObject()) {
                WEBNN_THROW(info.Env(), "The option argument must be Object.");
                return;
            }
            Napi::Object obj = info[2].As<Napi::Object>();
            Napi::Array propertyNames = obj.GetPropertyNames();
            for (size_t j = 0; j < propertyNames.Length(); ++j) {
                std::string name = propertyNames.Get(j).As<Napi::String>().Utf8Value();
                Napi::Value item = static_cast<Napi::Value>(obj.Get(name));
                if (name == "padding") {
                    mPadding = GetDimensions(item);
                    if (mPadding.size() != 4) {
                        WEBNN_THROW(info.Env(), "Failed to get dimensions.");
                        return;
                    }
                    mOptions.padding = mPadding.data();
                    mOptions.paddingCount = mPadding.size();
                } else if (name == "strides") {
                    mStride = GetDimensions(item);
                    if (mStride.size() != 2) {
                        WEBNN_THROW(info.Env(), "Failed to get dimensions.");
                        return;
                    }
                    mOptions.strides = mStride.data();
                    mOptions.stridesCount = mStride.size();
                } else if (name == "dilations") {
                    mDilations = GetDimensions(item);
                    if (mDilations.size() != 2) {
                        WEBNN_THROW(info.Env(), "Failed to get dimensions.");
                        return;
                    }
                    mOptions.dilations = mDilations.data();
                    mOptions.dilationsCount = mDilations.size();
                } else if (name == "groups") {
                    if (!item.IsNumber()) {
                        WEBNN_THROW(info.Env(), "The groups must be number.");
                        return;
                    } else {
                        mOptions.groups = item.As<Napi::Number>().Int32Value();
                    }
                } else if (name == "layout") {
                    if (!item.IsString()) {
                        WEBNN_THROW(info.Env(), "The layout must be string.");
                        return;
                    } else {
                        mOptions.inputLayout = static_cast<WebnnInputOperandLayout>(
                            OperandLayout(item.As<Napi::String>().Utf8Value()));
                    }
                } else {
                    WEBNN_THROW(info.Env(), "The option isn't supported.");
                    return;
                }
            }
        }
        OperandBase::SetOperand(
            webnnModelBuilderConv2d(modelBuilder, inputs[0], inputs[1], &mOptions));
    }

}  // namespace op
