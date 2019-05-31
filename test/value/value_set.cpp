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
#include <value/value_set.h>
#include <value/boolean.h>
#include <value/number.h>
#include <value/empty.h>
#include <value/tuple.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>

TEST(ValueSet, StartsEmpty) {
    ValueSet vt;
    ASSERT_EQ(0, vt.size());
}

TEST(ValueSet, InsertSizesUp) {
    ValueSet vt;
    vt.add(Value::empty());
    ASSERT_EQ(1, vt.size());
    vt.add(Value::fromNumber(12));
    ASSERT_EQ(2, vt.size());
}

TEST(ValueSet, FindExisting) {
    ValueSet vt;
    vt.add(Value::empty());;
    ASSERT_TRUE(vt.find(Value::empty()));
}

TEST(ValueSet, InsertEqual) {
    ValueSet vt;
    ASSERT_TRUE(vt.add(Value::empty()));
    ASSERT_EQ(1, vt.size());
    ASSERT_FALSE(vt.add(Value::empty()));
    ASSERT_EQ(1, vt.size());
    ASSERT_TRUE(vt.find(Value::empty()));
}

TEST(ValueSet, FindMissing) {
    ValueSet vt;
    vt.add(Value::fromNumber(12));
    ASSERT_FALSE(vt.find(Value::tuple({})));
}

TEST(ValueSet, At) {
    ValueSet vt;
    vt.add(Value::fromBoolean(true));
    vt.add(Value::fromBoolean(false));
    ASSERT_EQ(2, vt.size());
    ASSERT_NE(nullptr, vt.at(0));
    ASSERT_NE(nullptr, vt.at(1));
    ASSERT_EQ(nullptr, vt.at(2));
    ASSERT_EQ(nullptr, vt.at(3));
    ASSERT_TRUE(vt.at(0)->isOfClass<Value_Boolean>());
    ASSERT_TRUE(vt.at(1)->isOfClass<Value_Boolean>());
    ASSERT_FALSE(vt.at(0)->equals(vt.at(1)));
}

