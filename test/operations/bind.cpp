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
#include <operations/bind.h>
#include <value/value.h>
#include <value/bind.h>
#include <gtest/gtest.h>
#include <operations/dup.h>
#include <rtti/rtti.h>
#include <value/error.h>
#include <value/string.h>
#include <value/table.h>
#include <value/empty.h>
#include <parser/parser.h>
#include <operations/exec.h>

TEST(Bind, ZeroArgs) {
    MachineState s;
    Bind b;
    ASSERT_EQ(Operation::Result::ERROR, b.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(0, s.stack().size());
}

TEST(Bind, OneArg) {
    MachineState s;
    Bind b;
    s.stack().push(Value::tuple());
    ASSERT_EQ(Operation::Result::ERROR, b.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(1, s.stack().size());
}

TEST(Bind, MistypedArgs) {
    MachineState s;
    Bind b;
    s.stack().push(Value::tuple());
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, b.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
}

TEST(Bind, Rtti) {
    Bind b;
    ASSERT_TRUE(b.isOfClass<Bind>());
}

TEST(Bind, Valid) {
    MachineState s;
    s.stack().push(Value::fromOperation(std::make_shared<Dup>()));
    s.stack().push(Value::fromBoolean(true));
    Bind b;
    ASSERT_EQ(Operation::Result::SUCCESS, b.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Bind>());
    Exec e;
    ASSERT_EQ(Operation::Result::SUCCESS, e.execute(s));
    ASSERT_EQ(2, s.stack().size());
    ASSERT_TRUE(s.stack().pop()->equals(Value::fromBoolean(true)));
    ASSERT_TRUE(s.stack().pop()->equals(Value::fromBoolean(true)));
}

TEST(Bind, ParseOperation) {
    Parser p("value foo block slots $a,$b { loadslot $b loadslot $a sub } value main block { load foo push number 4 bind push number 3 swap exec }");
    MachineState ms;
    ASSERT_EQ(2, ms.load(&p));
    ASSERT_EQ(Operation::Result::SUCCESS, ms.execute());
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(1)->equals(ms.stack().peek()));
}
