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
#include <operation/typeof.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/type.h>
#include <rtti/rtti.h>
#include <value/boolean.h>

TEST(Typeof, EmptyStack) {
    MachineState s;
    Typeof t;
    ASSERT_EQ(Operation::Result::ERROR, t.execute(s));
}

TEST(Typeof, ValidElement) {
    MachineState s;
    Typeof t;
    s.stack().push(Value::fromBoolean(true));
    ASSERT_EQ(Operation::Result::SUCCESS, t.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Type>());
    ASSERT_EQ(ValueType::BOOLEAN, runtime_ptr_cast<Value_Type>(s.stack().peek())->value());
    ASSERT_EQ(Operation::Result::SUCCESS, t.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Type>());
    ASSERT_EQ(ValueType::TYPE, runtime_ptr_cast<Value_Type>(s.stack().peek())->value());
}
