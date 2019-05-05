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
#include <operations/break.h>
#include <gtest/gtest.h>
#include <operations/push.h>
#include <value/value.h>
#include <function/block.h>
#include <rtti/rtti.h>
#include <parser/parser.h>
#include <value/block.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>

TEST(Break, Result) {
    MachineState ms;
    std::shared_ptr<Block> b(new Block());
    b->add(std::shared_ptr<Operation>(new Break()));
    b->add(std::shared_ptr<Operation>(new Push(Value::empty())));
    ASSERT_EQ(Operation::Result::SUCCESS, b->execute(ms));
    ASSERT_EQ(0, ms.stack().size());
}

TEST(Break, Parser) {
    MachineState ms;
    Parser p("value main block { break\n push empty }");
    ASSERT_EQ(1, ms.load(&p));
    auto blk = runtime_ptr_cast<Value_Block>(ms.value_store().retrieve("main"));
    ASSERT_NE(nullptr, blk);
    ASSERT_EQ(Operation::Result::SUCCESS, blk->value()->execute(ms));
    ASSERT_EQ(0, ms.stack().size());
}

TEST(Break, Serialize) {
    MachineState ms;
    Break b;
    Serializer sz;
    b.serialize(&sz);
    auto bs = ByteStream::anonymous(sz.data(), sz.size());
    auto op = OperationLoader::loader()->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_EQ(Operation::Result::EXIT_BLOCK, op->execute(ms));
    ASSERT_EQ(0, ms.stack().size());
}

TEST(Break, Clone) {
    auto brk = std::make_shared<Break>();
    ASSERT_NE(brk, nullptr);
    auto cln = brk->clone();
    ASSERT_NE(cln, nullptr);
    ASSERT_NE(cln, brk);
    ASSERT_TRUE(cln->equals(brk));
}
