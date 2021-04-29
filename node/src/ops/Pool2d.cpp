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

#include "ops/Pool2d.h"

#include "Operand.h"
#include "Utils.h"

namespace node { namespace op {

    struct Pool2dOptions {
      public:
        std::vector<int32_t> windowDimensions;
        std::vector<int32_t> padding;
        std::vector<int32_t> strides;
        std::vector<int32_t> dilations;
        webnn::InputOperandLayout layout = webnn::InputOperandLayout::Nchw;

        const webnn::Pool2dOptions* AsPtr() {
            if (!windowDimensions.empty()) {
                mOptions.windowDimensionsCount = windowDimensions.size();
                mOptions.windowDimensions = windowDimensions.data();
            }
            if (!padding.empty()) {
                mOptions.paddingCount = padding.size();
                mOptions.padding = padding.data();
            }
            if (!strides.empty()) {
                mOptions.stridesCount = strides.size();
                mOptions.strides = strides.data();
            }
            if (!dilations.empty()) {
                mOptions.dilationsCount = dilations.size();
                mOptions.dilations = dilations.data();
            }
            mOptions.layout = layout;
            return &mOptions;
        }

      private:
        webnn::Pool2dOptions mOptions;
    };

    Napi::Value Pool2d::Build(const Napi::CallbackInfo& info,
                              webnn::ModelBuilder builder,
                              Pool2dType type) {
        //   Operand averagePool2d(Operand input, optional Pool2dOptions options = {});
        //   Operand l2Pool2d(Operand input, optional Pool2dOptions options = {});
        //   Operand maxPool2d(Operand input, optional Pool2dOptions options = {});
        WEBNN_NODE_ASSERT(info.Length() == 1 || info.Length() == 2,
                          "The number of arguments is invalid.");

        webnn::Operand input;
        WEBNN_NODE_ASSERT(GetOperand(info[0], input), "The input parameter is invalid.");

        bool hasOptions = false;
        Pool2dOptions options;
        if (info.Length() == 2) {
            hasOptions = true;
            // dictionary Pool2dOptions {
            //   sequence<long> windowDimensions;
            //   sequence<long> padding;
            //   sequence<long> strides;
            //   sequence<long> dilations;
            //   AutoPad autoPad = "explicit";
            //   InputOperandLayout layout = "nchw";
            // };
            WEBNN_NODE_ASSERT(info[1].IsObject(), "The options must be an object.");
            Napi::Object jsOptions = info[1].As<Napi::Object>();
            if (HasOptionMember(jsOptions, "windowDimensions")) {
                WEBNN_NODE_ASSERT(
                    GetInt32Array(jsOptions.Get("windowDimensions"), options.windowDimensions, 2),
                    "The windowDimensions parameter is invalid.");
            }
            if (HasOptionMember(jsOptions, "padding")) {
                WEBNN_NODE_ASSERT(GetInt32Array(jsOptions.Get("padding"), options.padding, 4),
                                  "The padding parameter is invalid.");
            }
            if (HasOptionMember(jsOptions, "strides")) {
                WEBNN_NODE_ASSERT(GetInt32Array(jsOptions.Get("strides"), options.strides, 2),
                                  "The strides parameter is invalid.");
            }
            if (HasOptionMember(jsOptions, "dilations")) {
                WEBNN_NODE_ASSERT(GetInt32Array(jsOptions.Get("dilations"), options.dilations, 2),
                                  "The dilations parameter is invalid.");
            }
            if (HasOptionMember(jsOptions, "layout")) {
                WEBNN_NODE_ASSERT(GetInputOperandLayout(jsOptions.Get("layout"), options.layout),
                                  "The layout parameter is invalid.");
            }
        }

        webnn::Operand pool2d;
        switch (type) {
            case Pool2dType::kAveragePool2d:
                pool2d = builder.AveragePool2d(input, hasOptions ? options.AsPtr() : nullptr);
                break;
            case Pool2dType::kMaxPool2d:
                pool2d = builder.MaxPool2d(input, hasOptions ? options.AsPtr() : nullptr);
                break;
            default:
                WEBNN_NODE_THROW_AND_RETURN("The type of pool2d is not supported.");
        }

        Napi::Object object = Operand::constructor.New({});
        Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);
        operand->SetImpl(pool2d);
        return object;
    }

}}  // namespace node::op
