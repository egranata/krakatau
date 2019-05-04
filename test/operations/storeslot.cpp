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
#include <operations/storeslot.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <value/error.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <operations/op_loader.h>
#include <function/block.h>
#include <value/value_table.h>
#include <parser/parser.h>

TEST(Storeslot, StoreValue) {
    MachineState ms;
    auto blk = std::make_shared<Block>();
    blk->newSlot();
    ms.pushSlot(blk);
    auto slk = ms.currentSlot();
    ASSERT_EQ(0, slk->size());
    Storeslot ss("hello");
    ms.stack().push(Value::fromNumber(123));
    ss.execute(ms);
    ASSERT_EQ(1, slk->size());
    ASSERT_NE(nullptr, slk->find(Value::fromString("hello")));
    ASSERT_TRUE(Value::fromNumber(123)->equals(slk->find(Value::fromString("hello"))));
}

TEST(Storeslot, EmptyStack) {
    MachineState ms;
    auto blk = std::make_shared<Block>();
    blk->newSlot();
    ms.pushSlot(blk);
    auto slk = ms.currentSlot();
    ASSERT_EQ(0, slk->size());
    Storeslot ss("hello");
    ss.execute(ms);
    ASSERT_EQ(0, slk->size());
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Error>());
}

TEST(Storeslot, Describe) {
    Storeslot l("key");
    ASSERT_EQ("storeslot \"key\"", l.describe());
}

TEST(Storeslot, Serialize) {
    Storeslot st("key");
    Serializer s;
    st.serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Storeslot>());
    ASSERT_EQ("key", runtime_ptr_cast<Storeslot>(op)->key());
}

TEST(Storeslot, Parse) {
    Parser p("storeslot foo");
    auto opl = OperationLoader::loader();
    auto op = opl->fromParser(&p);
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Storeslot>());
    ASSERT_EQ("foo", runtime_ptr_cast<Storeslot>(op)->key());
}
