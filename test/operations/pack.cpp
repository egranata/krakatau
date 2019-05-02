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
#include <operations/pack.h>
#include <value/value.h>
#include <value/tuple.h>
#include <value/number.h>
#include <gtest/gtest.h>
#include <value/boolean.h>
#include <rtti/rtti.h>
#include <value/error.h>
#include <value/empty.h>

TEST(Pack, ZeroArgs) {
    MachineState s;
    Pack pk;
    ASSERT_EQ(Operation::Result::ERROR, pk.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(0, s.stack().size());
}

TEST(Pack, InsufficientPayload) {
    MachineState ms;
    Pack pk;
    ms.stack().push(Value::fromNumber(3));
    ASSERT_EQ(Operation::Result::ERROR, pk.execute(ms));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(ms.stack().pop())->value());
    ASSERT_EQ(1, ms.stack().size());
}

TEST(Pack, NotANumber) {
    MachineState ms;
    Pack pk;
    ms.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, pk.execute(ms));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(ms.stack().pop())->value());
    ASSERT_EQ(1, ms.stack().size());
}

TEST(Pack, EmptyResult) {
    MachineState ms;
    Pack pk;
    ms.stack().push(Value::fromNumber(0));
    ASSERT_EQ(Operation::Result::SUCCESS, pk.execute(ms));
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Tuple>());
    ASSERT_EQ(0, runtime_ptr_cast<Value_Tuple>(ms.stack().pop())->size());
}

TEST(Pack, Tuple) {
    MachineState ms;
    Pack pk;
    ms.stack().push(Value::fromBoolean(true));
    ms.stack().push(Value::empty());
    ms.stack().push(Value::fromNumber(2));
    ASSERT_EQ(Operation::Result::SUCCESS, pk.execute(ms));
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Tuple>());
    auto val = ms.stack().pop();
    auto tpl = runtime_ptr_cast<Value_Tuple>(val);
    ASSERT_NE(nullptr, tpl);
    ASSERT_EQ(2, tpl->size());
    ASSERT_EQ(0, ms.stack().size());
    ASSERT_TRUE(tpl->at(0)->isOfClass<Value_Empty>());
    ASSERT_TRUE(tpl->at(1)->isOfClass<Value_Boolean>());
}
