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

#include "ops/LeakyRelu.h"

#include "Utils.h"

namespace node { namespace op {

    Napi::Value LeakyRelu::Build(const Napi::CallbackInfo& info, webnn::ModelBuilder builder) {
        // partial interface ModelBuilder {
        //   Operand leakyRelu(Operand x, optional LeakyReluOptions options = {});
        // };
        WEBNN_NODE_ASSERT(info.Length() == 1 || info.Length() == 2,
                          "The number of arguments is invalid.");

        webnn::Operand input;
        WEBNN_NODE_ASSERT(GetOperand(info[0], input), "The input parameter is invalid.");

        webnn::Operand leakyRelu;
        if (info.Length() == 1) {
            leakyRelu = builder.LeakyRelu(input);
        } else {
            // dictionary LeakyReluOptions {
            //   float alpha = 0.01;
            // };
            webnn::LeakyReluOptions options;
            WEBNN_NODE_ASSERT(info[1].IsObject(), "The options must be an object.");
            Napi::Object jsOptions = info[1].As<Napi::Object>();
            if (HasOptionMember(jsOptions, "alpha")) {
                WEBNN_NODE_ASSERT(GetFloat(jsOptions.Get("alpha"), options.alpha),
                                  "The alpha parameter is invalid.");
            }
            leakyRelu = builder.LeakyRelu(input, &options);
        }

        Napi::Object object = Operand::constructor.New({});
        Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);
        operand->SetImpl(leakyRelu);
        return object;
    }
}}  // namespace node::op