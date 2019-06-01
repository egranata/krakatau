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
#include <value/iterable.h>
#include <value/tuple.h>
#include <value/empty.h>
#include <value/number.h>
#include <gtest/gtest.h>
#include <value/boolean.h>
#include <value/iterator.h>

TEST(Iterable, CorrectCast) {
    auto tpl = Value::tuple({Value::empty(), Value::fromBoolean(false)});
    auto num = Value::fromNumber(123);

    ASSERT_EQ(nullptr, std::dynamic_pointer_cast<IterableValue>(num));
    ASSERT_NE(nullptr, std::dynamic_pointer_cast<IterableValue>(tpl));

    ASSERT_EQ(nullptr, IterableValue::asIterable(num));
    ASSERT_NE(nullptr, IterableValue::asIterable(tpl));
}

TEST(Iterable, Size) {
    auto tpl = Value::tuple({Value::empty(), Value::fromBoolean(false)});
    auto itr = IterableValue::asIterable(tpl);
    ASSERT_NE(nullptr, itr);
    ASSERT_EQ(2, itr->size());
}

TEST(Iterable, Elements) {
    auto tpl = Value::tuple({Value::empty(), Value::fromBoolean(false)});
    auto itr = IterableValue::asIterable(tpl);
    ASSERT_NE(nullptr, itr);
    ASSERT_TRUE(tpl->at(0)->equals(itr->at(0)));
    ASSERT_TRUE(tpl->at(1)->equals(itr->at(1)));
}

TEST(Iterable, Equality) {
    auto tpl = Value::tuple({Value::empty(), Value::fromBoolean(false)});
    auto num = Value::fromNumber(123);

    auto itpl = IterableValue::asIterable(tpl);
    auto inum = IterableValue::asIterable(num);

    ASSERT_FALSE(itpl->equals(inum));
    ASSERT_TRUE(itpl->equals(itpl));
}

TEST(Iterable, Iterator) {
    auto tpl1 = Value::tuple({Value::empty(), Value::fromBoolean(false)});
    auto tpl2 = std::dynamic_pointer_cast<Value_Tuple>(tpl1->clone());

    auto itpl = IterableValue::asIterable(tpl1);

    int i = 0;
    for (const auto& val : *itpl) {
        ASSERT_TRUE(tpl2->at(i)->equals(val));
        ++i;
    }
    ASSERT_EQ(2, i);
}
