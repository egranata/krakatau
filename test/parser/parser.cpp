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

#include <gtest/gtest.h>
#include <parser/parser.h>
#include <value/value.h>
#include <value/empty.h>
#include <value/number.h>
#include <value/boolean.h>
#include <value/tuple.h>
#include <rtti/rtti.h>
#include <value/block.h>
#include <function/block.h>
#include <value/error.h>
#include <operations/arith.h>
#include <operations/dup.h>
#include <operations/load.h>
#include <operations/store.h>
#include <operations/push.h>
#include <value/operation.h>
#include <operations/resetstack.h>
#include <value/type.h>
#include <value/string.h>
#include <value/table.h>
#include <stream/shared_file.h>
#include <value/bind.h>
#include <function/bind.h>

TEST(Parser, Next) {
    Parser p("hello world 1234");
    auto tok = p.next();
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == Token(TokenKind::IDENTIFIER, "hello"));
    tok = p.next();
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == Token(TokenKind::IDENTIFIER, "world"));
    tok = p.next();
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == Token(TokenKind::NUMBER, "1234"));
    tok = p.next();
    ASSERT_FALSE(tok.has_value());
}

TEST(Parser, IgnoreSpacing) {
    Parser p("hello\n     \t\t\n   world       \n\n\t   1234");
    auto tok = p.next();
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == Token(TokenKind::IDENTIFIER, "hello"));
    tok = p.next();
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == Token(TokenKind::IDENTIFIER, "world"));
    tok = p.next();
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == Token(TokenKind::NUMBER, "1234"));
    tok = p.next();
    ASSERT_FALSE(tok.has_value());
}

TEST(Parser, NextIfKind) {
    Parser p("hello empty 1234");
    auto tok = p.nextIf(TokenKind::IDENTIFIER);
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == Token(TokenKind::IDENTIFIER, "hello"));
    tok = p.nextIf(TokenKind::BOOLEAN);
    ASSERT_FALSE(tok.has_value());
    tok = p.nextIf(TokenKind::IDENTIFIER);
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == Token(TokenKind::IDENTIFIER, "empty"));
    tok = p.nextIf(TokenKind::IDENTIFIER);
    ASSERT_FALSE(tok.has_value());
    tok = p.nextIf(TokenKind::CLOSE_BLOCK);
    ASSERT_FALSE(tok.has_value());
    tok = p.nextIf(TokenKind::NUMBER);
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == Token(TokenKind::NUMBER, "1234"));
}

TEST(Parser, NextIfValue) {
    Parser p("hello empty 1234");
    auto wanted = Token(TokenKind::IDENTIFIER, "hello");
    auto tok = p.nextIf(wanted);
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == wanted);
    wanted = Token(TokenKind::IDENTIFIER, "notempty");
    tok = p.nextIf(wanted);
    ASSERT_FALSE(tok.has_value());
    wanted = Token(TokenKind::IDENTIFIER, "empty");
    tok = p.nextIf(wanted);
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == wanted);
    wanted = Token(TokenKind::NUMBER, "4321");
    tok = p.nextIf(wanted);
    ASSERT_FALSE(tok.has_value());
    wanted = Token(TokenKind::NUMBER, "1234");
    tok = p.nextIf(wanted);
    ASSERT_TRUE(tok.has_value());
    ASSERT_TRUE(tok.value() == wanted);
}

TEST(Parser, NextIfNullptr) {
    Parser p("hello 1234");
    ASSERT_FALSE(p.nextIf(nullptr));
    ASSERT_NE(std::nullopt, p.next());
    ASSERT_FALSE(p.nextIf(nullptr));
    ASSERT_NE(std::nullopt, p.next());
    ASSERT_TRUE(p.nextIf(nullptr));
    ASSERT_EQ(std::nullopt, p.next());
}

TEST(Parser, ParseIllegalValue) {
    Parser p("value foobar illegal");
    auto value = p.parseValue();
    ASSERT_EQ(nullptr, value);
}

TEST(Parser, ParseEmpty) {
    Parser p("value foobar empty");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_EQ(value->name, "foobar");
    ASSERT_TRUE(value->value->isOfClass<Value_Empty>());
}

TEST(Parser, ParseNumber) {
    Parser p("value foobar number 123456");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_EQ(value->name, "foobar");
    ASSERT_TRUE(value->value->isOfClass<Value_Number>());
    ASSERT_EQ(123456, runtime_ptr_cast<Value_Number>(value->value)->value());
}

TEST(Parser, ParseIllegalNumber) {
    Parser p("value foobar number foo");
    auto value = p.parseValue();
    ASSERT_EQ(nullptr, value);
}

TEST(Parser, ParseBoolean) {
    Parser p("value foobar boolean false");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_EQ(value->name, "foobar");
    ASSERT_TRUE(value->value->isOfClass<Value_Boolean>());
    ASSERT_FALSE(runtime_ptr_cast<Value_Boolean>(value->value)->value());
}

TEST(Parser, ParseIllegalBoolean) {
    Parser p("value foobar boolean 123");
    auto value = p.parseValue();
    ASSERT_EQ(nullptr, value);
}

TEST(Parser, ParseTuple) {
    Parser p("value foobar tuple (boolean true , number 123 , empty)");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Tuple>());
    auto tpl = runtime_ptr_cast<Value_Tuple>(value->value);
    ASSERT_NE(nullptr, tpl);
    ASSERT_EQ(3, tpl->size());
    ASSERT_TRUE(tpl->at(0)->isOfClass<Value_Boolean>());
    ASSERT_TRUE(tpl->at(1)->isOfClass<Value_Number>());
    ASSERT_TRUE(tpl->at(2)->isOfClass<Value_Empty>());
}

TEST(Parser, ParseUnterminatedTuple) {
    Parser p("value foobar tuple ( boolean true ");
    ASSERT_EQ(nullptr, p.parseValue());
}

TEST(Parser, ParseMistypedTuple) {
    Parser p("value foobar tuple ( boolean true , number hello ) ");
    ASSERT_EQ(nullptr, p.parseValue());
}

TEST(Parser, ParseEmptyTuple) {
    Parser p("value foobar tuple (  )");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Tuple>());
    auto tpl = runtime_ptr_cast<Value_Tuple>(value->value);
    ASSERT_NE(nullptr, tpl);
    ASSERT_EQ(0, tpl->size());
}

TEST(Parser, ParseNestedTuple) {
    Parser p("value foobar tuple (tuple (empty, empty, boolean true, number 123), boolean false)");
    auto value = p.parseValue();
    auto outer_tuple = runtime_ptr_cast<Value_Tuple>(value->value);
    ASSERT_NE(nullptr, outer_tuple);
    ASSERT_EQ(2, outer_tuple->size());
    auto inner_tuple = runtime_ptr_cast<Value_Tuple>(outer_tuple->at(0));
    ASSERT_NE(nullptr, inner_tuple);
    ASSERT_EQ(4, inner_tuple->size());
}

TEST(Parser, ParseBlock) {
    Parser p("value main block { load foo, dup, add, resetstack, push empty, store bar }");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Block>());
    auto blk = runtime_ptr_cast<Value_Block>(value->value);
    ASSERT_EQ(6, blk->value()->size());
    ASSERT_TRUE(blk->value()->at(0)->isOfClass<Load>());
    ASSERT_TRUE(blk->value()->at(1)->isOfClass<Dup>());
    ASSERT_TRUE(blk->value()->at(2)->isOfClass<Add>());
    ASSERT_TRUE(blk->value()->at(3)->isOfClass<ResetStack>());
    ASSERT_TRUE(blk->value()->at(4)->isOfClass<Push>());
    ASSERT_TRUE(runtime_ptr_cast<Push>(blk->value()->at(4))->value()->isOfClass<Value_Empty>());
    ASSERT_TRUE(blk->value()->at(5)->isOfClass<Store>());
    ASSERT_EQ("bar", runtime_ptr_cast<Store>(blk->value()->at(5))->key());
}

TEST(Parser, ParseBlockWithSlots) {
    Parser p("value main block slots a b, c, d { }");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Block>());
    auto blk = runtime_ptr_cast<Value_Block>(value->value);
    ASSERT_EQ(4, blk->value()->numSlotValues());
    ASSERT_EQ("a", blk->value()->slotValueAt(0).value());
    ASSERT_EQ("b", blk->value()->slotValueAt(1).value());
    ASSERT_EQ("c", blk->value()->slotValueAt(2).value());
    ASSERT_EQ("d", blk->value()->slotValueAt(3).value());
    ASSERT_FALSE(blk->value()->slotValueAt(4).has_value());
}

TEST(Parser, ParseBlockEmptySlots) {
    Parser p("value main block slots { push number 1 }");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Block>());
    auto blk = runtime_ptr_cast<Value_Block>(value->value);
    ASSERT_EQ(0, blk->value()->numSlotValues());
    ASSERT_FALSE(blk->value()->slotValueAt(0).has_value());
    ASSERT_EQ(1, blk->value()->size());
    ASSERT_TRUE(blk->value()->at(0)->isOfClass<Push>());
}

TEST(Parser, ParseNewlineBlock) {
    Parser p("value main block {\n  load foo\n  dup\n  add\n  resetstack\n  push empty\n  store bar\n}");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Block>());
    auto blk = runtime_ptr_cast<Value_Block>(value->value);
    ASSERT_EQ(6, blk->value()->size());
    ASSERT_TRUE(blk->value()->at(0)->isOfClass<Load>());
    ASSERT_TRUE(blk->value()->at(1)->isOfClass<Dup>());
    ASSERT_TRUE(blk->value()->at(2)->isOfClass<Add>());
    ASSERT_TRUE(blk->value()->at(3)->isOfClass<ResetStack>());
    ASSERT_TRUE(blk->value()->at(4)->isOfClass<Push>());
    ASSERT_TRUE(runtime_ptr_cast<Push>(blk->value()->at(4))->value()->isOfClass<Value_Empty>());
    ASSERT_TRUE(blk->value()->at(5)->isOfClass<Store>());
    ASSERT_EQ("bar", runtime_ptr_cast<Store>(blk->value()->at(5))->key());
}

TEST(Parser, ParseSemicolonBlock) {
    Parser p("value main block {load foo;\n  dup\n  add;\n  resetstack; push empty\n  store bar\n}");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Block>());
    auto blk = runtime_ptr_cast<Value_Block>(value->value);
    ASSERT_EQ(6, blk->value()->size());
    ASSERT_TRUE(blk->value()->at(0)->isOfClass<Load>());
    ASSERT_TRUE(blk->value()->at(1)->isOfClass<Dup>());
    ASSERT_TRUE(blk->value()->at(2)->isOfClass<Add>());
    ASSERT_TRUE(blk->value()->at(3)->isOfClass<ResetStack>());
    ASSERT_TRUE(blk->value()->at(4)->isOfClass<Push>());
    ASSERT_TRUE(runtime_ptr_cast<Push>(blk->value()->at(4))->value()->isOfClass<Value_Empty>());
    ASSERT_TRUE(blk->value()->at(5)->isOfClass<Store>());
    ASSERT_EQ("bar", runtime_ptr_cast<Store>(blk->value()->at(5))->key());
}

TEST(Parser, ParseTwoValues) {
    Parser p("value zero number 0\nvalue no boolean false");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_EQ("zero", value->name);
    ASSERT_TRUE(value->value->isOfClass<Value_Number>());
    value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_EQ("no", value->name);
    ASSERT_TRUE(value->value->isOfClass<Value_Boolean>());
}

TEST(Parser, ParseError) {
    Parser p("value foobar error insufficientArguments");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Error>());
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(value->value)->value());
}

TEST(Parser, ParseInvalidError) {
    Parser p("value foobar error notARealError");
    auto value = p.parseValue();
    ASSERT_EQ(nullptr, value);
}

TEST(Parser, ParseType) {
    Parser p("value foobar type type");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Type>());
    ASSERT_EQ(ValueType::TYPE, runtime_ptr_cast<Value_Type>(value->value)->value());
}

TEST(Parser, ParseInvalidType) {
    Parser p("value foobar type invalid");
    auto value = p.parseValue();
    ASSERT_EQ(nullptr, value);
}

TEST(Parser, ParseOperation) {
    Parser p("value foobar operation dup");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Operation>());
    ASSERT_TRUE(runtime_ptr_cast<Value_Operation>(value->value)->value()->isOfClass<Dup>());
}

TEST(Parser, ParseInvalidOperation) {
    Parser p("value foobar operation notAReaLoepration");
    auto value = p.parseValue();
    ASSERT_EQ(nullptr, value);
}

TEST(Parser, ParseString) {
    Parser p("value foobar string \"dup\"");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_String>());
    ASSERT_EQ("\"dup\"", runtime_ptr_cast<Value_String>(value->value)->value());
}

TEST(Parser, ParseInvalidString) {
    Parser p("value foobar string dup\"");
    auto value = p.parseValue();
    ASSERT_EQ(nullptr, value);
}

TEST(Parser, ParseTable) {
    Parser p("value foobar table [number 123 -> boolean false, number 456 -> boolean true]");
    auto value = p.parseValue();
    ASSERT_NE(nullptr, value);
    ASSERT_TRUE(value->value->isOfClass<Value_Table>());
    auto tbl = runtime_ptr_cast<Value_Table>(value->value);
    ASSERT_NE(nullptr, tbl);
    ASSERT_EQ(2, tbl->size());
    ASSERT_TRUE(tbl->find(Value::fromNumber(123))->equals(Value::fromBoolean(false)));
    ASSERT_TRUE(tbl->find(Value::fromNumber(456))->equals(Value::fromBoolean(true)));
    ASSERT_TRUE(tbl->find(Value::fromNumber(121))->equals(Value::empty()));
}

TEST(Parser, ParseInvalidTable) {
    Parser p("value foobar table [number 123 boolean false, number 456 -> boolean true]");
    auto value = p.parseValue();
    ASSERT_EQ(nullptr, value);
}

TEST(Parser, FromFile) {
    FILE* f = tmpfile();
    fputs("value foobar number 123\nvalue barfoo number 321", f);
    rewind(f);
    auto sf = SharedFile::owned(f);
    Parser p(sf);
    ASSERT_TRUE(sf);
    ASSERT_EQ(f, sf.file());
    auto value = p.parseValue();
    ASSERT_EQ(0, p.errorsCount());
    ASSERT_NE(nullptr, value);
    ASSERT_EQ("foobar", value->name);
    ASSERT_NE(nullptr, value->value);
    ASSERT_TRUE(value->value->isOfClass<Value_Number>());
    ASSERT_EQ(123, value->value->asClass<Value_Number>()->value());
    value = p.parseValue();
    ASSERT_EQ(0, p.errorsCount());
    ASSERT_NE(nullptr, value);
    ASSERT_EQ("barfoo", value->name);
    ASSERT_NE(nullptr, value->value);
    ASSERT_TRUE(value->value->isOfClass<Value_Number>());
    ASSERT_EQ(321, value->value->asClass<Value_Number>()->value());
    value = p.parseValue();
    ASSERT_EQ(nullptr, value);
}

TEST(Parser, ConcurrentParsing) {
    Parser p1("value a number 123 value b number 321");
    Parser p2("value z number 500 value y number 400");
    auto v11 = p1.parseValue();
    ASSERT_NE(v11, nullptr);
    ASSERT_EQ("a", v11->name);
    ASSERT_TRUE(Value::fromNumber(123)->equals(v11->value));
    auto v21 = p2.parseValue();
    ASSERT_NE(v21, nullptr);
    ASSERT_EQ("z", v21->name);
    ASSERT_TRUE(Value::fromNumber(500)->equals(v21->value));
    auto v12 = p1.parseValue();
    ASSERT_NE(v12, nullptr);
    ASSERT_EQ("b", v12->name);
    ASSERT_TRUE(Value::fromNumber(321)->equals(v12->value));
    auto v22 = p2.parseValue();
    ASSERT_NE(v22, nullptr);
    ASSERT_EQ("y", v22->name);
    ASSERT_TRUE(Value::fromNumber(400)->equals(v22->value));
    ASSERT_EQ(nullptr, p1.parseValue());
    ASSERT_EQ(nullptr, p2.parseValue());
}

TEST(Parser, PeekIf) {
    Parser p("hello , 12345 boolean");
    ASSERT_TRUE(p.peekIf(TokenKind::IDENTIFIER));
    ASSERT_FALSE(p.peekIf(TokenKind::COMMA));
    ASSERT_EQ(TokenKind::IDENTIFIER, p.next()->kind());
    ASSERT_FALSE(p.peekIf(TokenKind::IDENTIFIER));
    ASSERT_TRUE(p.peekIf(TokenKind::COMMA));
    ASSERT_EQ(TokenKind::COMMA, p.next()->kind());
    ASSERT_TRUE(p.peekIf(TokenKind::NUMBER));
    ASSERT_FALSE(p.peekIf(TokenKind::ARROW));
    ASSERT_EQ("12345", p.next()->value());
    ASSERT_TRUE(p.peekIf(TokenKind::IDENTIFIER));
    ASSERT_EQ(TokenKind::IDENTIFIER, p.next()->kind());
    ASSERT_FALSE(p.peekIf(TokenKind::ARROW));
    ASSERT_FALSE(p.peekIf(TokenKind::COMMA));
    ASSERT_FALSE(p.peekIf(TokenKind::IDENTIFIER));
}

TEST(Parser, ValidBind) {
    Parser p_op("value foo bind number 24 operation dup");
    Parser p_block("value foo bind number 24 block slots $a { loadslot $a dup }");
    Parser p_nested("value foo bind number 24 bind number 12 operation mul");
    auto v_op = p_op.parseValue();
    auto v_block = p_block.parseValue();
    auto v_nested = p_nested.parseValue();

    ASSERT_NE(nullptr, v_op);
    ASSERT_NE(nullptr, v_block);
    ASSERT_NE(nullptr, v_nested);

    ASSERT_TRUE(v_op->value->isOfClass<Value_Bind>());
    ASSERT_TRUE(v_block->value->isOfClass<Value_Bind>());
    ASSERT_TRUE(v_nested->value->isOfClass<Value_Bind>());

    auto b_op = v_op->value->asClass<Value_Bind>();
    auto b_block = v_block->value->asClass<Value_Bind>();
    auto b_nested = v_nested->value->asClass<Value_Bind>();

    ASSERT_NE(nullptr, b_op->value()->callable().operation());
    ASSERT_NE(nullptr, b_block->value()->callable().block());
    ASSERT_NE(nullptr, b_nested->value()->callable().bind());
    ASSERT_NE(nullptr, b_nested->value()->callable().bind()->callable().operation());
}

TEST(Parser, InvalidBind) {
    Parser p("value foo bind empty boolean false");
    ASSERT_EQ(nullptr, p.parseValue());
}
