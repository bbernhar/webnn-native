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

#ifndef __OPS_INPUT_H_
#define __OPS_INPUT_H_

#include <memory>
#include <string>

#include "OperandBase.h"

namespace op {

    class Input final : public OperandBase {
      public:
        Input(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder);
        ~Input() = default;

      private:
        std::string mName;
        std::vector<int32_t> mDimensions;
        WebnnOperandDescriptor mDescriptor;
    };

}  // namespace op

#endif  // __OPS_INPUT_H_
