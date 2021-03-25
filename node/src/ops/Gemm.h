#ifndef ___OPS_GEMM_H__
#define ___OPS_GEMM_H__

#include "OperandBase.h"

namespace op {

    class Gemm final : public OperandBase {
      public:
        Gemm(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder);
        ~Gemm() = default;
    };

}  // namespace op

#endif  // ___OPS_GEMM_H__
