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
#include <value/empty.h>

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
        Value::fromNumber(1), Value::fromNumber(2),
        Value::fromNumber(3), Value::fromNumber(4),
        Value::fromNumber(5)
    });
    std::shared_ptr<Call> c = std::make_shared<Call>("foo", tpl);

    ASSERT_EQ(Operation::Result::SUCCESS, c->execute(ms));
    ASSERT_EQ(5, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(1)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(2)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(3)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(4)->equals(ms.stack().pop()));
    ASSERT_TRUE(Value::fromNumber(5)->equals(ms.stack().pop()));
}

TEST(Call, RunsBlock) {
    MachineState ms;
    Parser p("value foo block { swap sub }");
    ms.load(&p);
    std::shared_ptr<Value_Tuple> tpl = Value::tuple({
        Value::fromNumber(12), Value::fromNumber(24)
    });
    std::shared_ptr<Call> c = std::make_shared<Call>("foo", tpl);

    ASSERT_EQ(Operation::Result::SUCCESS, c->execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(12)->equals(ms.stack().peek()));
}

TEST(Call, SlotsOlder) {
    MachineState ms;
    Parser p("value foo block slots $a,$b { loadslot $a } value bar block slots $a,$b { loadslot $b }");
    ms.load(&p);
    std::shared_ptr<Value_Tuple> tpl = Value::tuple({
        Value::fromNumber(12), Value::fromNumber(24)
    });
    std::shared_ptr<Call> c_foo = std::make_shared<Call>("foo", tpl);
    std::shared_ptr<Call> c_bar = std::make_shared<Call>("bar", tpl);

    ASSERT_EQ(Operation::Result::SUCCESS, c_foo->execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(12)->equals(ms.stack().pop()));

    ASSERT_EQ(Operation::Result::SUCCESS, c_bar->execute(ms));
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(Value::fromNumber(24)->equals(ms.stack().pop()));
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
