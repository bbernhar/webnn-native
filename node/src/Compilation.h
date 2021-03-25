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

#ifndef __Compilation_H__
#define __Compilation_H__

#include <napi.h>
#include <iostream>
#include <map>

#include <webnn/webnn.h>

template <typename T>
std::map<std::string, T> GetNamedOperands(Napi::Object obj) {
    std::map<std::string, T> namedOperands;
    Napi::Array propertyNames = obj.GetPropertyNames();
    for (size_t i = 0; i < propertyNames.Length(); ++i) {
        std::string name = propertyNames.Get(i).As<Napi::String>().Utf8Value();
        Napi::Object item = obj.Get(name).As<Napi::Object>();
        Napi::TypedArray array = item.Get("buffer").As<Napi::TypedArray>();
        Napi::ArrayBuffer buffer = array.ArrayBuffer();
        T operand;
        operand.buffer = reinterpret_cast<void*>(buffer.Data());
        operand.size = buffer.ByteLength();
        namedOperands[name] = operand;
    }
    return namedOperands;
}

class ComputeAsyncWorker;
class Compilation : public Napi::ObjectWrap<Compilation> {
  public:
    static Napi::Object Initialize(Napi::Env env, Napi::Object exports);
    static Napi::FunctionReference constructor;

    explicit Compilation(const Napi::CallbackInfo& info);
    ~Compilation();
    void SetCompilation(WebnnCompilation);
    WebnnCompilation GetCompilation();
    void SetOutputName(std::vector<std::string> outputName);

    Napi::Value Compute(const Napi::CallbackInfo& info);

  private:
    WebnnCompilation mCompilation;
    ComputeAsyncWorker* mComputeWorker;
    std::vector<std::string> mOutputName;
};

#endif  // __Compilation_H__
