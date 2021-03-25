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

#ifndef __OPS_CONSTANT_H_
#define __OPS_CONSTANT_H_

#include <napi.h>

#include "ops/OperandBase.h"

namespace op {

    union Scalar {
        float floatValue;
        int32_t int32Value;
        uint32_t uint32Value;
    };

    class Constant final : public OperandBase {
      public:
        Constant(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder);
        ~Constant() = default;

      private:
        WebnnOperandDescriptor mDescriptor;
        std::vector<int32_t> mDimensions;
        Napi::ObjectReference mConstantReference;
        void const* mValue;
        size_t mSize;
        Scalar mScalarValue;
    };

}  // namespace op

#endif  // __OPS_CONSTANT_H_
