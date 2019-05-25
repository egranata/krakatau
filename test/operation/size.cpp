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
#include <operation/size.h>
#include <value/value.h>
#include <value/tuple.h>
#include <value/number.h>
#include <gtest/gtest.h>
#include <value/boolean.h>
#include <rtti/rtti.h>
#include <value/string.h>
#include <value/table.h>

TEST(Size, EmptyStack) {
    MachineState s;
    Size sz;
    ASSERT_EQ(Operation::Result::ERROR, sz.execute(s));
}

TEST(Size, ValidElement) {
    MachineState s;
    Size sz;
    s.stack().push(Value::fromBoolean(true));
    ASSERT_EQ(Operation::Result::SUCCESS, sz.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(1, runtime_ptr_cast<Value_Number>(s.stack().pop())->value());
}

TEST(Size, Tuple) {
    MachineState s;
    Size sz;
    auto val = Value::tuple({});
    auto tpl = runtime_ptr_cast<Value_Tuple>(val);
    tpl->append(Value::empty());
    tpl->append(Value::empty());
    tpl->append(Value::empty());
    tpl->append(Value::empty());
    tpl->append(Value::empty());
    s.stack().push(val);
    ASSERT_EQ(Operation::Result::SUCCESS, sz.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(5, runtime_ptr_cast<Value_Number>(s.stack().pop())->value());
}

TEST(Size, String) {
    MachineState s;
    Size sz;
    auto val = Value::fromString("test test test");
    s.stack().push(val);
    ASSERT_EQ(Operation::Result::SUCCESS, sz.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(14, runtime_ptr_cast<Value_Number>(s.stack().pop())->value());
}

TEST(Size, Table) {
    MachineState s;
    Size sz;
    auto val = Value::table({});
    auto tbl = runtime_ptr_cast<Value_Table>(val);
    tbl->append(Value::fromBoolean(false), Value::empty());
    tbl->append(Value::fromBoolean(true), Value::empty());
    s.stack().push(val);
    ASSERT_EQ(Operation::Result::SUCCESS, sz.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(2, runtime_ptr_cast<Value_Number>(s.stack().pop())->value());
}
