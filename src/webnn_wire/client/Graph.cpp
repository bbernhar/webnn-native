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

#include "webnn_wire/client/Graph.h"

#include "webnn_wire/WireCmd_autogen.h"
#include "webnn_wire/client/ApiObjects_autogen.h"
#include "webnn_wire/client/Client.h"

namespace webnn_wire { namespace client {

    MLComputeGraphStatus Graph::Compute(MLNamedInputs inputs, MLNamedOutputs outputs) {
        NamedInputs* namedInputs = FromAPI(inputs);
        NamedOutputs* namedOutputs = FromAPI(outputs);

        GraphComputeCmd cmd;
        cmd.graphId = this->id;
        cmd.inputsId = namedInputs->id;
        cmd.outputsId = namedOutputs->id;

        client->SerializeCommand(cmd);

        return MLComputeGraphStatus::MLComputeGraphStatus_Success;
    }

}}  // namespace webnn_wire::client
