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

#include <value/value.h>
#include <value/empty.h>
#include <value/boolean.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <stream/serializer.h>
#include <stream/byte_stream.h>
#include <value/number.h>
#include <error/error_codes.h>
#include <value/block.h>
#include <function/block.h>
#include <value/value_types.h>
#include <value/tuple.h>
#include <operations/nop.h>
#include <operations/pop.h>
#include <operations/resetstack.h>
#include <value/operation.h>
#include <value/string.h>
#include <value/table.h>

TEST(ValueSerialize, Boolean) {
    auto val = Value::fromBoolean(true);
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(dv->isOfClass<Value_Boolean>());
    ASSERT_TRUE(val->equals(dv));
}

TEST(ValueSerialize, Empty) {
    auto val = Value::empty();
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(dv->isOfClass<Value_Empty>());
    ASSERT_TRUE(val->equals(dv));
}

TEST(ValueSerialize, Number) {
    auto val = Value::fromNumber(409612);
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(dv->isOfClass<Value_Number>());
    ASSERT_EQ(runtime_ptr_cast<Value_Number>(val)->value(), runtime_ptr_cast<Value_Number>(dv)->value());
    ASSERT_TRUE(val->equals(dv));
}

TEST(ValueSerialize, Error) {
    auto val = Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS);
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->equals(dv));
}

TEST(ValueSerialize, Type) {
    auto val = Value::type(ValueType::NUMBER);
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->equals(dv));
}

TEST(ValueSerialize, Tuple) {
    auto val = Value::tuple();
    auto tpl = runtime_ptr_cast<Value_Tuple>(val);
    tpl->append(Value::empty())
       ->append(Value::fromNumber(1234))
       ->append(Value::fromBoolean(false));
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->equals(dv));
}

TEST(ValueSerialize, Block) {
    auto blk = std::make_shared<Block>();
    blk->add(std::make_shared<Nop>());
    blk->add(std::make_shared<ResetStack>());
    blk->add(std::make_shared<Pop>());
    auto val = Value::fromBlock(blk);
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(dv->isOfClass<Value_Block>());
    ASSERT_EQ(3, runtime_ptr_cast<Value_Block>(dv)->value()->size());
}

TEST(ValueSerialize, Operation) {
    auto val = Value::fromOperation(std::shared_ptr<Operation>(new ResetStack()));
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->isOfClass<Value_Operation>());
    ASSERT_TRUE(runtime_ptr_cast<Value_Operation>(val)->value()->isOfClass<ResetStack>());
}

TEST(ValueSerialize, String) {
    auto val = Value::fromString("a test string here");
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->isOfClass<Value_String>());
    ASSERT_TRUE(val->equals(dv));
    ASSERT_EQ(runtime_ptr_cast<Value_String>(val)->value(), runtime_ptr_cast<Value_String>(dv)->value());
}

TEST(ValueSerialize, Table) {
    auto val = Value::table();
    auto tbl = runtime_ptr_cast<Value_Table>(val);
    tbl->append(Value::fromString("one two three"), Value::fromNumber(123));
    tbl->append(Value::fromString("four five seven"), Value::fromNumber(457));
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->isOfClass<Value_Table>());
    ASSERT_TRUE(val->equals(dv));
    ASSERT_EQ(runtime_ptr_cast<Value_Table>(dv)->size(), tbl->size());
}