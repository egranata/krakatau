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

#include <value/operation.h>
#include <operations/op.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <operations/op_loader.h>
#include <function/bind.h>
#include <function/block.h>

Value_Operation::Value_Operation(std::shared_ptr<Operation> b) : mValue(b) {}

std::shared_ptr<Operation> Value_Operation::value() const {
    return mValue;
}

std::shared_ptr<PartialBind> Value_Operation::bind() const {
    return std::dynamic_pointer_cast<PartialBind>(value());
}
std::shared_ptr<Block> Value_Operation::block() const {
    return std::dynamic_pointer_cast<Block>(value());
}

std::string Value_Operation::describe() const {
    return mValue->describe();
}

bool Value_Operation::equals(std::shared_ptr<Value> v) const {
    auto blk = runtime_ptr_cast<Value_Operation>(v);
    if (blk == nullptr) return false;

    return value()->equals(blk->value());
}

std::shared_ptr<Value> Value_Operation::fromByteStream(ByteStream* bs) {
    if (auto op = OperationLoader::loader()->fromByteStream(bs)) {
        return std::shared_ptr<Value>(new Value_Operation(op));
    }

    return nullptr;
}

size_t Value_Operation::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += value()->serialize(s);
    return wr;
}

std::shared_ptr<Value> Value_Operation::fromParser(Parser* p) {
    if (auto op = OperationLoader::loader()->fromParser(p)) {
        return std::shared_ptr<Value>(new Value_Operation(op));
    } else return nullptr;
}

size_t Value_Operation::hash() const {
    return 0x0E0000 | enumToNumber(value()->getClassId());
}

Operation::Result Value_Operation::execute(MachineState& ms) {
    return value()->execute(ms);
}

std::shared_ptr<Value> Value_Operation::clone() const {
    return Value::fromOperation(value()->clone());
}
