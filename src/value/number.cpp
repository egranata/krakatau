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

#include <value/number.h>
#include <stream/indenting_stream.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>

Value_Number::Value_Number(uint64_t n) : mValue(n) {}

uint64_t Value_Number::value() const {
    return mValue;
}

std::string Value_Number::describe() const {
    IndentingStream is;
    is.append("%llu", value());
    return is.str();
}

bool Value_Number::equals(std::shared_ptr<Value> v) const {
    auto num = runtime_ptr_cast<Value_Number>(v);
    if (num == nullptr) return false;

    return num->value() == value();
}

std::shared_ptr<Value> Value_Number::fromByteStream(ByteStream* bs) {
    if (auto val = bs->readNumber()) {
        return Value::fromNumber(val.value());
    }

    return nullptr;
}

size_t Value_Number::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeNumber(mValue);
    return wr;
}

std::shared_ptr<Value> Value_Number::fromParser(Parser* p) {
    auto valstr = p->expectedError(TokenKind::NUMBER);
    if (!valstr) return nullptr;
    char *ep;
    uint64_t value = strtoull(valstr->value().c_str(), &ep, 0);
    if (ep && *ep != 0) {
        p->error("not a valid number");
        return nullptr;
    }
    return Value::fromNumber(value);
}

size_t Value_Number::hash() const {
    return (size_t)value();
}

std::shared_ptr<Value> Value_Number::clone() const {
    return Value::fromNumber(value());
}
