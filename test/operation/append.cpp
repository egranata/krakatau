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

#include <machine/state.h>
#include <operation/append.h>
#include <value/value.h>
#include <value/tuple.h>
#include <value/number.h>
#include <gtest/gtest.h>
#include <value/boolean.h>
#include <rtti/rtti.h>
#include <value/error.h>
#include <value/string.h>
#include <value/table.h>
#include <value/empty.h>
#include <value/operation.h>

TEST(Append, ZeroArgs) {
    MachineState s;
    Append a;
    ASSERT_EQ(Operation::Result::ERROR, a.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(0, s.stack().size());
}

TEST(Append, OneArg) {
    MachineState s;
    Append a;
    s.stack().push(Value::tuple({}));
    ASSERT_EQ(Operation::Result::ERROR, a.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(1, s.stack().size());
}

TEST(Append, MistypedArgs) {
    MachineState s;
    Append a;
    s.stack().push(Value::fromBoolean(false));
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, a.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
}

TEST(Append, Tuple) {
    MachineState s;
    Append a;
    auto vtpl0 = Parser("tuple (number 1, number 2)").parseValuePayload();
    auto tpl0 = vtpl0->asClass<Value_Tuple>();
    s.stack().push(vtpl0);
    s.stack().push(Value::fromNumber(3));
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(s));
    ASSERT_EQ(1, s.stack().size());
    auto vtpl1 = s.stack().pop();
    auto tpl1 = vtpl1->asClass<Value_Tuple>();
    ASSERT_NE(nullptr, tpl1);
    ASSERT_EQ(3, tpl1->size());
    ASSERT_EQ(2, tpl0->size());
    ASSERT_TRUE(Value::fromNumber(1)->equals(tpl1->at(0)));
    ASSERT_TRUE(Value::fromNumber(2)->equals(tpl1->at(1)));
    ASSERT_TRUE(Value::fromNumber(3)->equals(tpl1->at(2)));
    ASSERT_TRUE(Value::fromNumber(1)->equals(tpl0->at(0)));
    ASSERT_TRUE(Value::fromNumber(2)->equals(tpl0->at(1)));
    ASSERT_FALSE(vtpl0->equals(vtpl1));
}

TEST(Append, TableInvalidType) {
    MachineState s;
    Append a;
    auto vtbl = Parser("table [number 1 -> boolean true, number 2 -> boolean false]").parseValuePayload();
    s.stack().push(vtbl);
    s.stack().push(Value::fromNumber(3));
    ASSERT_EQ(Operation::Result::ERROR, a.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
    ASSERT_TRUE(Value::fromNumber(3)->equals(s.stack().pop()));
    ASSERT_TRUE(vtbl->equals(s.stack().peek()));
}

TEST(Append, TableInvalidSize) {
    MachineState s;
    Append a;
    auto vtbl = Parser("table [number 1 -> boolean true, number 2 -> boolean false]").parseValuePayload();
    auto vtpl = Parser("tuple (number 0, number 100, boolean false, empty)").parseValuePayload();
    s.stack().push(vtbl);
    s.stack().push(vtpl);
    ASSERT_EQ(Operation::Result::ERROR, a.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
    ASSERT_TRUE(vtpl->equals(s.stack().pop()));
    ASSERT_TRUE(vtbl->equals(s.stack().pop()));
}

TEST(Append, Table) {
    MachineState s;
    Append a;
    auto vtbl0 = Parser("table [number 1 -> boolean true, number 2 -> boolean false]").parseValuePayload();
    auto vtpl = Parser("tuple (number 5, string \"hello\")").parseValuePayload();
    auto vtbl1 = Parser("table [number 1 -> boolean true, number 2 -> boolean false, number 5 -> string \"hello\"]").parseValuePayload();
    auto tbl0 = vtbl0->asClass<Value_Table>();
    ASSERT_NE(nullptr, vtbl0);
    ASSERT_NE(nullptr, vtpl);
    s.stack().push(vtbl0);
    s.stack().push(vtpl);
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_TRUE(vtbl1->equals(s.stack().peek()));
    ASSERT_EQ(2, tbl0->size());
    ASSERT_FALSE(vtbl0->equals(vtbl1));
}

TEST(Append, ParseAndSerialize) {
    auto op = Parser("value foo operation append").parseValue();
    ASSERT_TRUE(op->value->isOfClass<Value_Operation>());
    ASSERT_TRUE(op->value->asClass<Value_Operation>()->value()->isOfClass<Append>());
    Serializer sz;
    op->value->serialize(&sz);
    auto bs = ByteStream::anonymous(sz.data(), sz.size());
    auto dsz = Value::fromByteStream(bs.get());
    ASSERT_NE(dsz, nullptr);
    ASSERT_TRUE(dsz->isOfClass<Value_Operation>());
    ASSERT_TRUE(dsz->asClass<Value_Operation>()->value()->isOfClass<Append>());
    ASSERT_TRUE(dsz->equals(op->value));
}
