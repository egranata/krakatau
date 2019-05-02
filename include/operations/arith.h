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

#include <operations/op.h>
#include <value/number.h>
#include <memory>
#include <operations/op_loader.h>

class Binary_Arithmetic_Operation : public Operation {
    public:
        virtual std::shared_ptr<Value> eval(Value_Number* v1, Value_Number* v2) = 0;
        Operation::Result execute(MachineState&) override;

        ABSTRACT_OPERATION_SUBCLASS(Binary_Arithmetic_Operation, OperationType::BINARY_ARITHMETIC, Operation);
};

class Unary_Arithmetic_Operation : public Operation {
    public:
        virtual std::shared_ptr<Value> eval(Value_Number* v1) = 0;
        Operation::Result execute(MachineState&) override;

        ABSTRACT_OPERATION_SUBCLASS(Unary_Arithmetic_Operation, OperationType::UNARY_ARITHMETIC, Operation);
};

#define BINARY_ARITH_OPERATION(ClassName, OpType) \
class ClassName ## _Binary_Arithmetic_Operation : public Binary_Arithmetic_Operation { \
    public: \
        std::shared_ptr<Value> eval(Value_Number* v1, Value_Number* v2) override; \
        OPERATION_SUBCLASS(ClassName ## _Binary_Arithmetic_Operation, OpType, Binary_Arithmetic_Operation); \
}; \

BINARY_ARITH_OPERATION(Add, OperationType::ADD);
BINARY_ARITH_OPERATION(Subtract, OperationType::SUBTRACT);
BINARY_ARITH_OPERATION(Multiply, OperationType::MULTIPLY);
BINARY_ARITH_OPERATION(Divide, OperationType::DIVIDE);
BINARY_ARITH_OPERATION(Modulo, OperationType::MODULO);

#undef BINARY_ARITH_OPERATION

#define UNARY_ARITH_OPERATION(ClassName, OpType) \
class ClassName ## _Unary_Arithmetic_Operation : public Unary_Arithmetic_Operation { \
    public: \
        std::shared_ptr<Value> eval(Value_Number* v) override; \
        OPERATION_SUBCLASS(ClassName ## _Unary_Arithmetic_Operation, OpType, Unary_Arithmetic_Operation); \
}; \

UNARY_ARITH_OPERATION(Positive, OperationType::POSITIVE);
UNARY_ARITH_OPERATION(Zero, OperationType::ZERO);
UNARY_ARITH_OPERATION(Negative, OperationType::NEGATIVE);

#undef UNARY_ARITH_OPERATION

#endif
