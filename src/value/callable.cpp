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

#include <value/callable.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <operations/op_loader.h>
#include <operations/op.h>
#include <rtti/enum.h>
#include <function/bind.h>
#include <value/operation.h>
#include <value/block.h>
#include <function/block.h>
#include <function/callable.h>

Value_Callable::Value_Callable(Callable c) : mValue(c) {}

Callable Value_Callable::value() const {
    return mValue;
}

std::string Value_Callable::describe() const {
    return value().describe();
}

bool Value_Callable::equals(std::shared_ptr<Value> v) const {
    auto blk = runtime_ptr_cast<Value_Callable>(v);
    if (blk == nullptr) return false;

    return value().equals(blk->value());
}

std::shared_ptr<Value> Value_Callable::fromByteStream(ByteStream* bs) {
    if (auto blk = Callable::fromByteStream(bs)) {
        return std::shared_ptr<Value>(new Value_Callable(blk));
    }

    return nullptr;
}

size_t Value_Callable::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += value().serialize(s);
    return wr;
}

std::shared_ptr<Value> Value_Callable::fromParser(Parser* p) {
    if (auto blk = Callable::fromParser(p)) {
        return std::shared_ptr<Value>(new Value_Callable(blk));
    }

    return nullptr;
}

size_t Value_Callable::hash() const {
    size_t hash = 0xCABE000;
    hash |= value().hash();
    return hash;
}

std::shared_ptr<Value> Value_Callable::clone() const {
    auto newCallable = value().clone();
    return Value::fromCallable(newCallable);
}

Operation::Result Value_Callable::execute(MachineState& ms) {
    return value().execute(ms);
}
