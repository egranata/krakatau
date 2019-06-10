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
#include <gtest/gtest.h>
#include <operation/block.h>
#include <value/number.h>
#include <value/boolean.h>
#include <value/empty.h>
#include <value/string.h>
#include <value/block.h>
#include <value/operation.h>
#include <operation/dup.h>
#include <value/error.h>
#include <value/character.h>
#include <value/tuple.h>
#include <value/type.h>
#include <rtti/rtti.h>
#include <value/atom.h>
#include <value/table.h>
#include <operation/at.h>
#include <error/error_codes.h>
#include <value/set.h>
#include <parser/parser.h>
#include <value/bind.h>
#include <operation/bind.h>
#include <value/operation.h>

TEST(Value, Number) {
    auto v(Value::fromNumber(123));
    ASSERT_TRUE(v->isOfClass<Value_Number>());
    ASSERT_EQ(123, runtime_ptr_cast<Value_Number>(v)->value());

    ASSERT_FALSE(v->equals(Value::fromNumber(4)));
    ASSERT_FALSE(v->equals(Value::empty()));
    ASSERT_TRUE(v->equals(Value::fromNumber(123)));

    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Empty) {
    auto v(Value::empty());
    ASSERT_TRUE(v->isOfClass<Value_Empty>());

    ASSERT_FALSE(v->equals(Value::fromNumber(4)));
    ASSERT_TRUE(v->equals(Value::empty()));

    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Character) {
    auto v(Value::fromCharacter('A'));
    ASSERT_TRUE(v->isOfClass<Value_Character>());
    ASSERT_EQ('A', runtime_ptr_cast<Value_Character>(v)->value());

    ASSERT_FALSE(v->equals(Value::fromBoolean(true)));
    ASSERT_FALSE(v->equals(Value::fromNumber(1221)));
    ASSERT_FALSE(v->equals(Value::fromCharacter('B')));
    ASSERT_TRUE(v->equals(Value::fromCharacter('A')));

    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Boolean) {
    auto v(Value::fromBoolean(false));
    ASSERT_TRUE(v->isOfClass<Value_Boolean>());
    ASSERT_EQ(false, runtime_ptr_cast<Value_Boolean>(v)->value());

    ASSERT_FALSE(v->equals(Value::fromBoolean(true)));
    ASSERT_FALSE(v->equals(Value::fromNumber(1221)));
    ASSERT_TRUE(v->equals(Value::fromBoolean(false)));

    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Atom) {
    auto v(Value::atom("true"));
    ASSERT_TRUE(v->isOfClass<Value_Atom>());
    ASSERT_EQ("true", v->asClass<Value_Atom>()->value());

    ASSERT_TRUE(v->equals(v));
    ASSERT_TRUE(v->equals(Value::atom("true")));
    ASSERT_FALSE(v->equals(Value::atom("false")));
    ASSERT_FALSE(v->equals(Value::fromNumber(1221)));
}

TEST(Value, TypeMismatch) {
    std::shared_ptr<Value> v = Value::fromNumber(123);
    ASSERT_FALSE(v->isOfClass<Value_Boolean>());
    ASSERT_FALSE(v->isOfClass<Value_Empty>());
    ASSERT_EQ(nullptr, runtime_ptr_cast<Value_Boolean>(v));
    ASSERT_FALSE(v->isOfClass<Value_Block>());
    ASSERT_NE(nullptr, v->asClass<Value_Number>());
    ASSERT_EQ(nullptr, v->asClass<Value_Empty>());
    ASSERT_EQ(nullptr, v->asClass<Value_Boolean>());

    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Operation) {
    auto v(Value::fromOperation(std::shared_ptr<Operation>(new At())));
    ASSERT_TRUE(v->isOfClass<Value_Operation>());
    ASSERT_NE(nullptr, runtime_ptr_cast<Value_Operation>(v));
    ASSERT_EQ("at", v->describe());

    ASSERT_FALSE(v->equals(Value::fromBoolean(true)));
    ASSERT_FALSE(v->equals(Value::fromNumber(1221)));
    ASSERT_TRUE(Value::fromOperation(std::shared_ptr<Operation>(new At())));

    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Print) {
    ASSERT_EQ("4556", Value::fromNumber(4556)->describe());
    ASSERT_EQ("empty", Value::empty()->describe());
    ASSERT_EQ("true", Value::fromBoolean(true)->describe());
    ASSERT_EQ("0x65", Value::fromCharacter(0x65)->describe());
    ASSERT_EQ("false", Value::fromBoolean(false)->describe());
    ASSERT_EQ(" hello ", Value::fromString(" hello ")->describe());
    auto tpl_val = Value::tuple({Value::empty(), Value::fromString("hello"), Value::fromNumber(12)});
    ASSERT_EQ("(empty, hello, 12)", tpl_val->describe());
    ASSERT_EQ("string", Value::type(ValueType::STRING)->describe());
    auto tbl_val = Value::table({{Value::fromNumber(123), Value::fromNumber(42)}});
    ASSERT_EQ("[123 -> 42]", tbl_val->describe());
    ASSERT_EQ("bind(empty, dup)", Value::fromBind(std::make_shared<PartialBind>(Value::empty(), std::make_shared<Dup>()))->describe());
    auto set_val = Value::set({Value::empty()});
    ASSERT_EQ("[empty]", set_val->describe());
    ASSERT_EQ("atom(true)", Value::atom("true")->describe());
}

TEST(Value, String) {
    auto v(Value::fromString("hello world"));
    ASSERT_TRUE(v->isOfClass<Value_String>());
    ASSERT_EQ(U"hello world", runtime_ptr_cast<Value_String>(v)->value());

    ASSERT_FALSE(v->equals(Value::fromBoolean(true)));
    ASSERT_FALSE(v->equals(Value::fromString("   abc   ")));
    ASSERT_TRUE(v->equals(Value::fromString("hello world")));

    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, StringFromParserIsUnquoted) {
    Parser p("value foo string \"hello world\"");
    auto nv = p.parseValue();
    ASSERT_NE(nullptr, nv);
    ASSERT_NE(nv->value, nullptr);
    auto val = nv->value;
    ASSERT_TRUE(val->isOfClass<Value_String>());
    auto vs = val->asClass<Value_String>();
    ASSERT_EQ(U"hello world", vs->value());
}

TEST(Value, Error) {
    auto v(Value::error(ErrorCode::DIV_BY_ZERO));
    ASSERT_TRUE(v->isOfClass<Value_Error>());
    ASSERT_EQ(ErrorCode::DIV_BY_ZERO, v->asClass<Value_Error>()->value());

    ASSERT_FALSE(v->equals(Value::fromBoolean(true)));
    ASSERT_FALSE(v->equals(Value::error(ErrorCode::TYPE_MISMATCH)));
    ASSERT_TRUE(v->equals(Value::error(ErrorCode::DIV_BY_ZERO)));

    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Tuple) {
    Value_Tuple vp;
    ASSERT_EQ(0, vp.size());
    ASSERT_TRUE(vp.at(12)->isOfClass<Value_Empty>());
    vp.append(Value::empty());
    ASSERT_EQ(1, vp.size());
    ASSERT_TRUE(vp.at(0)->isOfClass<Value_Empty>());
    vp.append(Value::fromNumber(12));
    ASSERT_EQ(2, vp.size());
    ASSERT_TRUE(vp.at(0)->isOfClass<Value_Empty>());
    ASSERT_TRUE(vp.at(1)->isOfClass<Value_Number>());
    vp.append(Value::fromBoolean(false));
    ASSERT_EQ(3, vp.size());
    ASSERT_TRUE(vp.at(0)->isOfClass<Value_Empty>());
    ASSERT_TRUE(vp.at(1)->isOfClass<Value_Number>());
    ASSERT_TRUE(vp.at(2)->isOfClass<Value_Boolean>());
    ASSERT_TRUE(vp.at(3)->isOfClass<Value_Empty>());

    Parser p("tuple (number 12, boolean false, empty, number 24)");
    auto v = p.parseValuePayload();
    ASSERT_NE(nullptr, v);
    ASSERT_TRUE(v->isOfClass<Value_Tuple>());
    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Type) {
    auto v(Value::type(ValueType::NUMBER));
    ASSERT_TRUE(v->isOfClass<Value_Type>());
    ASSERT_EQ(ValueType::NUMBER, runtime_ptr_cast<Value_Type>(v)->value());

    ASSERT_TRUE(v->equals(Value::type(ValueType::NUMBER)));
    ASSERT_FALSE(v->equals(Value::empty()));
    ASSERT_FALSE(v->equals(Value::type(ValueType::STRING)));

    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Table) {
    auto tbl_val = Value::table({});
    auto tbl = runtime_ptr_cast<Value_Table>(tbl_val);
    ASSERT_EQ(0, tbl->size());
    tbl->append(Value::fromNumber(123), Value::fromNumber(42));
    ASSERT_EQ(1, tbl->size());
    ASSERT_TRUE(tbl->keyAt(0)->isOfClass<Value_Number>());
    ASSERT_TRUE(tbl->valueAt(0)->isOfClass<Value_Number>());
    ASSERT_TRUE(tbl->find(Value::fromNumber(123), nullptr)->isOfClass<Value_Number>());
    tbl->append(Value::fromNumber(456), Value::fromBoolean(false));
    ASSERT_EQ(2, tbl->size());
    ASSERT_TRUE(tbl->find(Value::fromNumber(123), nullptr)->isOfClass<Value_Number>());
    ASSERT_TRUE(tbl->find(Value::fromNumber(456), nullptr)->isOfClass<Value_Boolean>());

    Parser p("table [number 1 -> boolean true, number 0 -> boolean false, number 2 -> empty]");
    auto v = p.parseValuePayload();
    ASSERT_NE(nullptr, v);
    ASSERT_TRUE(v->isOfClass<Value_Table>());
    ASSERT_TRUE(v->equals(v->clone()));
}

TEST(Value, Bind) {
    auto val_bind = Value::fromBind(std::make_shared<PartialBind>(Value::empty(), std::make_shared<Dup>()));
    ASSERT_TRUE(val_bind->isOfClass<Value_Operation>());
    ASSERT_TRUE(val_bind->equals(val_bind->clone()));
}

TEST(Value, Set) {
    auto set = Value::set({});
    ASSERT_EQ(0, set->size());
    set->append(Value::fromNumber(123))->append(Value::fromNumber(42));
    ASSERT_EQ(2, set->size());
    ASSERT_TRUE(set->valueAt(0)->isOfClass<Value_Number>());
    ASSERT_TRUE(set->find(Value::fromNumber(123)));
    ASSERT_FALSE(set->find(Value::fromNumber(555)));
    set->append(Value::fromNumber(456));
    ASSERT_EQ(3, set->size());
    ASSERT_TRUE(set->find(Value::fromNumber(123)));
    ASSERT_TRUE(set->find(Value::fromNumber(456)));

    Parser p("set [number 1, number 2, number 3]");
    auto v = p.parseValuePayload();
    ASSERT_NE(nullptr, v);
    ASSERT_TRUE(v->isOfClass<Value_Set>());
    ASSERT_TRUE(v->equals(v->clone()));
}
