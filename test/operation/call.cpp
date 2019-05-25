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
#include <operation/call.h>
#include <gtest/gtest.h>
#include <parser/parser.h>
#include <value/operation.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <value/tuple.h>
#include <value/value.h>
#include <value/error.h>
#include <value/boolean.h>
#include <value/number.h>

TEST(Call, NoSuchValue) {
    MachineState ms;
    std::shared_ptr<Value_Tuple> tpl = Value::tuple({
        Value::fromBoolean(false), Value::fromBoolean(false)
    });
    std::shared_ptr<Call> c = std::make_shared<Call>("no", tpl);

    ASSERT_EQ(Operation::Result::ERROR, c->execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Error>());
    ASSERT_EQ(ErrorCode::NOT_FOUND, ms.stack().peek()->asClass<Value_Error>()->value());
}

TEST(Call, NotCallable) {
    MachineState ms;
    ms.value_store().store("key", Value::fromBoolean(false));
    std::shared_ptr<Value_Tuple> tpl = Value::tuple({
        Value::fromBoolean(false), Value::fromBoolean(false)
    });
    std::shared_ptr<Call> c = std::make_shared<Call>("key", tpl);

    ASSERT_EQ(Operation::Result::ERROR, c->execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Error>());
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, ms.stack().peek()->asClass<Value_Error>()->value());
}

TEST(Call, PushesValues) {
    MachineState ms;
    Parser p("value foo block { nop }");
    ms.load(&p);
    std::shared_ptr<Value_Tuple> tpl = Value::tuple({
        Value::fromNumber(12), Value::fromNumber(24)
    });
    std::shared_ptr<Call> c = std::make_shared<Call>("foo", tpl);

    ASSERT_EQ(Operation::Result::SUCCESS, c->execute(ms));
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(24)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(12)->equals(ms.stack().pop()));
}

TEST(Call, RunsBlock) {
    MachineState ms;
    Parser p("value foo block { mul }");
    ms.load(&p);
    std::shared_ptr<Value_Tuple> tpl = Value::tuple({
        Value::fromNumber(12), Value::fromNumber(24)
    });
    std::shared_ptr<Call> c = std::make_shared<Call>("foo", tpl);

    ASSERT_EQ(Operation::Result::SUCCESS, c->execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(288)->equals(ms.stack().peek()));
}

TEST(Call, Equality) {
    std::shared_ptr<Value_Tuple> tpl1 = Value::tuple({
        Value::fromNumber(12), Value::fromNumber(24)
    });

    std::shared_ptr<Value_Tuple> tpl2 = Value::tuple({
        Value::fromNumber(12), Value::empty()
    });

    std::shared_ptr<Call> c_foo_1 = std::make_shared<Call>("foo", tpl1);
    std::shared_ptr<Call> c_foo_2 = std::make_shared<Call>("foo", tpl2);

    ASSERT_FALSE(c_foo_1->equals(c_foo_2));

    ASSERT_TRUE(c_foo_1->equals(c_foo_1));
    ASSERT_TRUE(c_foo_2->equals(c_foo_2));

    std::shared_ptr<Call> c_bar_1 = std::make_shared<Call>("bar", tpl1);
    std::shared_ptr<Call> c_bar_2 = std::make_shared<Call>("bar", tpl2);

    ASSERT_FALSE(c_foo_1->equals(c_bar_1));
    ASSERT_FALSE(c_foo_1->equals(c_bar_2));

    ASSERT_TRUE(c_foo_1->equals(c_foo_1->clone()));
    ASSERT_TRUE(c_foo_2->equals(c_foo_2->clone()));
}

TEST(Call, Parse) {
    Parser p("operation call foo (number 1, boolean false, empty,)");
    auto vl = p.parseValuePayload();
    ASSERT_NE(nullptr, vl);
    ASSERT_TRUE(vl->isOfClass<Value_Operation>());
    auto op = vl->asClass<Value_Operation>()->value();
    ASSERT_TRUE(op->isOfClass<Call>());
    auto cl = op->asClass<Call>();
    ASSERT_EQ("foo", cl->name());
    ASSERT_EQ(3, cl->arguments()->size());
    ASSERT_TRUE(Value::fromNumber(1)->equals(cl->arguments()->at(0)));
    ASSERT_TRUE(Value::fromBoolean(false)->equals(cl->arguments()->at(1)));
    ASSERT_TRUE(Value::empty()->equals(cl->arguments()->at(2)));
}

TEST(Call, Serialize) {
    std::shared_ptr<Value_Tuple> tpl = Value::tuple({
        Value::fromNumber(12), Value::fromNumber(24)
    });
    std::shared_ptr<Call> c = std::make_shared<Call>("foo", tpl);

    Serializer s;
    c->serialize(&s);

    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Call>());
    auto cl = op->asClass<Call>();

    ASSERT_EQ("foo", cl->name());
    ASSERT_EQ(2, cl->arguments()->size());
    ASSERT_TRUE(Value::fromNumber(12)->equals(cl->arguments()->at(0)));
    ASSERT_TRUE(Value::fromNumber(24)->equals(cl->arguments()->at(1)));
}
