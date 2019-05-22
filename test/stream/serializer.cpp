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

#include <stream/serializer.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <stream/byte_stream.h>

TEST(Serializer, WriteCount) {
    Serializer s;
    ASSERT_EQ(8, s.writeNumber(5000));
    ASSERT_EQ(4, s.writeNumber(1234, 4));
    ASSERT_EQ(8, s.writeNumber(5000, 8));
    ASSERT_EQ(2, s.writeNumber(12, 2));
    ASSERT_EQ(1, s.writeNumber(50, 1));
    ASSERT_EQ(7, s.writeIdentifier("hello"));
}

TEST(Serializer, Number) {
    Serializer s;
    s.writeNumber(1234);
    s.writeNumber(500, 4);
    s.writeNumber(800, 2);
    s.writeNumber(50, 1);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    ASSERT_EQ(1234, bs->readNumber(8).value_or(0));
    ASSERT_EQ(500, bs->readNumber(4).value_or(0));
    ASSERT_EQ(800, bs->readNumber(2).value_or(0));
    ASSERT_EQ(50, bs->readNumber(1).value_or(0));
    ASSERT_FALSE(bs->readNumber().has_value());
}

TEST(Serializer, Identifier) {
    Serializer s;
    s.writeIdentifier("hello");
    auto bs = ByteStream::anonymous(s.data(), s.size());
    ASSERT_EQ("hello", bs->readIdentifier().value_or(""));
}

TEST(Serializer, Data) {
    Serializer s;
    s.writeData(11, (uint8_t*)"hello world");
    auto bs = ByteStream::anonymous(s.data(), s.size());
    ASSERT_EQ("hello world", bs->readData().value_or(""));
}

TEST(Serializer, Boolean) {
    Serializer s;
    s.writeBoolean(false);
    s.writeBoolean(true);
    auto bs = ByteStream::anonymous(s.data(), s.size());
    ASSERT_EQ(false, bs->readBoolean().value());
    ASSERT_EQ(true, bs->readBoolean().value());
}
