#ifndef __OPS_OPERAND_BASE_H_
#define __OPS_OPERAND_BASE_H_

#include <napi.h>
#include <string>
#include <vector>

#include <webnn/webnn.h>

namespace op {

    class OperandBase {
      public:
        OperandBase();
        explicit OperandBase(WebnnOperand operand);
        ~OperandBase();

        WebnnOperand GetOperand();

      protected:
        void SetOperand(WebnnOperand operand);

      private:
        WebnnOperand mOperand;
    };

}  // namespace op

#endif  // __OPS_OPERAND_BASE_H_
