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
#include <operation/push.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/number.h>
#include <rtti/rtti.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <operation/op_loader.h>
#include <value/boolean.h>

TEST(Push, Test) {
    MachineState s;
    Push p(Value::fromNumber(1234));
    p.execute(s);
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(1234, runtime_ptr_cast<Value_Number>(s.stack().peek())->value());
}

TEST(Push, Serialize) {
    Push p(Value::fromBoolean(false));
    Serializer s;
    p.serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Push>());
    ASSERT_TRUE(runtime_ptr_cast<Push>(op)->value()->isOfClass<Value_Boolean>());
}
