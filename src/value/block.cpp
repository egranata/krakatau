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

#include <value/block.h>
#include <function/block.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <operations/op_loader.h>
#include <operations/op.h>
#include <rtti/enum.h>
#include <value/operation.h>

Value_Block::Value_Block(std::shared_ptr<Block> b) : mValue(b) {}

std::shared_ptr<Block> Value_Block::value() const {
    return mValue;
}

std::string Value_Block::describe() const {
    return mValue->describe();
}

bool Value_Block::equals(std::shared_ptr<Value> v) const {
    auto blk = runtime_ptr_cast<Value_Block>(v);
    if (blk == nullptr) return false;

    return value()->equals(blk->value());
}

std::shared_ptr<Value> Value_Block::fromByteStream(ByteStream* bs) {
    if (auto blk = Block::fromByteStream(bs)) {
        return std::shared_ptr<Value>(new Value_Operation(blk));
    }

    return nullptr;
}

size_t Value_Block::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += value()->serialize(s);
    return wr;
}

std::shared_ptr<Value> Value_Block::fromParser(Parser* p) {
    if (auto blk = Block::fromParser(p)) {
        return std::shared_ptr<Value>(new Value_Operation(blk));
    }

    return nullptr;
}

size_t Value_Block::hash() const {
    size_t hash = 0xBEC0000;
    auto blk = value();
    for (size_t i = 0; i < blk->size(); ++i) {
        hash += enumToNumber(blk->at(i)->getClassId());
    }
    return hash;
}

std::shared_ptr<Value> Value_Block::clone() const {
    auto newBlockOp = value()->clone();
    return Value::fromOperation(newBlockOp);
}

Operation::Result Value_Block::execute(MachineState& ms) {
    return value()->execute(ms);
}
