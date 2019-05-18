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
#include <operation/swap.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/boolean.h>
#include <value/empty.h>

TEST(Swap, NotEnoughState) {
    MachineState s;
    Swap w;
    ASSERT_EQ(Operation::Result::ERROR, w.execute(s));
    s.stack().pop();
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, w.execute(s));
}

TEST(Swap, ValidData) {
    MachineState s;
    Swap w;
    s.stack().push(Value::fromBoolean(true));
    s.stack().push(Value::empty());
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Empty>());
    w.execute(s);
    ASSERT_EQ(2, s.stack().size());
    ASSERT_TRUE(s.stack().pop()->isOfClass<Value_Boolean>());
    ASSERT_TRUE(s.stack().pop()->isOfClass<Value_Empty>());
}
