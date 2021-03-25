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

#ifndef __UTILS_H__
#define __UTILS_H__

#define NAPI_EXPERIMENTAL
#include <napi.h>
#include <unordered_map>

#include "Operand.h"

#define WEBNN_THROW(env, messages) Napi::Error::New(env, messages).ThrowAsJavaScriptException();

#define WEBNN_THROW_ONE_INPUT(size, env)                   \
    if (size != 1) {                                       \
        WEBNN_THROW(env, "The operation need one input."); \
        return env.Null();                                 \
    }

#define WEBNN_THROW_TWO_INPUT(size, env)                   \
    if (size != 2) {                                       \
        WEBNN_THROW(env, "The operation need two input."); \
        return env.Null();                                 \
    }

inline char* getNAPIStringCopy(const Napi::Value& value) {
    std::string utf8 = value.ToString().Utf8Value();
    int len = utf8.length() + 1;  // +1 NULL
    char* str = new char[len];
    strncpy(str, utf8.c_str(), len);
    return str;
};

inline std::vector<WebnnOperand> GetInputs(const Napi::CallbackInfo& info) {
    std::vector<WebnnOperand> inputs;
    inputs.reserve(info.Length());
    for (size_t i = 0; i < info.Length(); ++i) {
        if (info[i].IsArray()) {
            Napi::Array array = info[i].As<Napi::Array>();
            for (size_t j = 0; j < array.Length(); j++) {
                if (array.Get(j).IsObject()) {
                    Napi::Object object = array.Get(j).As<Napi::Object>();
                    if (object.InstanceOf(Operand::constructor.Value())) {
                        Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);
                        inputs.push_back(operand->GetOperand());
                    }
                }
            }
        } else if (info[i].IsObject()) {
            Napi::Object object = info[i].As<Napi::Object>();
            if (object.InstanceOf(Operand::constructor.Value())) {
                Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);
                inputs.push_back(operand->GetOperand());
            }
        }
    }
    return inputs;
}

static std::unordered_map<std::string, WebnnOperandType> s_operand_type_map = {
    {"float32", WebnnOperandType_Float32},
    {"float16", WebnnOperandType_Float16},
    {"int32", WebnnOperandType_Int32},
    {"uint32", WebnnOperandType_Uint32},
};

inline WebnnOperandType OperandType(const Napi::Value& value) {
    if (!value.IsString()) {
        return WebnnOperandType_Force32;
    }
    std::string type = value.As<Napi::String>().Utf8Value();
    if (s_operand_type_map.find(type) == s_operand_type_map.end()) {
        return WebnnOperandType_Force32;
    }
    return s_operand_type_map[type];
}

inline std::vector<int32_t> GetDimensions(const Napi::Value& value) {
    if (!value.IsArray()) {
        // errorMessages = "The type of the option must be array.";
        return {};
    }
    Napi::Array array = value.As<Napi::Array>();
    size_t rank = array.Length();
    if (rank == 0) {
        // errorMessages = "The dimensions is empty.";
        return {};
    }
    std::vector<int32_t> dimensions;
    dimensions.reserve(rank);
    for (uint32_t i = 0; i < rank; ++i) {
        Napi::Value dimension = static_cast<Napi::Value>(array[i]);
        if (!dimension.IsNumber()) {
            // errorMessages = "The dimension must be number.";
            return {};
        }
        dimensions.push_back(dimension.As<Napi::Number>().Int32Value());
    }
    return dimensions;
}

static std::unordered_map<std::string, uint32_t> OperandLayoutMap = {
    {"nchw", 0},
    {"nhwc", 1},
};

inline uint32_t OperandLayout(std::string name) {
    return OperandLayoutMap[name];
};

#endif  // __UTILS_H__
