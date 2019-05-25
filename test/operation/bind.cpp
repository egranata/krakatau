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

#include <operation/bind.h>
#include <gtest/gtest.h>
#include <parser/parser.h>
#include <machine/state.h>
#include <rtti/rtti.h>
#include <operation/dup.h>
#include <value/block.h>
#include <value/operation.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <operation/op_loader.h>
#include <operation/block.h>
#include <value/number.h>
#include <value/boolean.h>

TEST(Bind, BindOfOperation) {
    auto op = std::make_shared<Dup>();
    auto bind = std::make_shared<PartialBind>(Value::fromNumber(12), op);
    MachineState ms;
    ASSERT_EQ(Operation::Result::SUCCESS, bind->execute(ms));
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(12)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(12)->equals(ms.stack().pop()));
}

TEST(Bind, BindOfBlock) {
    Parser p("block { push number 4 add }");
    auto blk = p.parseValuePayload();
    ASSERT_TRUE(blk->isOfClass<Value_Operation>());
    ASSERT_NE(nullptr, blk->asClass<Value_Operation>()->block());
    auto bind = std::make_shared<PartialBind>(Value::fromNumber(5), blk->asClass<Value_Operation>()->value());
    MachineState ms;
    ASSERT_EQ(Operation::Result::SUCCESS, bind->execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(9)->equals(ms.stack().pop()));
}

TEST(Bind, NestedBind) {
    Parser p("block { sub mul }");
    auto blk = p.parseValuePayload();
    ASSERT_TRUE(blk->isOfClass<Value_Operation>());
    ASSERT_TRUE(blk->asClass<Value_Operation>()->value()->isOfClass<Block>());
    auto bind1 = std::make_shared<PartialBind>(Value::fromNumber(5), blk->asClass<Value_Operation>()->value());
    auto bind2 = std::make_shared<PartialBind>(Value::fromNumber(3), bind1);
    auto bind3 = std::make_shared<PartialBind>(Value::fromNumber(8), bind2);
    MachineState ms;
    ASSERT_EQ(Operation::Result::SUCCESS, bind3->execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(16)->equals(ms.stack().pop()));
}

TEST(Bind, BindEquality) {
    Parser p1("block { sub mul }");
    auto blk1 = p1.parseValuePayload();
    Parser p2("operation dup");
    auto blk2 = p2.parseValuePayload();
    ASSERT_TRUE(blk2->isOfClass<Value_Operation>());
    auto bind_1false = std::make_shared<PartialBind>(Value::fromBoolean(false), blk1->asClass<Value_Operation>()->value());
    auto bind_2false = std::make_shared<PartialBind>(Value::fromBoolean(false), blk2->asClass<Value_Operation>()->value());
    auto bind_1true = std::make_shared<PartialBind>(Value::fromBoolean(true), blk1->asClass<Value_Operation>()->value());

    ASSERT_FALSE(bind_1false->equals(bind_2false));
    ASSERT_FALSE(bind_1false->equals(bind_1true));
    ASSERT_TRUE(bind_1false->equals(bind_1false));

    ASSERT_TRUE(bind_1false->equals(bind_1false->clone()));
    ASSERT_TRUE(bind_2false->equals(bind_2false->clone()));
    ASSERT_TRUE(bind_1true->equals(bind_1true->clone()));
}

TEST(Bind, NestedBindEquality) {
    Parser p1("block { sub mul }");
    auto blk1 = p1.parseValuePayload();
    Parser p2("block { mul dup }");
    auto blk2 = p2.parseValuePayload();

    auto bind_blk1 = std::make_shared<PartialBind>(Value::fromNumber(5), blk1->asClass<Value_Operation>()->value());
    auto bind_blk2 = std::make_shared<PartialBind>(Value::fromNumber(5), blk2->asClass<Value_Operation>()->value());

    auto bind_blk1_10 = std::make_shared<PartialBind>(Value::fromNumber(10), bind_blk1);
    auto bind_blk2_10 = std::make_shared<PartialBind>(Value::fromNumber(10), bind_blk2);

    auto bind_blk1_15 = std::make_shared<PartialBind>(Value::fromNumber(15), bind_blk1);

    ASSERT_FALSE(bind_blk1_10->equals(bind_blk2_10));
    ASSERT_TRUE(bind_blk1_10->equals(bind_blk1_10));
    ASSERT_TRUE(bind_blk1_10->equals(bind_blk1_10->clone()));

    ASSERT_FALSE(bind_blk1_15->equals(bind_blk2_10));
    ASSERT_FALSE(bind_blk1_15->equals(bind_blk1_10));
}

TEST(Bind, OperationSerialize) {
    auto bind = std::make_shared<PartialBind>(Value::fromNumber(5), std::make_shared<Dup>());
    Serializer s;
    bind->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto deser = OperationLoader::loader()->fromByteStream(bs.get());
    ASSERT_TRUE(bind->equals(deser));
}

TEST(Bind, BlockSerialize) {
    Parser p1("block { sub mul }");
    auto blk1 = p1.parseValuePayload();
    auto bind = std::make_shared<PartialBind>(Value::fromNumber(5), blk1->asClass<Value_Operation>()->value());
    Serializer s;
    bind->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto deser = OperationLoader::loader()->fromByteStream(bs.get());
    ASSERT_TRUE(bind->equals(deser));
}

TEST(Bind, NestedSerialize) {
    auto b0 = std::make_shared<PartialBind>(Value::fromNumber(5), std::make_shared<Dup>());
    auto b1 = std::make_shared<PartialBind>(Value::fromNumber(5), b0);
    auto bind = std::make_shared<PartialBind>(Value::fromBoolean(false), b1);
    Serializer s;
    bind->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto deser = OperationLoader::loader()->fromByteStream(bs.get());
    ASSERT_TRUE(bind->equals(deser));
}

TEST(Bind, ParseValue) {
    Parser p1("value foo bind number 12 operation exec");
    ASSERT_NE(nullptr, p1.parseValue());
}

TEST(Bind, FromTuple) {
    Parser p("value main block { push operation dup push number 5 push number 2 pack push type bind typecast }");
    MachineState ms;
    ms.load(&p);
    ms.execute();
    ASSERT_EQ(1, ms.stack().size());
    auto val = ms.stack().pop();
    ASSERT_NE(nullptr, val->asClass<Value_Operation>());
    auto bnd = val->asClass<Value_Operation>()->bind();
    ASSERT_NE(nullptr, bnd);
    ASSERT_TRUE(Value::fromNumber(5)->equals(bnd->value()));
    ASSERT_TRUE(bnd->callable()->isOfClass<Dup>());
    bnd->execute(ms);
    ASSERT_EQ(2, ms.stack().size());
}
