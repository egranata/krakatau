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

#include <operation/push.h>
#include <stream/indenting_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <rtti/rtti.h>
#include <machine/state.h>

Push::Push(std::shared_ptr<Value> v) : mValue(v) {}

Operation::Result Push::doExecute(MachineState& s) {
    s.stack().push(mValue);
    return Operation::Result::SUCCESS;
}

std::shared_ptr<Value> Push::value() const {
    return mValue;
}

std::string Push::describe() const {
    IndentingStream is;
    is.append("push %s", mValue->describe().c_str());
    return is.str();
}

std::shared_ptr<Operation> Push::fromByteStream(ByteStream* bs) {
    if (auto val = Value::fromByteStream(bs)) {
        return std::shared_ptr<Operation>(new Push(val));
    }

    return nullptr;
}

std::shared_ptr<Operation> Push::fromParser(Parser* p) {
    if (auto val = Value::fromParser(p)) {
        return std::shared_ptr<Operation>(new Push(val));
    }

    return nullptr;
}

size_t Push::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += value()->serialize(s);
    return wr;
}

bool Push::equals(std::shared_ptr<Operation> rhs) const {
    auto clr = rhs->asClass<Push>();
    if (clr) {
        return value()->equals(clr->value());
    }
    return false;
}

std::shared_ptr<Operation> Push::clone() const {
    return std::make_shared<Push>(value()->clone());
}

