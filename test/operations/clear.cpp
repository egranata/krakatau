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
#include <operations/clear.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/number.h>
#include <stack/stack.h>
#include <rtti/rtti.h>
#include <value/error.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <operations/op_loader.h>

TEST(Clear, PresentValue) {
    MachineState ms;
    Clear c("key");
    ms.value_store().store("key", Value::fromNumber(1234));
    ASSERT_EQ(Operation::Result::SUCCESS, c.execute(ms));
    ASSERT_EQ(nullptr, ms.value_store().retrieve("key"));
}

TEST(Clear, AbsentValue) {
    MachineState ms;
    Clear c("key");
    ASSERT_EQ(Operation::Result::ERROR, c.execute(ms));
    ASSERT_EQ(nullptr, ms.value_store().retrieve("key"));
}

TEST(Clear, Describe) {
    Clear l("key");
    ASSERT_EQ("clear \"key\"", l.describe());
}

TEST(Clear, Serialize) {
    Clear l("key");
    Serializer s;
    l.serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Clear>());
    ASSERT_EQ("key", runtime_ptr_cast<Clear>(op)->key());
}
