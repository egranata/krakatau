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

#include <value/value.h>
#include <stdlib.h>
#include <function/block.h>
#include <value/number.h>
#include <value/boolean.h>
#include <value/empty.h>
#include <value/operation.h>
#include <value/block.h>
#include <value/string.h>
#include <value/tuple.h>
#include <value/table.h>
#include <operations/op.h>
#include <value/error.h>
#include <value/type.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <parser/parser.h>

Value::Value() = default;
Value::~Value() = default;

bool Value::isOfType(ValueType vt) const {
    return (vt == ValueType::NONE);
}

std::shared_ptr<Value> Value::empty() {
    return std::shared_ptr<Value>(new Value_Empty());
}

std::shared_ptr<Value> Value::fromNumber(uint64_t n) {
    return std::shared_ptr<Value>(new Value_Number(n));
}

std::shared_ptr<Value> Value::fromBoolean(bool b) {
    return std::shared_ptr<Value>(new Value_Boolean(b));
}

std::shared_ptr<Value> Value::fromBlock(std::shared_ptr<Block> b) {
    return std::shared_ptr<Value>(new Value_Block(b));
}

std::shared_ptr<Value> Value::fromOperation(std::shared_ptr<Operation> b) {
    return std::shared_ptr<Value>(new Value_Operation(b));
}

std::shared_ptr<Value> Value::fromString(const std::string& s) {
    return std::shared_ptr<Value>(new Value_String(s));
}

std::shared_ptr<Value> Value::error(ErrorCode ec) {
    return std::shared_ptr<Value>(new Value_Error(ec));
}

std::shared_ptr<Value> Value::tuple() {
    return std::shared_ptr<Value>(new Value_Tuple());
}

std::shared_ptr<Value> Value::type(ValueType vt) {
    return std::shared_ptr<Value>(new Value_Type(vt));
}

std::shared_ptr<Value> Value::table() {
    return std::shared_ptr<Value>(new Value_Table());
}

std::shared_ptr<Value> Value::fromByteStream(ByteStream* bs) {
    if (bs->nextIf(Value_Empty::MARKER)) return Value_Empty::fromByteStream(bs);
    if (bs->nextIf(Value_Number::MARKER)) return Value_Number::fromByteStream(bs);
    if (bs->nextIf(Value_Boolean::MARKER)) return Value_Boolean::fromByteStream(bs);
    if (bs->nextIf(Value_Tuple::MARKER)) return Value_Tuple::fromByteStream(bs);
    if (bs->nextIf(Value_Error::MARKER)) return Value_Error::fromByteStream(bs);
    if (bs->nextIf(Value_Type::MARKER)) return Value_Type::fromByteStream(bs);
    if (bs->nextIf(Value_Block::MARKER)) return Value_Block::fromByteStream(bs);
    if (bs->nextIf(Value_Operation::MARKER)) return Value_Operation::fromByteStream(bs);
    if (bs->nextIf(Value_String::MARKER)) return Value_String::fromByteStream(bs);
    if (bs->nextIf(Value_Table::MARKER)) return Value_Table::fromByteStream(bs);

    return nullptr;
}

std::shared_ptr<Value> Value::fromParser(Parser* p) {
    if (p->nextIf(TokenKind::KW_EMPTY)) return Value_Empty::fromParser(p);
    if (p->nextIf(TokenKind::KW_NUMBER)) return Value_Number::fromParser(p);
    if (p->nextIf(TokenKind::KW_BOOLEAN)) return Value_Boolean::fromParser(p);
    if (p->nextIf(TokenKind::KW_TUPLE)) return Value_Tuple::fromParser(p);
    if (p->nextIf(TokenKind::KW_BLOCK)) return Value_Block::fromParser(p);
    if (p->nextIf(TokenKind::KW_ERROR)) return Value_Error::fromParser(p);
    if (p->nextIf(TokenKind::KW_TYPE)) return Value_Type::fromParser(p);
    if (p->nextIf(TokenKind::KW_OPERATION)) return Value_Operation::fromParser(p);
    if (p->nextIf(TokenKind::KW_STRING)) return Value_String::fromParser(p);
    if (p->nextIf(TokenKind::KW_TABLE)) return Value_Table::fromParser(p);

    p->error("expected value type keyword");
    return nullptr;
}

std::shared_ptr<Value> Value::typecast(ValueType vt) {
    if (isOfType(vt)) return shared_from_this();
    if (vt == ValueType::STRING) return Value::fromString(describe());
    return doTypecast(vt);
}

std::shared_ptr<Value> Value::doTypecast(ValueType) {
    return nullptr;
}

std::ostream& operator<<(std::ostream& os, std::shared_ptr<Value> sp) {
    return (os << sp.get());
}
std::ostream& operator<<(std::ostream& os, Value* v) {
    return (os << v->describe());
}
