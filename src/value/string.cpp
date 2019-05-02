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

#include <value/string.h>
#include <rtti/rtti.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <parser/parser.h>
#include <string>
#include <functional>

Value_String::Value_String(const std::string& s) : mValue(s) {}

std::string Value_String::value() const {
    return mValue;
}

std::string Value_String::describe() const {
    return value();
}

bool Value_String::equals(std::shared_ptr<Value> v) const {
    auto str = runtime_ptr_cast<Value_String>(v);
    if (str == nullptr) return false;

    return str->value() == value();
}

size_t Value_String::serialize(Serializer* s) {
    const auto& val(value());
    const auto sz = val.size();

    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeData(sz, (uint8_t*)val.data());
    return wr;
}

std::shared_ptr<Value> Value_String::fromByteStream(ByteStream* bs) {
    if (auto sv = bs->readData()) {
        return Value::fromString(sv.value());
    }
    
    return nullptr;
}

std::shared_ptr<Value> Value_String::fromParser(Parser* p) {
    auto valstr = p->expectedError(TokenKind::STRING);
    if (!valstr) return nullptr;
    auto value = valstr->value();
    return Value::fromString(value);
}

size_t Value_String::hash() const {
    return std::hash<std::string>()(value());
}
