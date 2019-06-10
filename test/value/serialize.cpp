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
#include <operation/block.h>
#include <operation/bind.h>
#include <value/value_types.h>
#include <value/tuple.h>
#include <operation/nop.h>
#include <operation/dup.h>
#include <operation/pop.h>
#include <operation/resetstack.h>
#include <value/operation.h>
#include <value/string.h>
#include <value/table.h>
#include <value/atom.h>
#include <value/bind.h>
#include <value/character.h>
#include <value/type.h>
#include <value/set.h>

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

TEST(ValueSerialize, Character) {
    auto val = Value::fromCharacter(0x1234);
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(dv->isOfClass<Value_Character>());
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
    auto val = Value::tuple({Value::empty(), Value::fromNumber(1234), Value::fromBoolean(false)});
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->equals(dv));
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
    auto val = Value::table({ {Value::fromString("one two three"), Value::fromNumber(123)}, {Value::fromString("four five seven"), Value::fromNumber(457)} });
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->isOfClass<Value_Table>());
    ASSERT_TRUE(val->equals(dv));
    ASSERT_EQ(2, val->size());
    ASSERT_EQ(runtime_ptr_cast<Value_Table>(dv)->size(), val->size());
}

TEST(ValueSerialize, Bind) {
    auto val = Value::fromBind(std::make_shared<PartialBind>(Value::fromNumber(1), std::make_shared<Dup>()));
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->equals(dv));
}

TEST(ValueSerialize, Set) {
    auto val = Value::set({ Value::fromString("one two three"), Value::fromNumber(457) });
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(val->isOfClass<Value_Set>());
    ASSERT_TRUE(val->equals(dv));
    ASSERT_EQ(2, val->size());
    ASSERT_EQ(runtime_ptr_cast<Value_Set>(dv)->size(), val->size());
}

TEST(ValueSerialize, Atom) {
    auto val = Value::atom("true");
    Serializer s;
    val->serialize(&s);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    auto dv = Value::fromByteStream(bs.get());
    ASSERT_NE(nullptr, dv);
    ASSERT_TRUE(dv->isOfClass<Value_Atom>());
    ASSERT_TRUE(val->equals(dv));
}
