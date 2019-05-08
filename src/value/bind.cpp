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

#include <value/bind.h>
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

Value_Bind::Value_Bind(std::shared_ptr<PartialBind> b) : mValue(b) {}

std::shared_ptr<PartialBind> Value_Bind::value() const {
    return mValue;
}

std::string Value_Bind::describe() const {
    return value()->describe();
}

bool Value_Bind::equals(std::shared_ptr<Value> v) const {
    auto blk = runtime_ptr_cast<Value_Bind>(v);
    if (blk == nullptr) return false;

    return value()->equals(blk->value());
}

std::shared_ptr<Value> Value_Bind::fromByteStream(ByteStream* bs) {
    if (auto blk = PartialBind::fromByteStream(bs)) {
        return std::shared_ptr<Value>(new Value_Bind(blk));
    }

    return nullptr;
}

size_t Value_Bind::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += value()->serialize(s);
    return wr;
}

std::shared_ptr<Value> Value_Bind::fromParser(Parser* p) {
    auto val = Value::fromParser(p);
    if (val == nullptr) return nullptr;
    auto vcl = Value::fromParser(p);
    if (vcl == nullptr) return nullptr;

    Callable clb(vcl);
    if (clb) return Value::fromBind(std::make_shared<PartialBind>(val, clb));
    else return nullptr;
}

size_t Value_Bind::hash() const {
    size_t hash = 0xE1DD000;
    auto bind = value();
    hash |= value()->value()->hash();
    hash += value()->callable().hash();
    return hash;
}

std::shared_ptr<Value> Value_Bind::clone() const {
    auto newBind = value()->clone();
    return Value::fromBind(newBind);
}

Operation::Result Value_Bind::execute(MachineState& ms) {
    return value()->execute(ms);
}
