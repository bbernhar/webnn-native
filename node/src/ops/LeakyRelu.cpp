#include "LeakyRelu.h"

#include <napi.h>
#include <iostream>
#include "Operand.h"
#include "Utils.h"

namespace op {
    LeakyRelu::LeakyRelu(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder) {
        const auto& inputs = GetInputs(info);
        if (inputs.size() != 1) {
            WEBNN_THROW(info.Env(), "The operation only support one input.");
            return;
        }

        WebnnLeakyReluOptions options;
        options.alpha = 0.01;

        if (info.Length() == 2) {
            Napi::Value value = static_cast<Napi::Value>(info[1]);
            if (!value.IsObject()) {
                WEBNN_THROW(info.Env(), "The option shoud be a object");
                return;
            }
            Napi::Object obj = value.As<Napi::Object>();
            Napi::Array propertyNames = obj.GetPropertyNames();
            for (size_t i = 0; i < propertyNames.Length(); i++) {
                std::string name = propertyNames.Get(i).As<Napi::String>().Utf8Value();
                if (name == "alpha") {
                    if (!obj.Get(name).IsNumber()) {
                        WEBNN_THROW(info.Env(), "The parameter 'alpha' should be a number");
                        return;
                    }

                    options.alpha = obj.Get(name).As<Napi::Number>().FloatValue();
                } else {
                    WEBNN_THROW(info.Env(), "The option isn't supported.");
                    return;
                }
            }
        }

        OperandBase::SetOperand(webnnModelBuilderLeakyRelu(modelBuilder, inputs[0], &options));
    }
}  // namespace op