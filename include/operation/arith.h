/*
 * Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STUFF_OPERATION_ARITH
#define STUFF_OPERATION_ARITH

#include <operation/op.h>
#include <value/number.h>
#include <memory>
#include <operation/op_loader.h>

template<typename T, OperationType OpType, typename P = Operation>
class Binary_Arithmetic_Operation : public DefaultConstructibleOperation<T,OpType,P> {
    public:
        virtual std::shared_ptr<Value> eval(Value_Number* v1, Value_Number* v2) = 0;
        Operation::Result execute(MachineState&) override;
};

template<typename T, OperationType OpType, typename P = Operation>
class Unary_Arithmetic_Operation : public DefaultConstructibleOperation<T,OpType,P> {
    public:
        virtual std::shared_ptr<Value> eval(Value_Number* v1) = 0;
        Operation::Result execute(MachineState&) override;
};

#define BINARY_ARITH_OPERATION(ClassName, OpType) \
class ClassName : public Binary_Arithmetic_Operation< \
    ClassName,\
    OpType> { \
    public: \
        std::shared_ptr<Value> eval(Value_Number* v1, Value_Number* v2) override; \
}; \

BINARY_ARITH_OPERATION(Add, OperationType::ADD);
BINARY_ARITH_OPERATION(Subtract, OperationType::SUBTRACT);
BINARY_ARITH_OPERATION(Multiply, OperationType::MULTIPLY);
BINARY_ARITH_OPERATION(Divide, OperationType::DIVIDE);
BINARY_ARITH_OPERATION(Modulo, OperationType::MODULO);

#undef BINARY_ARITH_OPERATION

#define UNARY_ARITH_OPERATION(ClassName, OpType) \
class ClassName : public Unary_Arithmetic_Operation<\
    ClassName,\
    OpType> { \
    public: \
        std::shared_ptr<Value> eval(Value_Number* v) override; \
}; \

UNARY_ARITH_OPERATION(Positive, OperationType::POSITIVE);
UNARY_ARITH_OPERATION(Zero, OperationType::ZERO);
UNARY_ARITH_OPERATION(Negative, OperationType::NEGATIVE);

#undef UNARY_ARITH_OPERATION

#endif
