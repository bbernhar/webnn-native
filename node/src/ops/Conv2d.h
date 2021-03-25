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

#ifndef ___OPS_CONV2D_H__
#define ___OPS_CONV2D_H__

#include <unordered_map>

#include "ops/OperandBase.h"

namespace op {

    class Conv2d final : public OperandBase {
      public:
        Conv2d(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder);
        ~Conv2d() = default;

      private:
        WebnnConv2dOptions mOptions;
        std::vector<int32_t> mPadding;
        std::vector<int32_t> mStride;
        std::vector<int32_t> mDilations;
    };

}  // namespace op

#endif  // ___OPS_CONV2D_H__
