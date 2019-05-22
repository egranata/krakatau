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

#include <stream/byte_stream.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(ByteStream, SizeOffset) {
    std::string buffer = "hello world";
    auto bs = ByteStream::anonymous((uint8_t*)buffer.data(), buffer.size());
    ASSERT_EQ(11, bs->size());
    ASSERT_NE(std::nullopt, bs->next());
}

TEST(ByteStream, Advance) {
    std::string buffer = "hello world";
    auto bs = ByteStream::anonymous((uint8_t*)buffer.data(), buffer.size());
    ASSERT_EQ('h', bs->next().value_or(0));
    ASSERT_EQ('e', bs->next().value_or(0));
    ASSERT_EQ('l', bs->next().value_or(0));
    ASSERT_EQ('l', bs->next().value_or(0));
    ASSERT_EQ('o', bs->next().value_or(0));
    ASSERT_EQ(' ', bs->next().value_or(0));
    ASSERT_EQ('w', bs->next().value_or(0));
    ASSERT_EQ('o', bs->next().value_or(0));
    ASSERT_EQ('r', bs->next().value_or(0));
    ASSERT_EQ('l', bs->next().value_or(0));
    ASSERT_EQ('d', bs->next().value_or(0));
    ASSERT_EQ(std::nullopt, bs->next());
    ASSERT_EQ(0, bs->next().value_or(0));
    ASSERT_TRUE(bs->eof());
}

TEST(ByteStream, Peek) {
    std::string buffer = "hello world";
    auto bs = ByteStream::anonymous((uint8_t*)buffer.data(), buffer.size());
    ASSERT_EQ('h', bs->peek().value_or(0));
    ASSERT_EQ('h', bs->peek().value_or(0));
    ASSERT_EQ('h', bs->peek().value_or(0));
    bs->next();
    ASSERT_EQ('e', bs->peek().value_or(0));
}

TEST(ByteStream, NextIf) {
    std::string buffer = "hello world";
    auto bs = ByteStream::anonymous((uint8_t*)buffer.data(), buffer.size());
    ASSERT_TRUE(bs->nextIf('h'));
    ASSERT_TRUE(bs->nextIf('e'));
    ASSERT_TRUE(bs->nextIf('l'));
    ASSERT_FALSE(bs->nextIf('e'));
    ASSERT_TRUE(bs->nextIf('l'));
    ASSERT_FALSE(bs->nextIf('e'));
    ASSERT_TRUE(bs->nextIf('o'));
}

TEST(ByteStream, NextIfNot) {
    std::string buffer = "hello world";
    auto bs = ByteStream::anonymous((uint8_t*)buffer.data(), buffer.size());
    ASSERT_FALSE(bs->nextIfNot('h').has_value());
    ASSERT_TRUE(bs->nextIfNot('e').has_value());
    ASSERT_TRUE(bs->nextIfNot('q').has_value());
    ASSERT_TRUE(bs->nextIfNot('q').has_value());
    ASSERT_FALSE(bs->nextIfNot('l').has_value());
    ASSERT_EQ('l', bs->next().value_or(0));
}

TEST(ByteStream, readUntil) {
    std::string buffer = "hello world";
    auto bs = ByteStream::anonymous((uint8_t*)buffer.data(), buffer.size());

    auto res = bs->readUntil('o');
    ASSERT_TRUE(res.first);
    ASSERT_EQ(5, res.second.size());
    ASSERT_EQ('h', res.second[0]);
    ASSERT_EQ('e', res.second[1]);
    ASSERT_EQ('l', res.second[2]);
    ASSERT_EQ('o', res.second[4]);

    res = bs->readUntil('x');
    ASSERT_FALSE(res.first);
    ASSERT_EQ(6, res.second.size());
    ASSERT_EQ(' ', res.second[0]);
    ASSERT_EQ('o', res.second[2]);
    ASSERT_EQ('r', res.second[3]);
    ASSERT_EQ('d', res.second[5]);

    res = bs->readUntil('q');
    ASSERT_FALSE(res.first);
    ASSERT_EQ(0, res.second.size());
}

TEST(ByteStream, HasAtLeast) {
    std::string buffer = "hello world";
    auto bs = ByteStream::anonymous((uint8_t*)buffer.data(), buffer.size());

    ASSERT_TRUE(bs->hasAtLeast(11));
    ASSERT_FALSE(bs->hasAtLeast(12));

    bs->next();
    ASSERT_TRUE(bs->hasAtLeast(1));
    ASSERT_TRUE(bs->hasAtLeast(5));
    ASSERT_TRUE(bs->hasAtLeast(9));
    ASSERT_FALSE(bs->hasAtLeast(11));

    bs->readUntil('x');
    ASSERT_FALSE(bs->hasAtLeast(1));
    ASSERT_FALSE(bs->hasAtLeast(2));
}

TEST(ByteStream, ReadNumber) {
    std::vector<uint8_t> i = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3,
                              0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1,
                              0x0, 0x5};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());

    auto n = bs->readNumber();
    ASSERT_TRUE(n.has_value());
    ASSERT_EQ(3, n.value_or(0));

    n = bs->readNumber(7);
    ASSERT_TRUE(n.has_value());
    ASSERT_EQ(1, n.value_or(0));

    n = bs->readNumber(1);
    ASSERT_TRUE(n.has_value());
    ASSERT_EQ(1, n.value_or(0));

    n = bs->readNumber();
    ASSERT_FALSE(n.has_value());
    n = bs->readNumber(3);
    ASSERT_FALSE(n.has_value());
    n = bs->readNumber(2);
    ASSERT_TRUE(n.has_value());
    ASSERT_EQ(5, n.value_or(1));
}

TEST(ByteStream, ReadExact) {
    std::vector<uint8_t> i = {0x0, 0x0, 0x0, 13};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    ASSERT_TRUE(bs->hasAtLeast(4));
    auto n = bs->readNumber(4);
    ASSERT_TRUE(n.has_value());
    ASSERT_EQ(13, n.value_or(0));
}

TEST(ByteStream, ReadIdentifier) {
    std::string buffer = "'hello''unterminated";
    auto bs = ByteStream::anonymous((uint8_t*)buffer.data(), buffer.size());

    auto n = bs->readIdentifier();
    ASSERT_TRUE(n.has_value());
    ASSERT_EQ("hello", n.value());

    n = bs->readIdentifier(7);
    ASSERT_FALSE(n.has_value());
}

TEST(ByteStream, ReadData) {
    std::vector<uint8_t> i = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 'A', 0x3, 13};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());

    auto os = bs->readData();
    ASSERT_TRUE(os.has_value());
    ASSERT_EQ("A", os.value());

    os = bs->readData();
    ASSERT_FALSE(os.has_value());
}

TEST(ByteStream, ReadBoolean) {
    std::vector<uint8_t> i = {0x0, 0x1, 0x2, 0x1};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());

    ASSERT_EQ(false, bs->readBoolean());
    ASSERT_EQ(true, bs->readBoolean());

    ASSERT_EQ(std::nullopt, bs->readBoolean());
    ASSERT_EQ(true, bs->readBoolean());

    ASSERT_EQ(std::nullopt, bs->readBoolean());
}
