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

#include "ops/Clamp.h"

#include "Operand.h"
#include "Utils.h"

namespace node { namespace op {

    Napi::Value Clamp::Build(const Napi::CallbackInfo& info, ml::GraphBuilder builder) {
        // Operand clamp(Operand x, optional ClampOptions options = {});
        WEBNN_NODE_ASSERT(info.Length() == 1 || info.Length() == 2,
                          "The number of arguments is invalid.");

        ml::Operand input;
        WEBNN_NODE_ASSERT(GetOperand(info[0], input), "The input parameter is invalid.");

        ml::Operand clamp;
        if (info.Length() == 1) {
            clamp = builder.Clamp(input);
        } else {
            // dictionary ClampOptions {
            //   Operand minValue;
            //   Operand maxValue;
            // };
            ml::ClampOptions options;
            WEBNN_NODE_ASSERT(info[1].IsObject(), "The options must be an object.");
            Napi::Object jsOptions = info[1].As<Napi::Object>();
            if (HasOptionMember(jsOptions, "minValue")) {
                WEBNN_NODE_ASSERT(GetOperand(jsOptions.Get("minValue"), options.minValue),
                                  "The minValue parameter is invalid.");
            }
            if (HasOptionMember(jsOptions, "maxValue")) {
                WEBNN_NODE_ASSERT(GetOperand(jsOptions.Get("maxValue"), options.maxValue),
                                  "The maxValue parameter is invalid.");
            }
            clamp = builder.Clamp(input, &options);
        }

        Napi::Object object = Operand::constructor.New({});
        Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);
        operand->SetImpl(clamp);
        return object;
    }

}}  // namespace node::op
