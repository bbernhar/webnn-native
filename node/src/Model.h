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

#ifndef __Model_H__
#define __Model_H__

#include <napi.h>

#include <webnn/webnn.h>

class Model : public Napi::ObjectWrap<Model> {
  public:
    static Napi::Object Initialize(Napi::Env env, Napi::Object exports);
    static Napi::FunctionReference constructor;

    explicit Model(const Napi::CallbackInfo& info);
    ~Model();
    void SetModel(WebnnModel);
    WebnnModel GetModel();

    Napi::Value Compile(const Napi::CallbackInfo& info);

  private:
    WebnnModel mModel;
    WebnnCompilation mCompilation;
    std::vector<std::string> mOutputName;
};

#endif  // __Model_H__
