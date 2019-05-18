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
#include <operation/halt.h>
#include <gtest/gtest.h>
#include <operation/push.h>
#include <value/value.h>
#include <operation/block.h>
#include <rtti/rtti.h>
#include <parser/parser.h>
#include <value/value_store.h>
#include <value/block.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>

TEST(Halt, Operation) {
    MachineState ms;
    Parser p("value main block { load foo exec push number 12 } value foo block { halt push number 24 }");
    ASSERT_EQ(2, ms.load(&p));
    ASSERT_EQ(Operation::Result::HALT, ms.execute());
    ASSERT_EQ(0, ms.stack().size());
}

TEST(Halt, Serialize) {
    MachineState ms;
    Halt op;
    Serializer sz;
    op.serialize(&sz);
    auto bs = ByteStream::anonymous(sz.data(), sz.size());
    auto ds = OperationLoader::loader()->fromByteStream(bs.get());
    ASSERT_NE(nullptr, ds);
    ASSERT_EQ(Operation::Result::HALT, ds->execute(ms));
    ASSERT_EQ(0, ms.stack().size());
}
