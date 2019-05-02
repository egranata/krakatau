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
#include <value/block.h>
#include <value/tuple.h>
#include <value/table.h>
#include <function/block.h>
#include <rtti/rtti.h>

TEST(ParserCommas, EmptyTuple) {
    Parser p("value foo tuple (,)");
    auto val = p.parseValue();
    ASSERT_EQ(0, p.errorsCount());
    ASSERT_NE(val, nullptr);
    ASSERT_NE(nullptr, val->value);
    ASSERT_TRUE(val->value->isOfClass<Value_Tuple>());
    ASSERT_EQ(0, runtime_ptr_cast<Value_Tuple>(val->value)->size());
}

TEST(ParserCommas, TrailingCommaTuple) {
    Parser p1("value foo tuple (empty,number 123,)");
    auto val1 = p1.parseValue();
    ASSERT_EQ(0, p1.errorsCount());
    Parser p2("value foo tuple (empty,number 123)");
    auto val2 = p2.parseValue();
    ASSERT_EQ(0, p2.errorsCount());
    ASSERT_NE(val1, nullptr);
    ASSERT_NE(nullptr, val1->value);
    ASSERT_NE(val2, nullptr);
    ASSERT_NE(nullptr, val2->value);
    ASSERT_TRUE(val1->value->isOfClass<Value_Tuple>());
    ASSERT_TRUE(val2->value->isOfClass<Value_Tuple>());
    ASSERT_TRUE(val1->value->equals(val2->value));
}

TEST(ParserCommas, EmptyTable) {
    Parser p("value foo table [,]");
    auto val = p.parseValue();
    ASSERT_EQ(0, p.errorsCount());
    ASSERT_NE(val, nullptr);
    ASSERT_NE(nullptr, val->value);
    ASSERT_TRUE(val->value->isOfClass<Value_Table>());
    ASSERT_EQ(0, runtime_ptr_cast<Value_Table>(val->value)->size());
}

TEST(ParserCommas, TrailingCommaTable) {
    Parser p1("value foo table [number 1 -> empty, boolean false -> empty,]");
    auto val1 = p1.parseValue();
    ASSERT_EQ(0, p1.errorsCount());
    Parser p2("value foo table [number 1 -> empty, boolean false -> empty]");
    auto val2 = p2.parseValue();
    ASSERT_EQ(0, p2.errorsCount());
    ASSERT_NE(val1, nullptr);
    ASSERT_NE(nullptr, val1->value);
    ASSERT_NE(val2, nullptr);
    ASSERT_NE(nullptr, val2->value);
    ASSERT_TRUE(val1->value->isOfClass<Value_Table>());
    ASSERT_TRUE(val2->value->isOfClass<Value_Table>());
    ASSERT_TRUE(val1->value->equals(val2->value));
}

TEST(ParserCommas, EmptyBlock) {
    Parser p("value foo block {,}");
    auto val = p.parseValue();
    ASSERT_EQ(0, p.errorsCount());
    ASSERT_NE(val, nullptr);
    ASSERT_NE(nullptr, val->value);
    ASSERT_TRUE(val->value->isOfClass<Value_Block>());
    ASSERT_EQ(0, runtime_ptr_cast<Value_Block>(val->value)->value()->size());
}

TEST(ParserCommas, TrailingCommaBlock) {
    Parser p1("value foo block {nop,nop,}");
    auto val1 = p1.parseValue();
    ASSERT_EQ(0, p1.errorsCount());
    Parser p2("value foo block {nop,nop}");
    auto val2 = p2.parseValue();
    ASSERT_EQ(0, p2.errorsCount());
    ASSERT_NE(val1, nullptr);
    ASSERT_NE(nullptr, val1->value);
    ASSERT_NE(val2, nullptr);
    ASSERT_NE(nullptr, val2->value);
    ASSERT_TRUE(val1->value->isOfClass<Value_Block>());
    ASSERT_TRUE(val2->value->isOfClass<Value_Block>());
    ASSERT_TRUE(val1->value->equals(val2->value));
}
