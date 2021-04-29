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

#ifndef NODE_UTILS_H_
#define NODE_UTILS_H_

#define NAPI_EXPERIMENTAL
#include <napi.h>
#include <unordered_map>

#include "Operand.h"

#define WEBNN_NODE_THROW_AND_RETURN(message)                            \
    Napi::Error::New(info.Env(), message).ThrowAsJavaScriptException(); \
    return info.Env().Undefined();

#define WEBNN_NODE_ASSERT(condition, message) \
    if (!(condition)) {                       \
        WEBNN_NODE_THROW_AND_RETURN(message); \
    }

namespace node {

    inline bool GetInt32Array(const Napi::Value& jsValue,
                              std::vector<int32_t>& array,
                              const size_t size = std::numeric_limits<size_t>::max()) {
        if (!jsValue.IsArray()) {
            return false;
        }
        Napi::Array jsArray = jsValue.As<Napi::Array>();
        if (size != std::numeric_limits<size_t>::max() && size != jsArray.Length()) {
            return false;
        }
        std::vector<int32_t> int32Array;
        for (uint32_t i = 0; i < jsArray.Length(); ++i) {
            Napi::Value jsItem = static_cast<Napi::Value>(jsArray[i]);
            if (!jsItem.IsNumber()) {
                return false;
            }
            int32Array.push_back(jsItem.As<Napi::Number>().Int32Value());
        }
        array = int32Array;
        return true;
    }

    template <class T>
    bool GetMappedValue(const std::unordered_map<std::string, T> map,
                        const std::string name,
                        T& value) {
        if (map.find(name) == map.end()) {
            return false;
        }
        value = map.at(name);
        return true;
    }

    inline bool GetOperandType(const Napi::Value& jsValue, webnn::OperandType& value) {
        const std::unordered_map<std::string, webnn::OperandType> operandTypeMap = {
            {"float32", webnn::OperandType::Float32},
            {"float16", webnn::OperandType::Float16},
            {"int32", webnn::OperandType::Int32},
            {"uint32", webnn::OperandType::Uint32},
        };
        if (!jsValue.IsString()) {
            return false;
        }
        return GetMappedValue(operandTypeMap, jsValue.As<Napi::String>().Utf8Value(), value);
    }

    inline bool GetInputOperandLayout(const Napi::Value& jsValue,
                                      webnn::InputOperandLayout& value) {
        const std::unordered_map<std::string, webnn::InputOperandLayout> inputOperandLayoutMap = {
            {"nchw", webnn::InputOperandLayout::Nchw},
            {"nhwc", webnn::InputOperandLayout::Nhwc},
        };
        if (!jsValue.IsString()) {
            return false;
        }
        return GetMappedValue(inputOperandLayoutMap, jsValue.As<Napi::String>().Utf8Value(), value);
    };

    inline bool GetFilterOperandLayout(const Napi::Value& jsValue,
                                       webnn::FilterOperandLayout& value) {
        const std::unordered_map<std::string, webnn::FilterOperandLayout> filterOperandLayoutMap = {
            {"oihw", webnn::FilterOperandLayout::Oihw},
            {"hwio", webnn::FilterOperandLayout::Hwio},
            {"ohwi", webnn::FilterOperandLayout::Ohwi},
        };
        if (!jsValue.IsString()) {
            return false;
        }
        return GetMappedValue(filterOperandLayoutMap, jsValue.As<Napi::String>().Utf8Value(),
                              value);
    };

    inline bool GetAutopad(const Napi::Value& jsValue, webnn::AutoPad& value) {
        const std::unordered_map<std::string, webnn::AutoPad> AutoPadMap = {
            {"explicit", webnn::AutoPad::Explicit},
            {"same-upper", webnn::AutoPad::SameUpper},
            {"same-lower", webnn::AutoPad::SameLower},
        };
        if (!jsValue.IsString()) {
            return false;
        }
        return GetMappedValue(AutoPadMap, jsValue.As<Napi::String>().Utf8Value(), value);
    };

    inline bool GetInt32(const Napi::Value& jsValue, int32_t& value) {
        if (!jsValue.IsNumber()) {
            return false;
        }
        value = jsValue.As<Napi::Number>().Int32Value();
        return true;
    }

    inline bool GetUint32(const Napi::Value& jsValue, uint32_t& value) {
        if (!jsValue.IsNumber()) {
            return false;
        }
        value = jsValue.As<Napi::Number>().Uint32Value();
        return true;
    }

    inline bool GetFloat(const Napi::Value& jsValue, float& value) {
        if (!jsValue.IsNumber()) {
            return false;
        }
        value = jsValue.As<Napi::Number>().FloatValue();
        return true;
    }

    inline bool GetBoolean(const Napi::Value& jsValue, bool& value) {
        if (!jsValue.IsBoolean()) {
            return false;
        }
        value = jsValue.As<Napi::Boolean>().Value();
        return true;
    }

    inline bool GetString(const Napi::Value& jsValue, std::string& value) {
        if (!jsValue.IsString()) {
            return false;
        }
        value = jsValue.As<Napi::String>().Utf8Value();
        return true;
    }

    inline bool GetOperand(const Napi::Value& jsValue, webnn::Operand& operand) {
        if (!jsValue.IsObject()) {
            return false;
        }
        Napi::Object jsObject = jsValue.As<Napi::Object>();
        if (!jsObject.InstanceOf(Operand::constructor.Value())) {
            return false;
        }
        operand = Napi::ObjectWrap<Operand>::Unwrap(jsObject)->GetImpl();
        return true;
    }

    inline bool GetOperandArray(const Napi::Value& jsValue, std::vector<webnn::Operand>& array) {
        if (!jsValue.IsArray()) {
            return false;
        }
        Napi::Array jsArray = jsValue.As<Napi::Array>();
        for (size_t j = 0; j < jsArray.Length(); j++) {
            if (!jsArray.Get(j).IsObject()) {
                return false;
            }
            Napi::Object object = jsArray.Get(j).As<Napi::Object>();
            if (!object.InstanceOf(Operand::constructor.Value())) {
                return false;
            }
            Operand* operand = Napi::ObjectWrap<Operand>::Unwrap(object);
            array.push_back(operand->GetImpl());
        }
        return true;
    }

    struct OperandDescriptor {
      public:
        webnn::OperandType type;
        std::vector<int32_t> dimensions;

        const webnn::OperandDescriptor* AsPtr() {
            if (!dimensions.empty()) {
                mDesc.dimensions = dimensions.data();
                mDesc.dimensionsCount = dimensions.size();
            }
            mDesc.type = type;
            return &mDesc;
        }

      private:
        webnn::OperandDescriptor mDesc;
    };

    inline bool GetOperandDescriptor(const Napi::Value& jsValue, OperandDescriptor& desc) {
        if (!jsValue.IsObject()) {
            return false;
        }
        Napi::Object jsDesc = jsValue.As<Napi::Object>();
        if (!jsDesc.Has("type")) {
            return false;
        }
        if (!GetOperandType(jsDesc.Get("type"), desc.type)) {
            return false;
        }
        if (jsDesc.Has("dimensions")) {
            if (!GetInt32Array(jsDesc.Get("dimensions"), desc.dimensions)) {
                return false;
            }
        }
        return true;
    }

    inline bool GetBufferView(const Napi::Value& jsValue,
                              const webnn::OperandType type,
                              const std::vector<int32_t>& dimensions,
                              void*& value,
                              size_t& size) {
        const std::unordered_map<webnn::OperandType, napi_typedarray_type> arrayTypeMap = {
            {webnn::OperandType::Float32, napi_float32_array},
            {webnn::OperandType::Int32, napi_int32_array},
            {webnn::OperandType::Uint32, napi_uint32_array},
        };
        if (!jsValue.IsTypedArray()) {
            return false;
        }
        Napi::TypedArray jsTypedArray = jsValue.As<Napi::TypedArray>();
        if (arrayTypeMap.find(type) == arrayTypeMap.end()) {
            return false;
        }
        if (arrayTypeMap.at(type) != jsTypedArray.TypedArrayType()) {
            return false;
        }
        value =
            reinterpret_cast<void*>(reinterpret_cast<int8_t*>(jsTypedArray.ArrayBuffer().Data()) +
                                    jsTypedArray.ByteOffset());
        size = jsTypedArray.ByteLength();
        size_t expectedSize;
        switch (type) {
            case webnn::OperandType::Float32:
                expectedSize = sizeof(float);
                break;
            case webnn::OperandType::Int32:
                expectedSize = sizeof(int32_t);
                break;
            case webnn::OperandType::Uint32:
                expectedSize = sizeof(uint32_t);
                break;
            default:
                return false;
        }
        for (auto dim : dimensions) {
            expectedSize *= dim;
        }
        if (expectedSize != size) {
            return false;
        }
        return true;
    }

    inline bool GetNamedOperands(const Napi::Value& jsValue,
                                 webnn::NamedOperands& namedOperands,
                                 std::vector<std::string>& names) {
        if (!jsValue.IsObject()) {
            return false;
        }
        Napi::Object jsOutputs = jsValue.As<Napi::Object>();
        namedOperands = webnn::CreateNamedOperands();
        Napi::Array outputNames = jsOutputs.GetPropertyNames();
        for (size_t j = 0; j < outputNames.Length(); ++j) {
            std::string name = outputNames.Get(j).As<Napi::String>().Utf8Value();
            Napi::Object output = jsOutputs.Get(name).As<Napi::Object>();
            if (!output.InstanceOf(Operand::constructor.Value())) {
                return false;
            }
            webnn::Operand operand = Napi::ObjectWrap<Operand>::Unwrap(output)->GetImpl();
            namedOperands.Set(name.data(), operand);
            names.push_back(name);
        }
        return true;
    }

    inline bool HasOptionMember(const Napi::Object& jsOptions, const std::string& name) {
        return jsOptions.Has(name) && !jsOptions.Get(name).IsUndefined();
    }

}  // namespace node

#endif  // NODE_UTILS_H_
