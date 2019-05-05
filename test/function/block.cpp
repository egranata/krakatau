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

#include <function/block.h>
#include <gtest/gtest.h>
#include <operations/nop.h>
#include <operations/push.h>
#include <operations/dup.h>
#include <value/number.h>
#include <value/error.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <vector>
#include <stream/serializer.h>
#include <machine/state.h>
#include <operations/arith.h>
#include <operations/loadslot.h>
#include <parser/parser.h>

TEST(Block, Size) {
    Block b;
    ASSERT_EQ(0, b.size());
    size_t size = 0;
    for (auto& op : b) {
        (void)op;
        ++size;
    }
    ASSERT_EQ(0, size);
    size = 0;
    b.add(std::make_shared<Nop>());
    ASSERT_EQ(1, b.size());
    for (auto& op : b) {
        (void)op;
        ++size;
    }
    ASSERT_EQ(1, size);
}

TEST(Block, Iterator) {
    auto op1 = std::make_shared<Nop>();
    auto op2 = std::make_shared<Nop>();
    Block b;
    b.add(op1);
    b.add(op2);
    ASSERT_EQ(op1.get(), b.begin()->get());
    ASSERT_EQ(op2.get(), (++b.begin())->get());
}

TEST(Block, Execute) {
    std::shared_ptr<Block> b(new Block());
    b->add(std::make_shared<Push>(Value::fromNumber(12)));
    b->add(std::make_shared<Dup>());
    MachineState s;
    ASSERT_EQ(Operation::Result::SUCCESS, b->execute(s));
    ASSERT_EQ(2, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());;
    ASSERT_EQ(12, runtime_ptr_cast<Value_Number>(s.stack().peek())->value());
    s.stack().pop();
    ASSERT_EQ(1, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());;
    ASSERT_EQ(12, runtime_ptr_cast<Value_Number>(s.stack().peek())->value());
}

TEST(Block, ExecuteFail) {
    std::shared_ptr<Block> b(new Block());
    b->add(std::make_shared<Dup>());
    MachineState s;
    ASSERT_EQ(Operation::Result::ERROR, b->execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().peek())->value());
}

TEST(Block, Print) {
    Block b;
    b.add(std::make_shared<Push>(Value::fromNumber(12)));
    b.add(std::make_shared<Dup>());
    ASSERT_EQ("block {\n push 12\n dup\n}", b.describe());
}

TEST(Block, PrintWithSlots) {
    Block b;
    b.addSlotValue("$slot");
    b.add(std::make_shared<Push>(Value::fromNumber(12)));
    b.add(std::make_shared<Dup>());
    ASSERT_EQ("block slots $slot {\n push 12\n dup\n}", b.describe());
}

TEST(Block, FromByteStream) {
    std::vector<uint8_t> i = {  0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 2,
                                0, 0, 0, 14,
                                0, 0, 0, 23 };
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto blk = Block::fromByteStream(bs.get());
    ASSERT_NE(nullptr, blk);
    ASSERT_EQ(2, blk->size());
    ASSERT_TRUE(blk->at(0)->isOfClass<Dup>());
    ASSERT_TRUE(blk->at(1)->isOfClass<Nop>());
}

TEST(Block, Serialize) {
    Serializer s;
    Block blk;
    blk.add(std::make_shared<Dup>());
    blk.add(std::make_shared<Nop>());
    blk.serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto db = Block::fromByteStream(bs.get());
    ASSERT_NE(db, nullptr);
    ASSERT_EQ(blk.size(), db->size());
    ASSERT_TRUE(db->at(0)->isOfClass<Dup>());
    ASSERT_TRUE(db->at(1)->isOfClass<Nop>());
}

TEST(Block, SerializeWithSlots) {
    Serializer s;
    Block blk;
    blk.addSlotValue("$a");
    blk.add(std::make_shared<Dup>());
    blk.add(std::make_shared<Nop>());
    blk.serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto db = Block::fromByteStream(bs.get());
    ASSERT_NE(db, nullptr);
    ASSERT_EQ(blk.size(), db->size());
    ASSERT_TRUE(db->at(0)->isOfClass<Dup>());
    ASSERT_TRUE(db->at(1)->isOfClass<Nop>());
    ASSERT_EQ(1, db->numSlotValues());
    ASSERT_EQ("$a", db->slotValueAt(0).value_or(""));
}

TEST(Block, AsOperation) {
    std::shared_ptr<Operation> blk(new Block());
    ASSERT_TRUE(blk->isOfClass<Block>());
    ASSERT_NE(nullptr, runtime_ptr_cast<Block>(blk));
}

TEST(Block, EqualityTrue) {
    Block blk;
    blk.add(std::make_shared<Dup>());
    blk.add(std::make_shared<Nop>());
    std::shared_ptr<Operation> b2(new Block());
    runtime_ptr_cast<Block>(b2)->add(std::make_shared<Dup>());
    runtime_ptr_cast<Block>(b2)->add(std::make_shared<Nop>());
    ASSERT_TRUE(blk.equals(b2));
}

TEST(Block, EqualityFalseSize) {
    Block blk;
    blk.add(std::make_shared<Dup>());
    blk.add(std::make_shared<Nop>());
    std::shared_ptr<Operation> b2(new Block());
    runtime_ptr_cast<Block>(b2)->add(std::make_shared<Dup>());
    ASSERT_FALSE(blk.equals(b2));
}

TEST(Block, EqualityFalseContent) {
    Block blk;
    blk.add(std::make_shared<Nop>());
    std::shared_ptr<Operation> b2(new Block());
    runtime_ptr_cast<Block>(b2)->add(std::make_shared<Dup>());
    ASSERT_FALSE(blk.equals(b2));
}

TEST(Block, EqualityFalseNumSlots) {
    auto b1 = std::make_shared<Block>();
    auto b2 = std::make_shared<Block>();
    b1->addSlotValue("$a");
    b2->addSlotValue("$b");
    b2->addSlotValue("$c");
    ASSERT_FALSE(b1->equals(b2));
}

TEST(Block, EqualityFalseSlotNames) {
    auto b1 = std::make_shared<Block>();
    auto b2 = std::make_shared<Block>();
    b1->addSlotValue("$a");
    b2->addSlotValue("$c");
    ASSERT_FALSE(b1->equals(b2));
}

TEST(Block, EqualityTrueSlots) {
    auto b1 = std::make_shared<Block>();
    auto b2 = std::make_shared<Block>();
    b1->addSlotValue("$a");
    b2->addSlotValue("$a");
    ASSERT_TRUE(b1->equals(b2));
}

TEST(Block, ValidSlots) {
    MachineState ms;
    auto blk = std::make_shared<Block>();
    blk->addSlotValue("$a");
    blk->addSlotValue("$b");
    ms.stack().push(Value::fromNumber(12));
    ms.stack().push(Value::fromNumber(24));
    blk->add(std::make_shared<Loadslot>("$a"));
    blk->add(std::make_shared<Loadslot>("$b"));
    blk->add(std::make_shared<Add_Binary_Arithmetic_Operation>());
    blk->execute(ms);
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(36)->equals(ms.stack().peek()));
}

TEST(Block, InsufficientSlots) {
    MachineState ms;
    auto blk = std::make_shared<Block>();
    blk->addSlotValue("$a");
    blk->addSlotValue("$b");
    ms.stack().push(Value::fromNumber(12));
    blk->add(std::make_shared<Loadslot>("$a"));
    blk->add(std::make_shared<Loadslot>("$b"));
    blk->add(std::make_shared<Add_Binary_Arithmetic_Operation>());
    blk->execute(ms);
    ASSERT_EQ(3, ms.stack().size());
    ASSERT_TRUE(Value::error(ErrorCode::NOT_FOUND)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(12)->equals(ms.stack().pop()));
}

TEST(Block, SlotsAreReentrant) {
    Parser p("value foo block slots $a { loadslot $a push number 0 eq iftrue break push number 1 loadslot $a sub dup load foo exec }"
             "\n"
             "value main block { push number 10 load foo exec }");
    MachineState ms;
    ASSERT_EQ(2, ms.load(&p));
    ms.execute();
    ASSERT_EQ(10, ms.stack().size());
    for(size_t i = 0; i < 10; ++i) {
        ASSERT_TRUE(Value::fromNumber(i)->equals(ms.stack().pop()));
    }
}
