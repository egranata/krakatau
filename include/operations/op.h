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

#ifndef STUFF_OPERATION_OP
#define STUFF_OPERATION_OP

#include <value/value.h>
#include <stack/stack.h>
#include <operations/op_types.h>
#include <memory>
#include <string>
#include <stdlib.h>

class Serializer;
class MachineState;

class Operation : public std::enable_shared_from_this<Operation> {
    public:
        enum class Result {
            HALT,
            ERROR,
            SUCCESS,
            RESTART_BLOCK,
            EXIT_BLOCK,
            AGAIN,
        };
        virtual Result execute(MachineState&) = 0;
        virtual std::string describe() const {
            return operationTypeToString(getClassId());
        }
        virtual ~Operation();

        virtual OperationType getClassId() const { return OperationType::NONE; }
        static OperationType getStaticClassId() { return OperationType::NONE; }
        virtual bool isOfType(OperationType) const;

        virtual size_t serialize(Serializer*) const;

        virtual bool equals(std::shared_ptr<Operation>) const;
        virtual std::shared_ptr<Operation> clone() const = 0;

        template <typename T>
        bool isOfClass() const { return isOfType( T::getStaticClassId() ); };

    protected:
        Operation();
        Operation(const Operation&) = delete;
        Operation& operator=(const Operation&) = delete;
};

template<typename T, OperationType OpType, typename Parent = Operation>
class BaseOperation : public Operation {
    public:
        virtual OperationType getClassId() const override { return OpType; }
        static OperationType getStaticClassId() { return OpType; }
        bool isOfType(OperationType aID) const override {
            return (aID == OpType) || this->Parent::isOfType(aID);
        }
};

template<typename T, OperationType OpType, typename Parent = Operation>
class DefaultConstructibleOperation : public BaseOperation<T,OpType,Parent> {
    private:
        std::shared_ptr<T> ensureDefaultConstructible() {
            return std::make_shared<T>();
        }
    public:
        std::shared_ptr<Operation> clone() const override {
            return std::make_shared<T>();
        }
        static std::shared_ptr<Operation> fromByteStream(ByteStream*) {
            return std::make_shared<T>();
        }
        static std::shared_ptr<Operation> fromParser(Parser*) {
            return std::make_shared<T>();
        }
};

std::string operationResultToString(Operation::Result);

#endif
