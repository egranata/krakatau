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
#include <operations/store.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/number.h>
#include <stack/stack.h>
#include <rtti/rtti.h>
#include <value/error.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <operations/op_loader.h>

TEST(Store, NewValue) {
    MachineState ms;
    Store s("key");
    ms.stack().push(Value::fromNumber(1234));
    ASSERT_EQ(Operation::Result::SUCCESS, s.execute(ms));
    ASSERT_EQ(0, ms.stack().size());
    ASSERT_NE(nullptr, ms.value_store().retrieve("key"));
    ASSERT_TRUE(ms.value_store().retrieve("key")->isOfClass<Value_Number>());
    ASSERT_EQ(1234, runtime_ptr_cast<Value_Number>(ms.value_store().retrieve("key"))->value());
}

TEST(Store, NoOverwrite) {
    MachineState ms;
    ms.value_store().store("key", Value::fromNumber(1234));
    Store s("key");
    ms.stack().push(Value::fromNumber(4321));
    ASSERT_EQ(Operation::Result::ERROR, s.execute(ms));
    ASSERT_TRUE(ms.value_store().retrieve("key")->isOfClass<Value_Number>());
    ASSERT_EQ(1234, runtime_ptr_cast<Value_Number>(ms.value_store().retrieve("key"))->value());
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Error>());
    ms.stack().pop();
    ASSERT_EQ(4321, runtime_ptr_cast<Value_Number>(ms.stack().peek())->value());
}

TEST(Store, Describe) {
    Store l("key");
    ASSERT_EQ("store \"key\"", l.describe());
}

TEST(Store, Serialize) {
    Store st("key");
    Serializer s;
    st.serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Store>());
    ASSERT_EQ("key", runtime_ptr_cast<Store>(op)->key());
}
