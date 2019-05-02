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

#include <operations/at.h>
#include <operations/arith.h>
#include <operations/push.h>
#include <operations/op_loader.h>
#include <operations/resetstack.h>
#include <value/boolean.h>
#include <gtest/gtest.h>
#include <operations/store.h>
#include <rtti/rtti.h>
#include <stream/byte_stream.h>
#include <operations/load.h>

TEST(OpLoader, At) {
    std::vector<uint8_t> i = {0, 0, 0, 13};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<At>());
}

TEST(OpLoader, Arith) {
    std::vector<uint8_t> i = {0, 0, 0, 5,
                              0, 0, 0, 10};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Add_Binary_Arithmetic_Operation>());
    opl = OperationLoader::loader();
    op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Positive_Unary_Arithmetic_Operation>());
}

TEST(OpLoader, InvalidOp) {
    std::vector<uint8_t> i = {0, 0, 1, 121};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_EQ(nullptr, op);
}

TEST(OpLoader, Push) {
    std::vector<uint8_t> i = {0, 0, 0, 25, '?', 1};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Push>());
    ASSERT_TRUE(runtime_ptr_cast<Push>(op)->value()->isOfClass<Value_Boolean>());
    ASSERT_TRUE(runtime_ptr_cast<Value_Boolean>(runtime_ptr_cast<Push>(op)->value())->value());
}

TEST(OpLoader, Load) {
    std::vector<uint8_t> i = {0, 0, 0, 18, '\'', 'h', 'e', 'l', 'l', 'o', '\''};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Load>());
    ASSERT_EQ("hello", runtime_ptr_cast<Load>(op)->key());
}

TEST(OpLoader, Store) {
    std::vector<uint8_t> i = {0, 0, 0, 28, '\'', 'h', 'e', 'l', 'l', 'o', '\''};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<Store>());
    ASSERT_EQ("hello", runtime_ptr_cast<Store>(op)->key());
}

TEST(OpLoader, ResetStack) {
    std::vector<uint8_t> i = {0, 0, 0, 26};
    auto bs = ByteStream::anonymous((uint8_t*)i.data(), i.size());
    auto opl = OperationLoader::loader();
    auto op = opl->fromByteStream(bs.get());
    ASSERT_NE(nullptr, op);
    ASSERT_TRUE(op->isOfClass<ResetStack>());
}