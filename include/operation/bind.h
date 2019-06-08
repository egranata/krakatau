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

#ifndef STUFF_OPERATION_BIND
#define STUFF_OPERATION_BIND

#include <operation/base_op.h>
#include <memory>
#include <string>

class Value;
class MachineState;
class Parser;
class Serializer;
class ByteStream;

class PartialBind : public BaseOperation<PartialBind, OperationType::PARTIALBIND> {
    public:
        PartialBind(std::shared_ptr<Value>, std::shared_ptr<Operation>);

        static std::shared_ptr<PartialBind> fromByteStream(ByteStream* bs);
        static std::shared_ptr<PartialBind> fromParser(Parser*);

        Operation::Result doExecute(MachineState&) override;

        std::shared_ptr<Value> value() const;
        std::shared_ptr<Operation> callable() const;

        std::string describe() const override;
        bool equals(std::shared_ptr<Operation>) const override;
        size_t serialize(Serializer*) const override;
        std::shared_ptr<Operation> clone() const override;

    private:
        std::shared_ptr<Value> mValue;
        std::shared_ptr<Operation> mCallable;
};

#endif
