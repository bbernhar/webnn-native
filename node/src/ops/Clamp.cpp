#include "Clamp.h"

#include <napi.h>
#include <iostream>
#include "Operand.h"
#include "Utils.h"

namespace op {
    Clamp::Clamp(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder) {
        const auto& inputs = GetInputs(info);
        if (inputs.size() != 1) {
            WEBNN_THROW(info.Env(), "The operation only need one input.");
            return;
        }

        WebnnClampOptions options = {};
        if (info.Length() == 2) {
            Napi::Value value = static_cast<Napi::Value>(info[1]);
            if (!value.IsObject()) {
                WEBNN_THROW(info.Env(), "The option must be Object.");
                return;
            }
            Napi::Object obj = value.As<Napi::Object>();
            Napi::Array propertyNames = obj.GetPropertyNames();
            for (size_t i = 0; i < propertyNames.Length(); i++) {
                std::string name = propertyNames.Get(i).As<Napi::String>().Utf8Value();
                Napi::Value item = static_cast<Napi::Value>(obj.Get(name));
                if (!item.IsObject()) {
                    WEBNN_THROW(info.Env(), "The option argument must be Object.");
                    return;
                }

                Napi::Object clampValue = item.As<Napi::Object>();

                if (!clampValue.InstanceOf(Operand::constructor.Value())) {
                    WEBNN_THROW(info.Env(), "The option argument must be a WebnnOperand.");
                    return;
                }

                Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(clampValue);
                if (name == "minValue") {
                    options.minValue = operand->GetOperand();
                } else if (name == "maxValue") {
                    options.maxValue = operand->GetOperand();
                } else {
                    WEBNN_THROW(info.Env(), "The option isn't supported.");
                    return;
                }
            }
        }
        OperandBase::SetOperand(webnnModelBuilderClamp(modelBuilder, inputs[0], &options));
    }
}  // namespace op