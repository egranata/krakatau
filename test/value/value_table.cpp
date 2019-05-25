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
#include <value/value_table.h>
#include <value/boolean.h>
#include <value/number.h>
#include <value/empty.h>
#include <value/tuple.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>

TEST(ValueTable, StartsEmpty) {
    ValueTable vt;
    ASSERT_EQ(0, vt.size());
}

TEST(ValueTable, InsertSizesUp) {
    ValueTable vt;
    vt.add(Value::empty(), Value::fromNumber(12));
    ASSERT_EQ(1, vt.size());
    vt.add(Value::fromNumber(12), Value::fromBoolean(false));
    ASSERT_EQ(2, vt.size());
}

TEST(ValueTable, FindExisting) {
    ValueTable vt;
    vt.add(Value::empty(), Value::fromNumber(12));
    ASSERT_TRUE(vt.find(Value::empty())->isOfClass<Value_Number>());
    ASSERT_EQ(12, runtime_ptr_cast<Value_Number>(vt.find(Value::empty()))->value());
}

TEST(ValueTable, InsertEqual) {
    ValueTable vt;
    ASSERT_TRUE(vt.add(Value::empty(), Value::fromNumber(12)));
    ASSERT_EQ(1, vt.size());
    ASSERT_FALSE(vt.add(Value::empty(), Value::fromBoolean(false)));
    ASSERT_EQ(1, vt.size());
    ASSERT_TRUE(vt.find(Value::empty())->isOfClass<Value_Number>());
    ASSERT_EQ(12, runtime_ptr_cast<Value_Number>(vt.find(Value::empty()))->value());
}

TEST(ValueTable, FindMissing) {
    ValueTable vt;
    vt.add(Value::empty(), Value::fromNumber(12));
    ASSERT_EQ(nullptr, vt.find(Value::tuple({})));
}

TEST(ValueTable, KeyAt) {
    ValueTable vt;
    vt.add(Value::fromBoolean(true), Value::fromNumber(12));
    vt.add(Value::fromBoolean(false), Value::fromNumber(24));
    ASSERT_EQ(2, vt.size());
    ASSERT_NE(nullptr, vt.keyAt(0));
    ASSERT_NE(nullptr, vt.keyAt(1));
    ASSERT_EQ(nullptr, vt.keyAt(2));
    ASSERT_EQ(nullptr, vt.keyAt(3));
    ASSERT_TRUE(vt.keyAt(0)->isOfClass<Value_Boolean>());
    ASSERT_TRUE(vt.keyAt(1)->isOfClass<Value_Boolean>());
    ASSERT_EQ(false,
        runtime_ptr_cast<Value_Boolean>(vt.keyAt(0))->value() && runtime_ptr_cast<Value_Boolean>(vt.keyAt(1))->value());
    ASSERT_EQ(true,
        runtime_ptr_cast<Value_Boolean>(vt.keyAt(0))->value() || runtime_ptr_cast<Value_Boolean>(vt.keyAt(1))->value());
}

TEST(ValueTable, ValueAt) {
    ValueTable vt;
    vt.add(Value::fromBoolean(true), Value::fromNumber(12));
    vt.add(Value::fromBoolean(false), Value::fromNumber(24));
    ASSERT_EQ(2, vt.size());
    ASSERT_NE(nullptr, vt.valueAt(0));
    ASSERT_NE(nullptr, vt.valueAt(1));
    ASSERT_EQ(nullptr, vt.valueAt(2));
    ASSERT_EQ(nullptr, vt.valueAt(3));
    ASSERT_TRUE(vt.valueAt(0)->isOfClass<Value_Number>());
    ASSERT_TRUE(vt.valueAt(1)->isOfClass<Value_Number>());
    ASSERT_EQ(36,
        runtime_ptr_cast<Value_Number>(vt.valueAt(0))->value() + runtime_ptr_cast<Value_Number>(vt.valueAt(1))->value());
    ASSERT_EQ(288,
        runtime_ptr_cast<Value_Number>(vt.valueAt(0))->value() * runtime_ptr_cast<Value_Number>(vt.valueAt(1))->value());
}

TEST(ValueTable, At) {
    ValueTable vt;
    vt.add(Value::fromBoolean(true), Value::fromNumber(12));
    vt.add(Value::fromBoolean(false), Value::fromNumber(24));
    ASSERT_EQ(2, vt.size());
    ASSERT_NE(nullptr, vt.at(0));
    ASSERT_NE(nullptr, vt.at(1));
    ASSERT_EQ(nullptr, vt.at(2));
    ASSERT_EQ(nullptr, vt.at(3));
    ASSERT_TRUE(vt.at(0)->isOfClass<Value_Tuple>());
    ASSERT_TRUE(vt.at(1)->isOfClass<Value_Tuple>());
}
