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

#ifndef __ML_H__
#define __ML_H__

#include <napi.h>

class ML : public Napi::ObjectWrap<ML> {
  public:
    static Napi::Object Initialize(Napi::Env env, Napi::Object exports);
    static Napi::FunctionReference constructor;

    ML(const Napi::CallbackInfo& info);
    ~ML();

  private:
    static Napi::Value GetNeuralNetworkContext(const Napi::CallbackInfo& info);
};

#endif  // __ML_H__
