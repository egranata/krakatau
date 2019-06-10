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

#include <value/atom.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <stream/indenting_stream.h>

Value_Atom::Value_Atom(const std::string& n) : mValue(n) {}

std::string Value_Atom::value() const {
    return mValue;
}

std::string Value_Atom::describe() const {
    IndentingStream is;
    is.append("atom(%s)", value().c_str());
    return is.str();
}

bool Value_Atom::equals(std::shared_ptr<Value> v) const {
    auto bln = runtime_ptr_cast<Value_Atom>(v);
    if (bln == nullptr) return false;

    return bln->value() == value();
}

std::shared_ptr<Value> Value_Atom::fromByteStream(ByteStream* bs) {
    if (auto id = bs->readIdentifier()) {
        return Value::atom(*id);
    }

    return nullptr;
}

size_t Value_Atom::serialize(Serializer* s) {
    size_t wr = s->writeNumber(MARKER, 1);
    wr += s->writeIdentifier(value());
    return wr;
}

std::shared_ptr<Value> Value_Atom::fromParser(Parser* p) {
    auto valstr = p->expectedError(TokenKind::IDENTIFIER);
    if (!valstr) return nullptr;
    return Value::atom(valstr->value());
}

size_t Value_Atom::hash() const {
    return std::hash<std::string>()(value()) | 0xA30550A3;
}

std::shared_ptr<Value> Value_Atom::clone() const {
    return Value::atom(value());
}
