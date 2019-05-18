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

#include <operation/bind.h>
#include <stream/indenting_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <machine/state.h>
#include <operation/op_loader.h>
#include <value/operation.h>

PartialBind::PartialBind(std::shared_ptr<Value> v, std::shared_ptr<Operation> c) : mValue(v), mCallable(c) {}

Operation::Result PartialBind::execute(MachineState& ms) {
    ms.stack().push(value());
    return callable()->execute(ms);
}

std::shared_ptr<Value> PartialBind::value() const {
    return mValue;
}
std::shared_ptr<Operation> PartialBind::callable() const {
    return mCallable;
}

std::string PartialBind::describe() const {
    IndentingStream is;
    is.append("bind(%s, %s)", value()->describe().c_str(), callable()->describe().c_str());
    return is.str();
}

bool PartialBind::equals(std::shared_ptr<Operation> op) const {
    if (auto rhs = runtime_ptr_cast<PartialBind>(op)) {
        bool v = value()->equals(rhs->value());
        bool c = callable()->equals(rhs->callable());
        return v && c;
    } else return false;
}

std::shared_ptr<PartialBind> PartialBind::fromByteStream(ByteStream* bs) {
    auto val = Value::fromByteStream(bs);
    auto cal = OperationLoader::loader()->fromByteStream(bs);
    return std::make_shared<PartialBind>(val, cal);
}

std::shared_ptr<PartialBind> PartialBind::fromParser(Parser* p) {
    auto val = Value::fromParser(p);
    if (val == nullptr) return nullptr;
    auto vcl = Value::fromParser(p);
    if (vcl == nullptr) return nullptr;
    if (vcl->asClass<Value_Operation>() == nullptr) return nullptr;
    else return std::make_shared<PartialBind>(val, vcl->asClass<Value_Operation>()->value());
}

size_t PartialBind::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += value()->serialize(s);
    wr += callable()->serialize(s);
    return wr;
}

std::shared_ptr<Operation> PartialBind::clone() const {
    auto val = value()->clone();
    auto cal = callable()->clone();
    return std::make_shared<PartialBind>(val, cal);
}
