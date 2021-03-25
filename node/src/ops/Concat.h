#ifndef ___OPS_CONCAT_H__
#define ___OPS_CONCAT_H__

#include "ops/OperandBase.h"
namespace op {

    class Concat final : public OperandBase {
      public:
        Concat(const Napi::CallbackInfo& info, WebnnModelBuilder modelBuilder);
        ~Concat() = default;
    };

}  // namespace op

#endif  // ___OPS_CONCAT_H__
