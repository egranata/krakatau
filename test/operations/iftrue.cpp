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
#include <value/value.h>
#include <operations/block.h>
#include <gtest/gtest.h>
#include <value/number.h>
#include <value/error.h>
#include <rtti/rtti.h>
#include <parser/parser.h>
#include <value/block.h>
#include <operations/push.h>
#include <operations/iftrue.h>

TEST(IfTrue, Taken) {
    MachineState ms;
    ms.stack().push(Value::fromBoolean(true));
    IfTrue it(std::shared_ptr<Operation>(new Push(Value::fromNumber(5))));
    it.execute(ms);
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(5, runtime_ptr_cast<Value_Number>(ms.stack().peek())->value());
}

TEST(IfTrue, NotTaken) {
    MachineState ms;
    ms.stack().push(Value::fromBoolean(false));
    IfTrue it(std::shared_ptr<Operation>(new Push(Value::fromNumber(5))));
    it.execute(ms);
    ASSERT_EQ(0, ms.stack().size());
}

TEST(IfTrue, ParserTaken) {
    MachineState ms;
    Parser p("value main block { push number 5, push boolean true, iftrue dup }");
    ASSERT_EQ(1, ms.load(&p));
    ASSERT_EQ(Operation::Result::SUCCESS, ms.execute());
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_EQ(ms.stack().pop(), ms.stack().pop());
}

TEST(IfTrue, ParserNotTaken) {
    MachineState ms;
    Parser p("value main block { push number 5, push boolean false, iftrue dup }");
    ASSERT_EQ(1, ms.load(&p));
    ASSERT_EQ(Operation::Result::SUCCESS, ms.execute());
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().pop()->isOfClass<Value_Number>());
}

TEST(IfTrue, Serialize) {
    MachineState ms;
    ms.stack().push(Value::fromBoolean(true));
    IfTrue it(std::shared_ptr<Operation>(new Push(Value::fromNumber(5))));
    Serializer sz;
    it.serialize(&sz);
    auto bs = ByteStream::anonymous(sz.data(), sz.size());
    auto op = OperationLoader::loader()->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    op->execute(ms);
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(5, runtime_ptr_cast<Value_Number>(ms.stack().peek())->value());
}
