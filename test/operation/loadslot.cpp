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
#include <operation/loadslot.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <value/error.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <operation/op_loader.h>
#include <operation/block.h>
#include <value/value_table.h>
#include <value/number.h>
#include <parser/parser.h>
#include <value/string.h>

TEST(Loadslot, PresentValue) {
    MachineState ms;
    auto blk = std::make_shared<Block>();
    blk->newSlot();
    ms.pushSlot(blk);
    auto slk = ms.currentSlot();
    slk->add(Value::fromString("hello"), Value::fromNumber(123));
    ASSERT_EQ(1, slk->size());
    Loadslot ls("hello");
    ls.execute(ms);
    ASSERT_EQ(1, slk->size());
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(123)->equals(ms.stack().pop()));
}

TEST(Loadslot, MissingValue) {
    MachineState ms;
    auto blk = std::make_shared<Block>();
    blk->newSlot();
    ms.pushSlot(blk);
    auto slk = ms.currentSlot();
    Loadslot ls("hello");
    ls.execute(ms);
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::error(ErrorCode::NOT_FOUND)->equals(ms.stack().pop()));
}

TEST(Loadslot, Describe) {
    Loadslot l("key");
    ASSERT_EQ("loadslot \"key\"", l.describe());
}

TEST(Loadslot, Serialize) {
    Loadslot st("key");
    Serializer s;
    st.serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Loadslot>());
    ASSERT_EQ("key", runtime_ptr_cast<Loadslot>(op)->key());
}

TEST(Loadslot, Parse) {
    Parser p("loadslot foo");
    auto opl = OperationLoader::loader();
    auto op = opl->fromParser(&p);
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Loadslot>());
    ASSERT_EQ("foo", runtime_ptr_cast<Loadslot>(op)->key());
}
