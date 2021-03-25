#include "Gemm.h"
#include <iostream>

#include <assert.h>
#include <iostream>
#include <unordered_map>
#include "Utils.h"

namespace op {
    Gemm::Gemm(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder) {
        const auto& inputs = GetInputs(info);
        if (inputs.size() != 2) {
            WEBNN_THROW(info.Env(), "The operation need two inputs.");
            return;
        }

        WebnnGemmOptions options;
        options.alpha = 1.0;
        options.beta = 1.0;
        options.aTranspose = false;
        options.bTranspose = false;
        options.c = NULL;
        if (info.Length() == 3) {
            Napi::Value value = static_cast<Napi::Value>(info[2]);
            if (!value.IsObject()) {
                WEBNN_THROW(info.Env(), "The third parameter shoud be a object");
                return;
            }
            Napi::Object obj = value.As<Napi::Object>();
            Napi::Array propertyNames = obj.GetPropertyNames();
            for (size_t j = 0; j < propertyNames.Length(); ++j) {
                std::string name = propertyNames.Get(j).As<Napi::String>().Utf8Value();
                if (name == "alpha") {
                    if (!obj.Get(name).IsNumber()) {
                        WEBNN_THROW(info.Env(), "The parameter 'a' should be a number");
                        return;
                    }

                    options.alpha = obj.Get(name).As<Napi::Number>().FloatValue();
                } else if (name == "beta") {
                    if (!obj.Get(name).IsNumber()) {
                        WEBNN_THROW(info.Env(), "The parameter 'b' should be a number");
                        return;
                    }

                    options.beta = obj.Get(name).As<Napi::Number>().FloatValue();
                } else if (name == "aTranspose") {
                    if (!obj.Get(name).IsBoolean()) {
                        WEBNN_THROW(info.Env(), "The parameter 'aTranspose' should be a boolean");
                        return;
                    }

                    options.aTranspose = obj.Get(name).As<Napi::Boolean>().Value();
                } else if (name == "bTranspose") {
                    if (!obj.Get(name).IsBoolean()) {
                        WEBNN_THROW(info.Env(), "The parameter 'bTranspose' should be a boolean");
                        return;
                    }

                    options.bTranspose = obj.Get(name).As<Napi::Boolean>().Value();
                } else if (name == "c") {
                    Napi::Object c = obj.Get(name).As<Napi::Object>();
                    if (c.InstanceOf(Operand::constructor.Value())) {
                        Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(c);
                        options.c = operand->GetOperand();
                    }
                } else {
                    WEBNN_THROW(info.Env(), "The option isn't supported.");
                    return;
                }
            }
        }

        OperandBase::SetOperand(
            webnnModelBuilderGemm(modelBuilder, inputs[0], inputs[1], &options));
    }
}  // namespace op