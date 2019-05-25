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

#include <stack/stack.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/number.h>
#include <value/empty.h>
#include <value/boolean.h>

TEST(Stack, PushPop) {
    Stack s;
    s.push(Value::empty());
    ASSERT_FALSE(s.empty());
    auto pv = s.pop();
    ASSERT_NE(pv, nullptr);
    ASSERT_TRUE(pv->isOfClass<Value_Empty>());
}

TEST(Stack, Empty) {
    Stack s;
    ASSERT_TRUE(s.empty());
    s.push(Value::empty());
    ASSERT_FALSE(s.empty());
    s.push(Value::empty());
    ASSERT_FALSE(s.empty());
    s.pop();
    ASSERT_FALSE(s.empty());
    s.pop();
    ASSERT_TRUE(s.empty());
}

TEST(Stack, Size) {
    Stack s;
    ASSERT_EQ(0, s.size());
    s.push(Value::empty());
    ASSERT_EQ(1, s.size());
    s.push(Value::empty());
    s.push(Value::empty());
    ASSERT_EQ(3, s.size());
    s.pop();
    ASSERT_EQ(2, s.size());
}

TEST(Stack, HasAtLeast) {
    Stack s;
    ASSERT_FALSE(s.hasAtLeast(2));
    s.push(Value::empty());
    s.push(Value::empty());
    ASSERT_TRUE(s.hasAtLeast(2));
    s.push(Value::empty());
    ASSERT_TRUE(s.hasAtLeast(2));
    s.pop();
    ASSERT_TRUE(s.hasAtLeast(1));
    s.pop();
    ASSERT_FALSE(s.hasAtLeast(2));
    ASSERT_TRUE(s.hasAtLeast(1));
    s.pop();
    ASSERT_FALSE(s.hasAtLeast(1));
}

TEST(Stack, Reset) {
    Stack s;
    s.push(Value::empty());
    s.push(Value::empty());
    s.push(Value::empty());
    s.push(Value::empty());
    ASSERT_NE(0, s.size());
    s.reset();
    ASSERT_EQ(0, s.size());
    s.reset();
    ASSERT_EQ(0, s.size());
}

TEST(Stack, Describe) {
    Stack s;
    s.push(Value::empty());
    s.push(Value::fromNumber(1234));
    s.push(Value::fromBoolean(false));
    ASSERT_EQ("false\n1234\nempty", s.describe());
    s.push(Value::empty());
    ASSERT_EQ("empty\nfalse\n1234\nempty", s.describe());
    s.reset();
    ASSERT_EQ("", s.describe());
}
