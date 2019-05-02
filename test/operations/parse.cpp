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
#include <operations/parse.h>
#include <value/value.h>
#include <value/tuple.h>
#include <value/number.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <value/error.h>

TEST(Parse, ZeroArgs) {
    MachineState s;
    Parse pk;
    ASSERT_EQ(Operation::Result::ERROR, pk.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(0, s.stack().size());
}

TEST(Pack, NotAString) {
    MachineState ms;
    Parse pk;
    ms.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, pk.execute(ms));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(ms.stack().pop())->value());
    ASSERT_EQ(1, ms.stack().size());
}

TEST(Parse, InvalidInput) {
    MachineState ms;
    Parse pk;
    ms.stack().push(Value::fromString("number"));
    ASSERT_EQ(Operation::Result::ERROR, pk.execute(ms));
    ASSERT_EQ(ErrorCode::SYNTAX_ERROR, runtime_ptr_cast<Value_Error>(ms.stack().pop())->value());
    ASSERT_EQ(1, ms.stack().size());
}

TEST(Parse, ValidSyntax) {
    MachineState ms;
    Parse pk;
    ms.stack().push(Value::fromString("number 123456"));
    ASSERT_EQ(Operation::Result::SUCCESS, pk.execute(ms));
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Number>());
}
