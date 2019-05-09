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

#include <value/type.h>
#include <value/value_types.h>
#include <rtti/rtti.h>
#include <rtti/enum.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <value/value_types.h>

Value_Type::Value_Type(ValueType n) : mValue(n) {}

ValueType Value_Type::value() const {
    return mValue;
}

std::string Value_Type::describe() const {
    return valueTypeToString(value());
}

bool Value_Type::equals(std::shared_ptr<Value> v) const {
    auto typ = runtime_ptr_cast<Value_Type>(v);
    if (typ == nullptr) return false;

    return typ->value() == value();
}

std::shared_ptr<Value> Value_Type::fromByteStream(ByteStream* bs) {
    if (auto val = valueTypeFromByteStream(bs)) {
        return Value::type(val.value());
    }

    return nullptr;
}

size_t Value_Type::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeNumber(enumToNumber(value()), 1);
    return wr;
}

#define VALUE_TYPE(NAME, TOKEN, STRING, NUMBER, CLASS) \
  id = p->nextIf(Token(TokenKind::IDENTIFIER, STRING)); \
  if (id != std::nullopt) { \
    auto ec = valueTypeFromString(id->value()); \
    if (ec) return Value::type(ec.value()); \
  }

std::shared_ptr<Value> Value_Type::fromParser(Parser* p) {
    std::optional<Token> id = std::nullopt;
#include <value/value_types.def>
    p->error("expected value type keyword");
    return nullptr;
}

size_t Value_Type::hash() const {
    return 0x31E0000 | enumToNumber(value());
}

std::shared_ptr<Value> Value_Type::clone() const {
    return Value::type(value());
}
