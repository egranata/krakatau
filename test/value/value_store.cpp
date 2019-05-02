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

#include <value/value_store.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <value/number.h>
#include <value/empty.h>

TEST(ValueStore, InsertNew) {
    ValueStore vs;
    auto v1 = Value::empty();
    auto v2 = Value::fromNumber(12);
    ASSERT_TRUE(vs.store("key1", v1));
    ASSERT_TRUE(vs.store("key2", v2));

    v1 = vs.retrieve("key1");
    v2 = vs.retrieve("key2");
    ASSERT_NE(v1, nullptr);
    ASSERT_NE(v2, nullptr);
    ASSERT_TRUE(v1->isOfClass<Value_Empty>());
    ASSERT_TRUE(v2->isOfClass<Value_Number>());
    ASSERT_EQ(12, runtime_ptr_cast<Value_Number>(v2)->value());
}

TEST(ValueStore, Overwrite) {
    ValueStore vs;
    auto v1 = Value::empty();
    auto v2 = Value::fromNumber(12);

    vs.store("key", v1);
    ASSERT_FALSE(vs.store("key", v2));
    ASSERT_TRUE(vs.retrieve("key")->isOfClass<Value_Empty>());

    ASSERT_TRUE(vs.store("key", v2, true));
    ASSERT_TRUE(vs.retrieve("key")->isOfClass<Value_Number>());
    ASSERT_EQ(12, runtime_ptr_cast<Value_Number>(vs.retrieve("key"))->value());
}

TEST(ValueStore, RetrieveMissing) {
    ValueStore vs;
    ASSERT_EQ(nullptr, vs.retrieve("noSuchkey"));
}

TEST(ValueStore, EraseExisting) {
    ValueStore vs;
    vs.store("key1", Value::empty());
    vs.store("key2", Value::empty());

    ASSERT_TRUE(vs.clear("key1"));
    ASSERT_NE(nullptr, vs.retrieve("key2"));
    ASSERT_EQ(nullptr, vs.retrieve("key1"));
}

TEST(ValueStore, EraseNotExisting) {
    ValueStore vs;
    vs.store("key2", Value::empty());

    ASSERT_FALSE(vs.clear("key1"));
    ASSERT_NE(nullptr, vs.retrieve("key2"));
    ASSERT_EQ(nullptr, vs.retrieve("key1"));
}
