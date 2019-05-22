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
#include <operation/select.h>
#include <value/value.h>
#include <value/table.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <value/number.h>
#include <value/operation.h>
#include <value/block.h>
#include <parser/parser.h>
#include <operation/dup.h>
#include <operation/swap.h>
#include <operation/nop.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <operation/op_loader.h>

TEST(Select, Match) {
    MachineState ms;
    Select s(Value::table( {
        {Value::fromNumber(12), Value::fromOperation(std::make_shared<Dup>())},
        {Value::fromNumber(24), Value::fromOperation(std::make_shared<Swap>())},
    } ));
    ms.stack().push(Value::fromNumber(24));
    ms.stack().push(Value::fromNumber(12));
    ASSERT_EQ(Operation::Result::SUCCESS, s.execute(ms));
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(24)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(24)->equals(ms.stack().pop()));
}

TEST(Select, DefaultMismatch) {
    MachineState ms;
    Parser p("block { push number 34 dup mul }");

    Select s(Value::table( {
        {Value::fromNumber(12), Value::fromOperation(std::make_shared<Dup>())},
        {Value::fromNumber(24), Value::fromOperation(std::make_shared<Swap>())},
    }), std::dynamic_pointer_cast<Value_Operation>(Value::fromParser(&p)));
    ms.stack().push(Value::fromNumber(36));
    ASSERT_EQ(Operation::Result::SUCCESS, s.execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(1156)->equals(ms.stack().pop()));
}

TEST(Select, NoDefaultMismatch) {
    MachineState ms;

    Select s(Value::table( {
        {Value::fromNumber(12), Value::fromOperation(std::make_shared<Dup>())},
        {Value::fromNumber(24), Value::fromOperation(std::make_shared<Swap>())},
    }));
    ms.stack().push(Value::fromNumber(36));
    ASSERT_EQ(Operation::Result::ERROR, s.execute(ms));
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(Value::error(ErrorCode::NOT_FOUND)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(36)->equals(ms.stack().pop()));
}

TEST(Select, Describe) {
    Select s1(Value::table({
        {Value::fromNumber(12), Value::fromOperation(std::make_shared<Dup>())},
    }));
    Select s2(Value::table({
        {Value::fromNumber(12), Value::fromOperation(std::make_shared<Dup>())},
    }), std::dynamic_pointer_cast<Value_Operation>(Value::fromOperation(std::make_shared<Swap>())));

    ASSERT_EQ("select [12 -> dup]", s1.describe());
    ASSERT_EQ("select [12 -> dup] else swap", s2.describe());
}

TEST(Select, Serialize) {
    Select s1(Value::table({
        {Value::fromNumber(12), Value::fromOperation(std::make_shared<Dup>())},
    }));
    Select s2(Value::table({
        {Value::fromNumber(12), Value::fromOperation(std::make_shared<Dup>())},
    }), std::dynamic_pointer_cast<Value_Operation>(Value::fromOperation(std::make_shared<Swap>())));

    Serializer ser;
    s1.serialize(&ser);
    s2.serialize(&ser);

    auto bs = ByteStream::anonymous(ser.data(), ser.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_TRUE(s1.equals(op));
    op = opl->fromByteStream(bs.get());
    ASSERT_TRUE(s2.equals(op));
}

TEST(Select, Parse) {
    Select s1(Value::table({
        {Value::fromNumber(12), Value::fromOperation(std::make_shared<Dup>())},
    }));
    Select s2(Value::table({
        {Value::fromNumber(12), Value::fromOperation(std::make_shared<Dup>())},
    }), std::dynamic_pointer_cast<Value_Operation>(Value::fromOperation(std::make_shared<Swap>())));

    Parser p1("select table [number 12 -> operation dup]");
    Parser p2("select table [number 12 -> operation dup] else operation swap");

    auto opl = OperationLoader::loader();

    auto dp1 = opl->fromParser(&p1);
    ASSERT_NE(dp1, nullptr);
    ASSERT_TRUE(s1.equals(dp1));

    auto dp2 = opl->fromParser(&p2);
    ASSERT_NE(dp2, nullptr);
    ASSERT_TRUE(s2.equals(dp2));
}
