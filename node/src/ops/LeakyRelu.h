#ifndef ___OPS_LEAKYRELU_H__
#define ___OPS_LEAKYRELU_H__

#include "ops/OperandBase.h"
namespace op {

    class LeakyRelu final : public OperandBase {
      public:
        LeakyRelu(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder);
        ~LeakyRelu() = default;
    };

}  // namespace op

#endif  // ___OPS_LEAKYRELU_H__
