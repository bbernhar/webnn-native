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

#include "webnn_native/onednn/NeuralNetworkContextDNNL.h"

#include "common/Log.h"
#include "common/RefCounted.h"
#include "webnn_native/onednn/ModelBuilderDNNL.h"

namespace webnn_native { namespace onednn {

    NeuralNetworkContextBase* Create() {
        Ref<NeuralNetworkContextBase> context = AcquireRef(new NeuralNetworkContext());
        dnnl_status_t status =
            reinterpret_cast<NeuralNetworkContext*>(context.Get())->CreateEngine();
        if (status != dnnl_success) {
            dawn::ErrorLog() << "Failed to create oneDNN engine.";
            return nullptr;
        }
        return context.Detach();
    }

    NeuralNetworkContext::NeuralNetworkContext() : mEngine(nullptr) {
    }

    NeuralNetworkContext::~NeuralNetworkContext() {
        if (mEngine != nullptr) {
            dnnl_engine_destroy(mEngine);
        }
    }

    dnnl_status_t NeuralNetworkContext::CreateEngine(dnnl_engine_kind_t engineKind) {
        return dnnl_engine_create(&mEngine, engineKind, 0);
    }

    GraphBuilderBase* NeuralNetworkContext::CreateModelBuilderImpl() {
        return new ModelBuilder(this);
    }

}}  // namespace webnn_native::onednn
