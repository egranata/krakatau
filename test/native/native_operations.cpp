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
#include <native/native_operations.h>
#include <operation/native.h>
#include <gtest/gtest.h>
#include <rtti/rtti.h>

TEST(NativeOperations, FindMissingBucket) {
    NativeOperations no;
    ASSERT_EQ(nullptr, no.find("nothing"));
}

TEST(NativeOperations, CreateBucket) {
    NativeOperations no;
    ASSERT_NE(nullptr, no.create("bucket"));
    ASSERT_NE(nullptr, no.find("bucket"));
    ASSERT_EQ(no.create("bucket").get(), no.find("bucket").get());
}

TEST(NativeOperations, CreateOperation) {
    NativeOperations no;
    NativeOperations::NativeOperationLoader l = {
            [] (Parser*) -> std::shared_ptr<Native> { return nullptr; },
            [] (ByteStream*) -> std::shared_ptr<Native> { return nullptr; }
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->registerOperation("op", l));
}

TEST(NativeOperations, DisallowDuplicates) {
    NativeOperations no;
    NativeOperations::NativeOperationLoader l1 = {
            [] (Parser*) -> std::shared_ptr<Native> { return nullptr; },
            [] (ByteStream*) -> std::shared_ptr<Native> { return nullptr; }
    };

    NativeOperations::NativeOperationLoader l2 = {
            [] (Parser*) -> std::shared_ptr<Native> { return nullptr; },
            [] (ByteStream*) -> std::shared_ptr<Native> { return nullptr; }
    };

    auto bucket = no.create("bucket");

    ASSERT_TRUE(bucket->registerOperation("op", l1));
    ASSERT_FALSE(bucket->registerOperation("op", l1));
    ASSERT_FALSE(bucket->registerOperation("op", l2));
}

TEST(NativeOperations, FindMissingOperation) {
    NativeOperations no;
    auto bucket = no.create("bucket");
    auto l = bucket->find("no");
    ASSERT_EQ(nullptr, l.mParser);
    ASSERT_EQ(nullptr, l.mLoader);
}

TEST(NativeOperations, FindPresentOperation) {
    NativeOperations no;
    NativeOperations::NativeOperationLoader l = {
            [] (Parser*) -> std::shared_ptr<Native> { return nullptr; },
            [] (ByteStream*) -> std::shared_ptr<Native> { return nullptr; }
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->registerOperation("op", l));

    auto fl = bucket->find("op");
    ASSERT_NE(nullptr, fl.mParser);
    ASSERT_NE(nullptr, fl.mLoader);
}
