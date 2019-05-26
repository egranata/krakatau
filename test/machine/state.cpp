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
#include <stream/byte_stream.h>
#include <gtest/gtest.h>
#include <value/empty.h>
#include <value/number.h>
#include <value/error.h>
#include <value/block.h>
#include <operation/block.h>
#include <value/tuple.h>
#include <value/type.h>
#include <value/boolean.h>
#include <rtti/rtti.h>
#include <operation/pop.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <value/operation.h>

TEST(MachineState, LoadEmpty) {
    std::vector<uint8_t> i = {MachineState::FORMAT_VERSION, '\'', 'n', 'a', 'm', 'e', '\'', 'E'};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    MachineState ms;
    ASSERT_EQ(1, ms.load(bs.get()));
    ASSERT_TRUE(ms.value_store().retrieve("name")->isOfClass<Value_Empty>());
}

TEST(MachineState, LoadNumber) {
    std::vector<uint8_t> i = {MachineState::FORMAT_VERSION, '\'', 'n', 'a', 'm', 'e', '\'', '#', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    MachineState ms;
    ASSERT_EQ(1, ms.load(bs.get()));
    ASSERT_TRUE(ms.value_store().retrieve("name")->isOfClass<Value_Number>());
    ASSERT_EQ(257, runtime_ptr_cast<Value_Number>(ms.value_store().retrieve("name"))->value());
}

TEST(MachineState, LoadTuple) {
    std::vector<uint8_t> i = {MachineState::FORMAT_VERSION, '\'', 'n', 'a', 'm', 'e', '\'', '(', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3,
                                                                   'E', '#', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 'E'};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    MachineState ms;
    ASSERT_EQ(1, ms.load(bs.get()));
    ASSERT_TRUE(ms.value_store().retrieve("name")->isOfClass<Value_Tuple>());
    Value_Tuple* tpl = runtime_ptr_cast<Value_Tuple>(ms.value_store().retrieve("name"));
    ASSERT_EQ(3, tpl->size());
    ASSERT_TRUE(tpl->at(0)->isOfClass<Value_Empty>());
    ASSERT_TRUE(tpl->at(1)->isOfClass<Value_Number>());
    ASSERT_TRUE(tpl->at(2)->isOfClass<Value_Empty>());
}

TEST(MachineState, LoadBoolean) {
    std::vector<uint8_t> i = {MachineState::FORMAT_VERSION, '\'', 't', 'r', 'u', 'e', '\'', '?', 0x1, '\'', 'f', 'a', 'l', 's', 'e', '\'', '?', 0x0};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    MachineState ms;
    ASSERT_EQ(2, ms.load(bs.get()));
    ASSERT_TRUE(runtime_ptr_cast<Value_Boolean>(ms.value_store().retrieve("true"))->value());
    ASSERT_FALSE(runtime_ptr_cast<Value_Boolean>(ms.value_store().retrieve("false"))->value());
}

TEST(MachineState, LoadError) {
    std::vector<uint8_t> i = {MachineState::FORMAT_VERSION, '\'', 't', 'e', 's', 't', '\'', '@', 0x4};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    MachineState ms;
    ASSERT_EQ(1, ms.load(bs.get()));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(ms.value_store().retrieve("test"))->value());
}

TEST(MachineState, LoadType) {
    std::vector<uint8_t> i = {MachineState::FORMAT_VERSION, '\'', 't', 'e', 's', 't', '\'', 'T', 0x4};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    MachineState ms;
    ASSERT_EQ(1, ms.load(bs.get()));
    ASSERT_EQ(ValueType::BLOCK, runtime_ptr_cast<Value_Type>(ms.value_store().retrieve("test"))->value());
}

TEST(MachineState, LoadBlock) {
    std::vector<uint8_t> i = {MachineState::FORMAT_VERSION, '\'', 't', 'e', 's', 't', '\'', '{',
                                0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 1,
                                0, 0, 0, 24};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    MachineState ms;
    ASSERT_EQ(1, ms.load(bs.get()));
    auto op = ms.value_store().retrieve("test")->asClass<Value_Operation>();
    ASSERT_NE(op, nullptr);
    auto blk = op->block();
    ASSERT_NE(blk, nullptr);
    ASSERT_EQ(1, blk->size());
}

TEST(MachineState, Serialize) {
    MachineState ms;
    ms.value_store().store("true", Value::fromBoolean(true));
    ms.value_store().store("false", Value::fromBoolean(false));
    ms.value_store().store("zero", Value::fromNumber(0));
    Serializer sz;
    ms.serialize(&sz);
    auto bs = ByteStream::anonymous((uint8_t*)sz.data(), sz.size());
    MachineState ms2;
    ASSERT_EQ(3, ms2.load(bs.get()));
    ASSERT_NE(nullptr, runtime_ptr_cast<Value_Boolean>(ms.value_store().retrieve("true")));
    ASSERT_NE(nullptr, runtime_ptr_cast<Value_Boolean>(ms.value_store().retrieve("false")));
    ASSERT_NE(nullptr, runtime_ptr_cast<Value_Number>(ms.value_store().retrieve("zero")));
}

TEST(MachineState, Parse) {
    Parser p("value foo empty\nvalue bar number 123\nvalue main block { nop }");
    MachineState ms;
    ASSERT_EQ(3, ms.load(&p));
    ASSERT_NE(nullptr, runtime_ptr_cast<Value_Empty>(ms.value_store().retrieve("foo")));
    ASSERT_NE(nullptr, runtime_ptr_cast<Value_Number>(ms.value_store().retrieve("bar")));
    ASSERT_NE(nullptr, runtime_ptr_cast<Value_Operation>(ms.value_store().retrieve("main")));
}

TEST(MachineState, BadVersion) {
    std::vector<uint8_t> i = {0, '\'', 't', 'e', 's', 't', '\'', '{',
                                0, 0, 0, 0, 0, 0, 0, 1,
                                0, 0, 0, 24};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    MachineState ms;
    ASSERT_EQ(0, ms.load(bs.get()));
    ASSERT_EQ(nullptr, ms.value_store().retrieve("test"));
}

TEST(MachineState, Execute) {
    Parser p("value main block { push number 123 dup add }");
    MachineState ms;
    ASSERT_EQ(1, ms.load(&p));
    auto ret = ms.execute();
    ASSERT_NE(std::nullopt, ret);
    ASSERT_EQ(Operation::Result::SUCCESS, ret.value());
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Number>());
}

TEST(MachineState, ExecuteOperation) {
    Parser p("value main operation dup");
    MachineState ms;
    ASSERT_EQ(1, ms.load(&p));
    ms.stack().push(Value::fromNumber(5));
    auto ret = ms.execute();
    ASSERT_EQ(Operation::Result::SUCCESS, ret.value());
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(ms.stack().pop()->isOfClass<Value_Number>());
    ASSERT_TRUE(ms.stack().pop()->isOfClass<Value_Number>());
}

TEST(MachineState, ExecuteBind) {
    Parser p("value main bind number 5 operation dup");
    MachineState ms;
    ASSERT_EQ(1, ms.load(&p));
    auto ret = ms.execute();
    ASSERT_EQ(Operation::Result::SUCCESS, ret.value());
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(ms.stack().pop()->isOfClass<Value_Number>());
    ASSERT_TRUE(ms.stack().pop()->isOfClass<Value_Number>());
}

TEST(MachineState, LoadNative) {
    Parser p("value main block { loadnative \"time\" call time::now () }");
    MachineState ms;
    ASSERT_EQ(1, ms.load(&p));
    auto ret = ms.execute();
    ASSERT_EQ(Operation::Result::SUCCESS, ret.value());
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Number>());
}
