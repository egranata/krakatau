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

#include <operation/storeslot.h>
#include <value/value_store.h>
#include <value/value.h>
#include <stream/indenting_stream.h>
#include <error/error_codes.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <rtti/rtti.h>
#include <machine/state.h>
#include <value/string.h>
#include <value/table.h>

Storeslot::Storeslot(const std::string& k) {
    mKey = k;
}
Operation::Result Storeslot::doExecute(MachineState& ms) {
    auto ptr = ms.stack().peek();
    auto vkey = Value::fromString(key());
    if (ms.currentSlot()->add(vkey, ptr)) {
        return Operation::Result::SUCCESS;
    } else {
        ms.stack().push(Value::error(ErrorCode::ALREADY_EXISTING));
        return Operation::Result::ERROR;
    }
}
std::string Storeslot::describe() const {
    IndentingStream is;
    is.append("storeslot \"%s\"", mKey.c_str());
    return is.str();
}

std::shared_ptr<Operation> Storeslot::fromByteStream(ByteStream* bs) {
    if (auto ident = bs->readIdentifier()) {
        return std::shared_ptr<Operation>(new Storeslot(ident.value()));
    }

    return nullptr;
}

std::shared_ptr<Operation> Storeslot::fromParser(Parser* p) {
    if (auto ident = p->nextIf(TokenKind::IDENTIFIER)) {
        return std::shared_ptr<Operation>(new Storeslot(ident->value()));
    }

    return nullptr;
}

std::string Storeslot::key() const {
    return mKey;
}

size_t Storeslot::serialize(Serializer* s) const {
    size_t wr = this->Operation::serialize(s);
    wr += s->writeIdentifier(key());
    return wr;
}

bool Storeslot::equals(std::shared_ptr<Operation> rhs) const {
    auto clr = runtime_ptr_cast<Storeslot>(rhs);
    if (clr) {
        return key() == clr->key();
    }
    return false;
}

std::shared_ptr<Operation> Storeslot::clone() const {
    return std::make_shared<Storeslot>(key());
}
