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
#include <operation/push.h>
#include <operation/exec.h>
#include <operation/dup.h>
#include <value/value.h>
#include <operation/block.h>
#include <gtest/gtest.h>
#include <value/number.h>
#include <value/operation.h>
#include <value/error.h>
#include <rtti/rtti.h>
#include <value/bind.h>
#include <operation/bind.h>

TEST(Exec, Block) {
    MachineState s;
    auto blk = std::make_shared<Block>();
    blk->add(std::make_shared<Push>(Value::fromNumber(12)));
    blk->add(std::make_shared<Dup>());
    s.stack().push(Value::fromBlock(blk));
    Exec e;
    ASSERT_EQ(Operation::Result::SUCCESS, e.execute(s));
    ASSERT_EQ(2, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());
}

TEST(Exec, Operation) {
    MachineState s;
    auto oper = std::shared_ptr<Operation>(new Push(Value::fromNumber(12)));
    s.stack().push(Value::fromOperation(oper));
    Exec e;
    ASSERT_EQ(Operation::Result::SUCCESS, e.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());
}

TEST(Exec, NotBlock) {
    MachineState s;
    Exec e;
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, e.execute(s));
    ASSERT_EQ(2, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Error>());
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().peek())->value());
}

TEST(Exec, Bind) {
    MachineState s;
    Exec e;
    s.stack().push(Value::fromBind(std::make_shared<PartialBind>(Value::fromNumber(123), std::make_shared<Dup>())));
    ASSERT_EQ(Operation::Result::SUCCESS, e.execute(s));
    ASSERT_EQ(2, s.stack().size());
    ASSERT_TRUE(Value::fromNumber(123)->equals(s.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(123)->equals(s.stack().pop()));
}
