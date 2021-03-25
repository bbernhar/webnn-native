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

#include "Transpose.h"

#include <iostream>

#include "Utils.h"

namespace op {

    Transpose::Transpose(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder)
        : mOptions({}) {
        const auto& inputs = GetInputs(info);
        if (inputs.size() != 1) {
            WEBNN_THROW(info.Env(), "The operation need one inputs.");
            return;
        }

        // There is some option in the struct to parse.
        if (info.Length() == 2) {
            if (!info[1].IsObject()) {
                WEBNN_THROW(info.Env(), "The option argument must be Object.");
                return;
            }
            Napi::Object obj = info[1].As<Napi::Object>();
            Napi::Array propertyNames = obj.GetPropertyNames();
            for (size_t j = 0; j < propertyNames.Length(); ++j) {
                std::string name = propertyNames.Get(j).As<Napi::String>().Utf8Value();
                Napi::Value item = static_cast<Napi::Value>(obj.Get(name));
                if (name == "permutation") {
                    mPermutation = GetDimensions(item);
                    if (mPermutation.size() != 4) {
                        WEBNN_THROW(info.Env(), "Failed to get dimensions.");
                        return;
                    }
                    mOptions.permutation = mPermutation.data();
                    mOptions.permutationCount = mPermutation.size();
                } else {
                    WEBNN_THROW(info.Env(), "The option isn't supported.");
                    return;
                }
            }
        }

        OperandBase::SetOperand(webnnModelBuilderTranspose(modelBuilder, inputs[0], &mOptions));
    }

}  // namespace op
