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

#ifndef STUFF_FUNCTION_BLOCK
#define STUFF_FUNCTION_BLOCK

#include <memory>
#include <operations/op.h>
#include <machine/state.h>
#include <vector>
#include <string>
#include <optional>
#include <value/value_table.h>

class Serializer;
class ByteStream;
class Parser;

class Block : public Operation {
    public:
        static std::shared_ptr<Block> fromByteStream(ByteStream*);
        static std::shared_ptr<Block> fromParser(Parser*);

        Operation::Result execute(MachineState&) override;
        void add(std::shared_ptr<Operation>);
        size_t size() const;
        std::shared_ptr<Operation> at(size_t) const;
        std::string describe() const override;
        size_t serialize(Serializer*) const override;
        bool equals(std::shared_ptr<Operation>) const override;

        std::shared_ptr<ValueTable> newSlot();
        void dropSlot();
        std::shared_ptr<ValueTable> currentSlot() const;

        void addSlotValue(std::string);
        size_t numSlotValues() const;
        std::optional<std::string> slotValueAt(size_t) const;

    private:
        std::vector<std::shared_ptr<Operation>> mOperations;
        std::vector<std::shared_ptr<ValueTable>> mSlots;
        std::vector<std::string> mSlotNames;
    public:
        decltype(mOperations)::const_iterator begin() const;
        decltype(mOperations)::const_iterator end() const;

        ABSTRACT_OPERATION_SUBCLASS(Block, OperationType::BLOCK, Operation);
};

#endif
