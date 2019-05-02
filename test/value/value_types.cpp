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

#include <value/value_types.h>
#include <stream/byte_stream.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <rtti/enum.h>

TEST(ValueTypes, ValidLoad) {
    std::vector<uint8_t> i = {3, 6};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto ec = valueTypeFromByteStream(bs.get());
    ASSERT_TRUE(ec.has_value());
    ASSERT_EQ(ValueType::BOOLEAN, ec.value());
    ec = valueTypeFromByteStream(bs.get());
    ASSERT_TRUE(ec.has_value());
    ASSERT_EQ(ValueType::ERROR, ec.value());
}

TEST(ValueTypes, InvalidLoad) {
    std::vector<uint8_t> i = {128};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto ec = valueTypeFromByteStream(bs.get());
    ASSERT_FALSE(ec.has_value());
}

TEST(ValueTypes, FromString) {
    auto val = valueTypeFromString("notFound");
    ASSERT_EQ(val, std::nullopt);
    val = valueTypeFromString("table");
    ASSERT_NE(std::nullopt, val);
    ASSERT_EQ(ValueType::TABLE, val.value());
}
