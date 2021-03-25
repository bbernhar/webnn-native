#include "OperandBase.h"

#include <iostream>
#include "Operand.h"

namespace op {

    OperandBase::OperandBase() : mOperand(nullptr) {
    }
    OperandBase::OperandBase(WebnnOperand operand) : mOperand(operand) {
    }

    void OperandBase::SetOperand(WebnnOperand operand) {
        mOperand = operand;
    }

    WebnnOperand OperandBase::GetOperand() {
        return mOperand;
    }

    OperandBase::~OperandBase() {
        if (mOperand) {
            webnnOperandRelease(mOperand);
        }
    }

}  // namespace op
