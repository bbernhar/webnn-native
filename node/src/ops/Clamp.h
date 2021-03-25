#ifndef ___OPS_CLAMP_H__
#define ___OPS_CLAMP_H__

#include "ops/OperandBase.h"
namespace op {

    class Clamp final : public OperandBase {
      public:
        Clamp(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder);
        ~Clamp() = default;
    };

}  // namespace op

#endif  // ___OPS_CONCAT_H__
