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

#ifndef WEBNN_NATIVE_ONEDNN_COMPILATION_DNNL_H_
#define WEBNN_NATIVE_ONEDNN_COMPILATION_DNNL_H_

#include "webnn_native/Compilation.h"
#include "webnn_native/onednn/ModelDNNL.h"

namespace webnn_native { namespace onednn {

    class Compilation : public CompilationBase {
      public:
        explicit Compilation(const Ref<Model>& model);
        ~Compilation() override = default;

      private:
        void ComputeImpl(NamedInputsBase* inputs,
                         WebnnComputeCallback callback,
                         void* userdata,
                         NamedOutputsBase* outputs = nullptr) override;

        Ref<Model> mModel;
    };

}}  // namespace webnn_native::onednn

#endif  // WEBNN_NATIVE_ONEDNN_COMPILATION_DNNL_H_
