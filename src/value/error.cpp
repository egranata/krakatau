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

#include <value/error.h>
#include <rtti/rtti.h>
#include <rtti/enum.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <error/error_codes.h>

Value_Error::Value_Error(ErrorCode n) : mValue(n) {}

ErrorCode Value_Error::value() const {
    return mValue;
}

std::string Value_Error::describe() const {
    return errorCodeToString(value());
}

bool Value_Error::equals(std::shared_ptr<Value> v) const {
    auto err = runtime_ptr_cast<Value_Error>(v);
    if (err == nullptr) return false;

    return err->value() == value();
}

std::shared_ptr<Value> Value_Error::fromByteStream(ByteStream* bs) {
    if (auto val = errorCodeFromByteStream(bs)) {
        return Value::error(val.value());
    }

    return nullptr;
}

size_t Value_Error::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeNumber(enumToNumber(value()), 1);
    return wr;
}

std::shared_ptr<Value> Value_Error::fromParser(Parser* p) {
    auto id = p->expectedError(TokenKind::IDENTIFIER);
    if (id == std::nullopt) return nullptr;
    auto ec = errorCodeFromString(id->value());
    if (ec == std::nullopt) {
        p->error("not a valid error code");
        return nullptr;
    }
    return Value::error(ec.value());
}

size_t Value_Error::hash() const {
    return 0xE690000 | enumToNumber(value());
}

std::shared_ptr<Value> Value_Error::clone() const {
    return Value::error(value());
}
