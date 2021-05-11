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

#include "ops/BatchNorm.h"

#include "Operand.h"
#include "Utils.h"

namespace node { namespace op {

    Napi::Value BatchNorm::Build(const Napi::CallbackInfo& info, ml::GraphBuilder builder) {
        // Operand batchNormalization(Operand input, Operand mean, Operand variance,
        //                            optional BatchNormalizationOptions options = {});
        WEBNN_NODE_ASSERT(info.Length() == 3 || info.Length() == 4,
                          "The number of arguments is invalid.");

        ml::Operand input;
        WEBNN_NODE_ASSERT(GetOperand(info[0], input), "The input parameter is invalid.");
        ml::Operand mean;
        WEBNN_NODE_ASSERT(GetOperand(info[1], mean), "The mean parameter is invalid.");
        ml::Operand variance;
        WEBNN_NODE_ASSERT(GetOperand(info[2], variance), "The variance parameter is invalid.");

        ml::Operand batchNorm;
        if (info.Length() == 3) {
            batchNorm = builder.BatchNorm(input, mean, variance);
        } else if (info.Length() == 4) {
            // dictionary BatchNormalizationOptions {
            //   Operand scale;
            //   Operand bias;
            //   long axis = 1;
            //   float epsilon = 1e-5;
            // };
            ml::BatchNormOptions options;
            WEBNN_NODE_ASSERT(info[3].IsObject(), "The options must be an object.")
            Napi::Object jsOptions = info[3].As<Napi::Object>();
            if (HasOptionMember(jsOptions, "scale")) {
                WEBNN_NODE_ASSERT(GetOperand(jsOptions.Get("scale"), options.scale),
                                  "The scale parameter is invalid.");
            }
            if (HasOptionMember(jsOptions, "bias")) {
                WEBNN_NODE_ASSERT(GetOperand(jsOptions.Get("bias"), options.bias),
                                  "The bias parameter is invalid.");
            }
            if (HasOptionMember(jsOptions, "axis")) {
                WEBNN_NODE_ASSERT(GetUint32(jsOptions.Get("axis"), options.axis),
                                  "The axis parameter is invalid.");
            }
            if (HasOptionMember(jsOptions, "epsilon")) {
                WEBNN_NODE_ASSERT(GetFloat(jsOptions.Get("epsilon"), options.epsilon),
                                  "The epsilon parameter is invalid.");
            }
            batchNorm = builder.BatchNorm(input, mean, variance, &options);
        }

        Napi::Object object = Operand::constructor.New({});
        Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);
        operand->SetImpl(batchNorm);
        return object;
    }
}}  // namespace node::op
