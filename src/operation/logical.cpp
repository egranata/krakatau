// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <operation/logical.h>
#include <value/error.h>
#include <error/error_codes.h>
#include <value/boolean.h>
#include <rtti/rtti.h>
#include <machine/state.h>

template<class T, OperationType OpType>
Operation::Result Binary_Logical_Operation<T,OpType>::doExecute(MachineState& s) {
    auto p1 = s.stack().pop();
    auto p2 = s.stack().pop();

    if (p1->isOfClass<Value_Boolean>() && p2->isOfClass<Value_Boolean>()) {
        auto n1 = runtime_ptr_cast<Value_Boolean>(p1);
        auto n2 = runtime_ptr_cast<Value_Boolean>(p2);
        std::shared_ptr<Value> ptr = eval(n1, n2);
        if (ptr->isOfClass<Value_Error>()) {
            s.stack().push(p2);
            s.stack().push(p1);
        }
        s.stack().push(ptr);
        return Operation::Result::SUCCESS;
    } else {
        s.stack().push(p2);
        s.stack().push(p1);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }
}

template<class T, OperationType OpType>
Operation::Result Unary_Logical_Operation<T,OpType>::doExecute(MachineState& s) {
    auto p = s.stack().pop();

    if (p->isOfClass<Value_Boolean>()) {
        auto n = runtime_ptr_cast<Value_Boolean>(p);
        std::shared_ptr<Value> ptr = eval(n);
        if (ptr->isOfClass<Value_Error>()) {
            s.stack().push(p);
        }
        s.stack().push(ptr);
        return Operation::Result::SUCCESS;
    } else {
        s.stack().push(p);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }
}

std::shared_ptr<Value> And::eval(Value_Boolean* v1, Value_Boolean* v2) {
    auto n1 = v1->value();
    auto n2 = v2->value();
    return Value::fromBoolean(n1 && n2);
}

std::shared_ptr<Value> Or::eval(Value_Boolean* v1, Value_Boolean* v2) {
    auto n1 = v1->value();
    auto n2 = v2->value();
    return Value::fromBoolean(n1 || n2);
}

std::shared_ptr<Value> Xor::eval(Value_Boolean* v1, Value_Boolean* v2) {
    auto n1 = v1->value();
    auto n2 = v2->value();
    return Value::fromBoolean((n1 || n2) && !(n1 && n2));
}

std::shared_ptr<Value> Not::eval(Value_Boolean* v) {
    auto n = v->value();
    return Value::fromBoolean(!n);
}
