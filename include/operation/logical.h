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

#ifndef STUFF_OPERATION_LOGICAL
#define STUFF_OPERATION_LOGICAL

#include <operation/op.h>
#include <value/boolean.h>
#include <memory>
#include <operation/op_loader.h>

template<typename T, OperationType OpType, typename P = Operation>
class Binary_Logical_Operation : public DefaultConstructibleOperation<T,OpType,P> {
    public:
        virtual std::shared_ptr<Value> eval(Value_Boolean* v1, Value_Boolean* v2) = 0;
        Operation::Result doExecute(MachineState&) override;
};

template<typename T, OperationType OpType, typename P = Operation>
class Unary_Logical_Operation : public DefaultConstructibleOperation<T,OpType,P> {
    public:
        virtual std::shared_ptr<Value> eval(Value_Boolean* v1) = 0;
        Operation::Result doExecute(MachineState&) override;
};

#define BINARY_LOGICAL_OPERATION(ClassName, OpType) \
class ClassName : public Binary_Logical_Operation<\
    ClassName,\
    OpType> { \
    public: \
        std::shared_ptr<Value> eval(Value_Boolean* v1, Value_Boolean* v2) override; \
}

BINARY_LOGICAL_OPERATION(And, OperationType::AND);
BINARY_LOGICAL_OPERATION(Or, OperationType::OR);
BINARY_LOGICAL_OPERATION(Xor, OperationType::XOR);

#undef BINARY_LOGICAL_OPERATION

#define UNARY_LOGICAL_OPERATION(ClassName, OpType) \
class ClassName : public Unary_Logical_Operation<\
    ClassName,\
    OpType> { \
    public: \
        std::shared_ptr<Value> eval(Value_Boolean* v) override; \
}

UNARY_LOGICAL_OPERATION(Not, OperationType::NOT);

#undef UNARY_LOGICAL_OPERATION

#endif
