#include "BatchNorm.h"

#include <napi.h>
#include <iostream>
#include "Operand.h"
#include "Utils.h"

namespace op {
    BatchNorm::BatchNorm(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder) {
        const auto& inputs = GetInputs(info);
        if (inputs.size() != 3) {
            WEBNN_THROW(info.Env(), "The op needs three inputs.");
            return;
        }

        WebnnBatchNormOptions mOptions;

        if (info.Length() == 4) {
            if (!info[3].IsObject()) {
                WEBNN_THROW(info.Env(), "The option argument must be Object.");
                return;
            }
            mOptions.axis = 1;
            mOptions.epsilon = 0.00001;

            Napi::Object obj = info[3].As<Napi::Object>();
            Napi::Array propertyNames = obj.GetPropertyNames();
            for (size_t i = 0; i < propertyNames.Length(); ++i) {
                std::string name = propertyNames.Get(i).As<Napi::String>().Utf8Value();
                Napi::Value item = static_cast<Napi::Value>(obj.Get(name));

                if (name == "scale") {
                    Napi::Object scale = item.As<Napi::Object>();
                    if (scale.InstanceOf(Operand::constructor.Value())) {
                        Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(scale);
                        mOptions.scale = operand->GetOperand();
                    }
                } else if (name == "bias") {
                    Napi::Object bias = item.As<Napi::Object>();
                    if (bias.InstanceOf(Operand::constructor.Value())) {
                        Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(bias);
                        mOptions.bias = operand->GetOperand();
                    }
                } else if (name == "axis") {
                    if (!item.IsNumber()) {
                        WEBNN_THROW(info.Env(), "The parameter 'axis' should be a number");
                        return;
                    }

                    mOptions.axis = item.As<Napi::Number>().Int32Value();
                } else if (name == "epsilon") {
                    if (!item.IsNumber()) {
                        WEBNN_THROW(info.Env(), "The parameter 'epsilon' should be a number");
                        return;
                    }

                    mOptions.epsilon = item.As<Napi::Number>().FloatValue();
                } else {
                    WEBNN_THROW(info.Env(), "The option isn't supported.");
                    return;
                }
            }
        }

        OperandBase::SetOperand(
            webnnModelBuilderBatchNorm(modelBuilder, inputs[0], inputs[1], inputs[2],
                                       (info.Length() == 4) ? &mOptions : nullptr));
    }
}  // namespace op