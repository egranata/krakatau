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

#include <error/error_codes.h>
#include <stream/byte_stream.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>
#include <rtti/enum.h>

TEST(ErrorCodes, ValidLoad) {
    std::vector<uint8_t> i = {2, 1};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto ec = errorCodeFromByteStream(bs.get());
    ASSERT_TRUE(ec.has_value());
    ASSERT_EQ(ErrorCode::NOT_FOUND, ec.value());
    ec = errorCodeFromByteStream(bs.get());
    ASSERT_TRUE(ec.has_value());
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, ec.value());
}

TEST(ErrorCodes, InvalidLoad) {
    std::vector<uint8_t> i = {128};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto ec = errorCodeFromByteStream(bs.get());
    ASSERT_FALSE(ec.has_value());
}

TEST(ErrorCodes, FromString) {
    auto err = errorCodeFromString("notFound");
    ASSERT_NE(err, std::nullopt);
    ASSERT_EQ(ErrorCode::NOT_FOUND, err.value());
    err = errorCodeFromString("no such error");
    ASSERT_EQ(std::nullopt, err);
}
