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

#include <value/character.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <stream/indenting_stream.h>

Value_Character::Value_Character(char32_t n) : mValue(n) {}

char32_t Value_Character::value() const {
    return mValue;
}

std::string Value_Character::describe() const {
    IndentingStream is;
    is.append("%#x", mValue);
    return is.str();
}

bool Value_Character::equals(std::shared_ptr<Value> v) const {
    auto bln = runtime_ptr_cast<Value_Character>(v);
    if (bln == nullptr) return false;

    return bln->value() == value();
}

std::shared_ptr<Value> Value_Character::fromByteStream(ByteStream* bs) {
    if (auto val = bs->readNumber(sizeof(char32_t))) {
        return Value::fromCharacter((char32_t)*val);
    }

    return nullptr;
}

size_t Value_Character::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeNumber(mValue, sizeof(char32_t));
    return wr;
}

std::shared_ptr<Value> Value_Character::fromParser(Parser* p) {
    char* ep;
    uint64_t value;
    if (auto valunicode = p->nextIf(TokenKind::UNICODE)) {
        value = strtoull(valunicode->value().c_str(), &ep, 16);
    } else if (auto valstr = p->nextIf(TokenKind::NUMBER)) {
        value = strtoull(valstr->value().c_str(), &ep, 0);
    } else {
        p->error("expected number or unicode");
        return nullptr;
    }
    if (ep && *ep != 0) {
        p->error("not a valid character");
        return nullptr;
    }
    if (value > UINT32_MAX) {
        p->error("not a valid character");
        return nullptr;
    }
    return Value::fromCharacter((char32_t)value);
}

size_t Value_Character::hash() const {
    return (size_t)value();
}

std::shared_ptr<Value> Value_Character::clone() const {
    return Value::fromCharacter(value());
}
