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
#include <operation/find.h>
#include <rtti/rtti.h>
#include <value/set.h>
#include <value/table.h>
#include <value/error.h>
#include <value/empty.h>
#include <value/number.h>
#include <gtest/gtest.h>
#include <value/boolean.h>

TEST(Find, ZeroArgs) {
    MachineState s;
    Find f;
    ASSERT_EQ(Operation::Result::ERROR, f.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(0, s.stack().size());
}

TEST(Find, OneArg) {
    MachineState s;
    Find f;
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, f.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(1, s.stack().size());
}

TEST(Find, MistypedArgs) {
    MachineState s;
    Find f;
    s.stack().push(Value::empty());
    s.stack().push(Value::set({}));
    ASSERT_EQ(Operation::Result::ERROR, f.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
}

TEST(Find, SetPresent) {
    MachineState s;
    Find f;
    s.stack().push(Value::set( {Value::fromNumber(123), Value::fromNumber(555)} ));
    s.stack().push(Value::fromNumber(555));
    ASSERT_EQ(Operation::Result::SUCCESS, f.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->asClass<Value_Boolean>()->value());
}

TEST(Find, TablePresent) {
    MachineState s;
    Find f;
    s.stack().push(Value::table( {
        {Value::fromNumber(123), Value::fromNumber(321)},
        {Value::fromNumber(456), Value::fromNumber(654)},
    } ));
    s.stack().push(Value::fromNumber(456));
    ASSERT_EQ(Operation::Result::SUCCESS, f.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_TRUE(Value::fromNumber(654)->equals(s.stack().peek()));
}

TEST(Find, SetMissing) {
    MachineState s;
    Find f;
    s.stack().push(Value::set( {Value::fromNumber(123), Value::fromNumber(555)} ));
    s.stack().push(Value::fromNumber(444));
    ASSERT_EQ(Operation::Result::SUCCESS, f.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_EQ(ErrorCode::NOT_FOUND, s.stack().peek()->asClass<Value_Error>()->value());
}

TEST(Find, TableMissing) {
    MachineState s;
    Find f;
    s.stack().push(Value::table( {
        {Value::fromNumber(123), Value::fromNumber(321)},
        {Value::fromNumber(456), Value::fromNumber(654)},
    } ));
    s.stack().push(Value::fromNumber(321));
    ASSERT_EQ(Operation::Result::SUCCESS, f.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_EQ(ErrorCode::NOT_FOUND, s.stack().peek()->asClass<Value_Error>()->value());
}

