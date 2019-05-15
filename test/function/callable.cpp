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

#include <function/callable.h>
#include <gtest/gtest.h>
#include <operations/dup.h>
#include <parser/parser.h>
#include <machine/state.h>
#include <value/value.h>
#include <rtti/rtti.h>
#include <value/block.h>
#include <function/bind.h>

TEST(Callable, Empty) {
    MachineState ms;

    Callable c1(std::shared_ptr<Operation>(nullptr));
    ASSERT_FALSE(c1);
    Callable c2(std::shared_ptr<Block>(nullptr));
    ASSERT_FALSE(c2);
    Callable c3(std::shared_ptr<Value>(nullptr));
    ASSERT_FALSE(c3);

    ASSERT_EQ(Operation::Result::ERROR, c1.execute(ms));
    ASSERT_EQ(Operation::Result::ERROR, c2.execute(ms));
    ASSERT_EQ(Operation::Result::ERROR, c3.execute(ms));

    ASSERT_EQ(nullptr, c1.operation());
    ASSERT_EQ(nullptr, c1.block());
    ASSERT_EQ(nullptr, c1.bind());
}

TEST(Callable, ValidOp) {
    MachineState ms;
    auto op = std::shared_ptr<Operation>(new Dup());
    ms.stack().push(Value::fromNumber(123));
    Callable clb(op);
    ASSERT_TRUE(clb);
    ASSERT_EQ(Operation::Result::SUCCESS, clb.execute(ms));
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(ms.stack().pop()->equals(ms.stack().peek()));

    ASSERT_NE(clb.operation(), nullptr);
    ASSERT_EQ(nullptr, clb.block());
    ASSERT_EQ(nullptr, clb.bind());
}

TEST(Callable, ValidBlock) {
    MachineState ms;
    auto vblk = Parser("block { push number 0 }").parseValuePayload();
    auto blk = runtime_ptr_cast<Value_Block>(vblk)->value();
    Callable clb(blk);
    ASSERT_TRUE(clb);
    ASSERT_EQ(Operation::Result::SUCCESS, clb.execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().pop()->equals(Value::fromNumber(0)));

    ASSERT_EQ(clb.operation(), nullptr);
    ASSERT_NE(nullptr, clb.block());
    ASSERT_EQ(nullptr, clb.bind());
}

TEST(Callable, BlockValue) {
    MachineState ms;
    auto vblk = Parser("block { push number 0 }").parseValuePayload();
    Callable clb(vblk);
    ASSERT_TRUE(clb);
    ASSERT_EQ(Operation::Result::SUCCESS, clb.execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().pop()->equals(Value::fromNumber(0)));

    ASSERT_EQ(clb.operation(), nullptr);
    ASSERT_NE(nullptr, clb.block());
    ASSERT_EQ(nullptr, clb.bind());
}

TEST(Callable, OpValue) {
    MachineState ms;
    auto vblk = Parser("operation push number 0").parseValuePayload();
    Callable clb(vblk);
    ASSERT_TRUE(clb);
    ASSERT_EQ(Operation::Result::SUCCESS, clb.execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().pop()->equals(Value::fromNumber(0)));

    ASSERT_NE(clb.operation(), nullptr);
    ASSERT_EQ(nullptr, clb.block());
    ASSERT_EQ(nullptr, clb.bind());
}

TEST(Callable, InvalidValue) {
    MachineState ms;
    auto vblk = Parser("number 0").parseValuePayload();
    Callable clb(vblk);
    ASSERT_FALSE(clb);
    ASSERT_EQ(Operation::Result::ERROR, clb.execute(ms));
}

TEST(Callable, Bind) {
    MachineState ms;
    auto bind = Value::fromBind(std::make_shared<PartialBind>(Value::fromNumber(1), Callable(std::make_shared<Dup>())));
    Callable clb(bind);
    ASSERT_TRUE(clb);
    ASSERT_EQ(Operation::Result::SUCCESS, clb.execute(ms));
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(ms.stack().pop()->equals(Value::fromNumber(1)));
    ASSERT_TRUE(ms.stack().pop()->equals(Value::fromNumber(1)));

    ASSERT_EQ(clb.operation(), nullptr);
    ASSERT_EQ(nullptr, clb.block());
    ASSERT_NE(nullptr, clb.bind());
}

TEST(Callable, Describe) {
    Callable c1(Value::fromOperation(std::make_shared<Dup>()));
    Callable c2(Value::fromBlock(std::make_shared<Block>()));
    Callable c3(Value::fromBind(std::make_shared<PartialBind>(Value::empty(), Callable(std::make_shared<Dup>()))));

    ASSERT_EQ("operation dup", c1.describe());
    ASSERT_EQ("block {\n}", c2.describe());
    ASSERT_EQ("bind(empty, operation dup)", c3.describe());
}
