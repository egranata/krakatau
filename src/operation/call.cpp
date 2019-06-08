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

#include <operation/call.h>
#include <machine/state.h>
#include <stream/indenting_stream.h>
#include <rtti/rtti.h>
#include <value/value.h>
#include <parser/parser.h>
#include <value/operation.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>

Call::Call(std::string name, std::shared_ptr<Value_Tuple> args) : mName(name), mArguments(args) {}

std::string Call::describe() const {
    IndentingStream is;
    is.append("call %s %s", name().c_str(), arguments()->describe().c_str());
    return is.str();
}

std::string Call::name() const {
    return mName;
}
std::shared_ptr<Value_Tuple> Call::arguments() const {
    return mArguments;
}

bool Call::equals(std::shared_ptr<Operation> op) const {
    if (auto rhs = op->asClass<Call>()) {
        return rhs->name() == name() &&
               rhs->arguments()->equals(arguments());
    }

    return false;
}

std::shared_ptr<Operation> Call::clone() const {
    return std::make_shared<Call>(name(),
                                  std::dynamic_pointer_cast<Value_Tuple>(arguments()->clone()));
}

Operation::Result Call::doExecute(MachineState& ms) {
    auto op = ms.value_store().retrieve(name());
    if (op == nullptr) {
        ms.stack().push(Value::error(ErrorCode::NOT_FOUND));
        return Operation::Result::ERROR;
    }
    if (!op->isOfClass<Value_Operation>()) {
        ms.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }

    const size_t n = arguments()->size();

    for(size_t i = 0; i < n; ++i) {
        ms.stack().push(arguments()->at(n-i-1));
    }
    return op->asClass<Value_Operation>()->execute(ms);
}

std::shared_ptr<Operation> Call::fromParser(Parser* p) {
    auto ident = p->expectedError(TokenKind::IDENTIFIER);
    if (!ident.has_value()) return nullptr;
    auto tpl = std::dynamic_pointer_cast<Value_Tuple>(Value_Tuple::fromParser(p));
    if (tpl == nullptr) return nullptr;

    return std::make_shared<Call>(ident->value(), tpl);
}

size_t Call::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += s->writeIdentifier(name());
    wr += arguments()->serialize(s);
    return wr;
}

std::shared_ptr<Operation> Call::fromByteStream(ByteStream* bs) {
    auto ident = bs->readIdentifier();
    if (!ident) return nullptr;

    auto val = Value::fromByteStream(bs);
    if (val == nullptr) return nullptr;
    if (val->isOfClass<Value_Tuple>() == false) return nullptr;
    std::shared_ptr<Value_Tuple> tpl = std::dynamic_pointer_cast<Value_Tuple>(val);

    return std::make_shared<Call>(*ident, tpl);
}
