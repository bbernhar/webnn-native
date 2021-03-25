#ifndef ___OPS_BATCHNORM_H__
#define ___OPS_BATCHNORM_H__

#include "ops/OperandBase.h"
namespace op {

    class BatchNorm final : public OperandBase {
      public:
        BatchNorm(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder);
        ~BatchNorm() = default;
    };

}  // namespace op

#endif  // ___OPS_BATCHNORM_H__
