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
#include <operations/loop.h>
#include <gtest/gtest.h>
#include <operations/push.h>
#include <value/value.h>
#include <function/block.h>
#include <rtti/rtti.h>
#include <parser/parser.h>
#include <value/block.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <value/boolean.h>

TEST(Loop, Parser) {
    MachineState ms;
    Parser p("value main block { push boolean false load count exec } value count block { not dup iftrue loop }");
    ASSERT_EQ(2, ms.load(&p));
    auto blk = runtime_ptr_cast<Value_Block>(ms.value_store().retrieve("main"));
    ASSERT_NE(nullptr, blk);
    ASSERT_EQ(Operation::Result::SUCCESS, blk->execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Boolean>());
    ASSERT_FALSE(runtime_ptr_cast<Value_Boolean>(ms.stack().peek())->value());
}

TEST(Loop, Serialize) {
    MachineState ms;
    Loop b;
    Serializer sz;
    b.serialize(&sz);
    auto bs = ByteStream::anonymous(sz.data(), sz.size());
    auto op = OperationLoader::loader()->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_EQ(Operation::Result::RESTART_BLOCK, op->execute(ms));
}
