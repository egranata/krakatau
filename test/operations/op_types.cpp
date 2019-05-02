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

#include <operations/op_types.h>
#include <stream/byte_stream.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <rtti/enum.h>

TEST(OperationTypes, ValidLoad) {
    std::vector<uint8_t> i = {0, 0, 0, 5,
                              0, 0, 0, 15};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto ec = operationTypeFromByteStream(bs.get());
    ASSERT_TRUE(ec.has_value());
    ASSERT_EQ(OperationType::ADD, ec.value());
    ec = operationTypeFromByteStream(bs.get());
    ASSERT_TRUE(ec.has_value());
    ASSERT_EQ(OperationType::EQUALS, ec.value());
}

TEST(OperationTypes, InvalidLoad) {
    std::vector<uint8_t> i = {0, 1, 0, 128};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto ec = operationTypeFromByteStream(bs.get());
    ASSERT_FALSE(ec.has_value());
}

TEST(OperationTypes, InsufficientSize) {
    std::vector<uint8_t> i = {0, 2};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto ec = operationTypeFromByteStream(bs.get());
    ASSERT_FALSE(ec.has_value());
}

TEST(OperationTypes, FromString) {
    auto val = operationTypeFromString("notFound");
    ASSERT_EQ(val, std::nullopt);
    val = operationTypeFromString("store");
    ASSERT_NE(std::nullopt, val);
    ASSERT_EQ(OperationType::STORE, val.value());
}
