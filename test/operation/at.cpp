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
#include <operation/at.h>
#include <value/character.h>
#include <value/value.h>
#include <value/tuple.h>
#include <value/number.h>
#include <gtest/gtest.h>
#include <value/boolean.h>
#include <rtti/rtti.h>
#include <value/error.h>
#include <value/string.h>
#include <value/table.h>
#include <value/set.h>
#include <value/empty.h>

TEST(At, ZeroArgs) {
    MachineState s;
    At at;
    ASSERT_EQ(Operation::Result::ERROR, at.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(0, s.stack().size());
}

TEST(At, OneArg) {
    MachineState s;
    At at;
    s.stack().push(Value::tuple({}));
    ASSERT_EQ(Operation::Result::ERROR, at.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(1, s.stack().size());
}

TEST(At, MistypedArgs) {
    MachineState s;
    At at;
    s.stack().push(Value::tuple({}));
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, at.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
}

TEST(At, TupleOutOfBounds) {
    MachineState s;
    At at;
    auto val = Value::tuple({Value::empty(), Value::empty()});
    s.stack().push(val);
    s.stack().push(Value::fromNumber(4));
    ASSERT_EQ(Operation::Result::ERROR, at.execute(s));
    ASSERT_EQ(ErrorCode::OUT_OF_BOUNDS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
}

TEST(At, ValidTuple) {
    MachineState s;
    At at;
    auto val = Value::tuple({Value::empty(), Value::empty(), Value::fromBoolean(true)});
    s.stack().push(val);
    s.stack().push(Value::fromNumber(2));
    ASSERT_EQ(Operation::Result::SUCCESS, at.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_EQ(true, runtime_ptr_cast<Value_Boolean>(s.stack().pop())->value());
}

TEST(At, Rtti) {
    At at;
    ASSERT_TRUE(at.isOfClass<At>());
}

TEST(At, StringOutOfBounds) {
    MachineState s;
    At at;
    auto val = Value::fromString("abc");
    s.stack().push(val);
    s.stack().push(Value::fromNumber(14));
    ASSERT_EQ(Operation::Result::ERROR, at.execute(s));
    ASSERT_EQ(ErrorCode::OUT_OF_BOUNDS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
}

TEST(At, ValidString) {
    MachineState s;
    At at;
    auto val = Value::fromString("\1\2\3\4");
    s.stack().push(val);
    s.stack().push(Value::fromNumber(2));
    ASSERT_EQ(Operation::Result::SUCCESS, at.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_EQ(3, runtime_ptr_cast<Value_Character>(s.stack().pop())->value());
}

TEST(At, Table) {
    MachineState s;
    At at;
    auto val = Value::table({});
    auto tbl = runtime_ptr_cast<Value_Table>(val);
    tbl->append(Value::fromBoolean(true), Value::fromNumber(456));
    s.stack().push(val);
    s.stack().push(Value::fromNumber(0));
    ASSERT_EQ(Operation::Result::SUCCESS, at.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Tuple>());
    auto tpl = s.stack().peek()->asClass<Value_Tuple>();
    ASSERT_EQ(2, tpl->size());
    ASSERT_TRUE(Value::fromBoolean(true)->equals(tpl->at(0)));
    ASSERT_TRUE(Value::fromNumber(456)->equals(tpl->at(1)));
}

TEST(At, MissingTable) {
    MachineState s;
    At at;
    auto val = Value::table({ {Value::fromBoolean(false), Value::fromNumber(123)} });
    val->append(Value::fromBoolean(true), Value::fromBoolean(456));
    s.stack().push(val);
    s.stack().push(Value::fromNumber(12));
    ASSERT_EQ(Operation::Result::ERROR, at.execute(s));
    ASSERT_EQ(ErrorCode::OUT_OF_BOUNDS, runtime_ptr_cast<Value_Error>(s.stack().peek())->value());
}

TEST(At, Set) {
    MachineState s;
    At at;
    auto val = Value::set({Value::fromBoolean(false)});
    s.stack().push(val);
    s.stack().push(Value::fromNumber(0));
    ASSERT_EQ(Operation::Result::SUCCESS, at.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->equals(Value::fromBoolean(false)));
}
