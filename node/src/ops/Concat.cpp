#include "Concat.h"

#include <napi.h>
#include <iostream>
#include "Operand.h"
#include "Utils.h"

namespace op {
    Concat::Concat(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder) {
        if (info.Length() != 2) {
            WEBNN_THROW(info.Env(), "The operation must have two parameters.");
            return;
        }

        const auto& inputs = GetInputs(info);
        if (inputs.size() < 2) {
            WEBNN_THROW(info.Env(), "The operation need more than one input.");
            return;
        }

        Napi::Value value = static_cast<Napi::Value>(info[1]);
        if (!value.IsNumber()) {
            WEBNN_THROW(info.Env(), "The axis must be number.");
            return;
        }

        Napi::Number number = value.As<Napi::Number>();
        int32_t axis = number.Int32Value();
        if (axis < 0) {
            WEBNN_THROW(info.Env(), "The axis should not be less than 0");
            return;
        }

        OperandBase::SetOperand(
            webnnModelBuilderConcat(modelBuilder, inputs.size(), inputs.data(), axis));
    }
}  // namespace op