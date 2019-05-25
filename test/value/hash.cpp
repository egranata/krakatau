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
#include <value/number.h>
#include <value/tuple.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <value/boolean.h>

TEST(Value, HashConsistent) {
    auto val(Value::fromNumber(123));
    ASSERT_EQ(val->hash(), val->hash());
}

TEST(Value, IdenticalSameHash) {
    auto val1(Value::tuple({}));
    auto val2(Value::tuple({}));

    runtime_ptr_cast<Value_Tuple>(val1)->append(Value::fromNumber(12));
    runtime_ptr_cast<Value_Tuple>(val1)->append(Value::fromBoolean(false));

    runtime_ptr_cast<Value_Tuple>(val2)->append(Value::fromNumber(12));
    runtime_ptr_cast<Value_Tuple>(val2)->append(Value::fromBoolean(false));

    ASSERT_TRUE(val1->equals(val2));
    ASSERT_EQ(val1->hash(), val2->hash());
}
