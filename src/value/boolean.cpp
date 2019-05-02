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

#include <value/boolean.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>

Value_Boolean::Value_Boolean(bool n) : mValue(n) {}

bool Value_Boolean::value() const {
    return mValue;
}

std::string Value_Boolean::describe() const {
    return value() ? "true" : "false";
}

bool Value_Boolean::equals(std::shared_ptr<Value> v) const {
    auto bln = runtime_ptr_cast<Value_Boolean>(v);
    if (bln == nullptr) return false;

    return bln->value() == value();
}

std::shared_ptr<Value> Value_Boolean::fromByteStream(ByteStream* bs) {
    if (auto val = bs->readNumber(1)) {
        return Value::fromBoolean(1 == val.value());
    }

    return nullptr;
}

size_t Value_Boolean::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeNumber(mValue ? 1 : 0, 1);
    return wr;
}

std::shared_ptr<Value> Value_Boolean::fromParser(Parser* p) {
    auto valstr = p->expectedError(TokenKind::BOOLEAN);
    if (!valstr) return nullptr;
    bool value = (valstr->value() == "true");
    return Value::fromBoolean(value);
}

size_t Value_Boolean::hash() const {
    return value() ? 0 : 1;
}
