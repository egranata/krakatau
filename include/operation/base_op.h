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

#ifndef STUFF_OPERATION_BASEOP
#define STUFF_OPERATION_BASEOP

#include <stack/stack.h>
#include <operation/base_op.h>
#include <value/error.h>
#include <machine/state.h>
#include <operation/preconditions.h>

template<typename T, OperationType OpType, typename Preconditions = PreconditionAllowAll, typename Parent = Operation>
class BaseOperation : public Operation {
    public:
        virtual OperationType getClassId() const override { return OpType; }
        static OperationType getStaticClassId() { return OpType; }
        bool isOfType(OperationType aID) const override {
            return (aID == OpType) || this->Parent::isOfType(aID);
        }
        Result execute(MachineState& ms) override  {
            if (auto ec = mPreconditions(ms)) {
                ms.stack().push(Value::error(*ec));
                return Operation::Result::ERROR;
            }
            return doExecute(ms);
        }

    protected:
        Preconditions mPreconditions;
        virtual Result doExecute(MachineState&) = 0;
};

template<typename T, OperationType OpType, typename Preconditions = PreconditionAllowAll, typename Parent = Operation>
class DefaultConstructibleOperation : public BaseOperation<T,OpType,Preconditions,Parent> {
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

std::ostream& operator<<(std::ostream&, std::shared_ptr<Operation>);
std::ostream& operator<<(std::ostream&, Operation*);

#endif
