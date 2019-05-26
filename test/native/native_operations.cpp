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
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    ASSERT_EQ(nullptr, no.find("nothing"));
}

TEST(NativeOperations, CreateBucket) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    ASSERT_NE(nullptr, no.create("bucket"));
    ASSERT_NE(nullptr, no.find("bucket"));
    ASSERT_EQ(no.create("bucket").get(), no.find("bucket").get());
}

TEST(NativeOperations, CreateOperation) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    NativeOperations::NativeOperationLoader l = {
            [] () -> std::shared_ptr<Native> { return nullptr; },
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->registerOperation("op", l));
}

TEST(NativeOperations, DisallowDuplicates) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    NativeOperations::NativeOperationLoader l1 = {
            [] () -> std::shared_ptr<Native> { return nullptr; },
    };

    NativeOperations::NativeOperationLoader l2 = {
            [] () -> std::shared_ptr<Native> { return nullptr; },
    };

    auto bucket = no.create("bucket");

    ASSERT_TRUE(bucket->registerOperation("op", l1));
    ASSERT_FALSE(bucket->registerOperation("op", l1));
    ASSERT_FALSE(bucket->registerOperation("op", l2));
}

TEST(NativeOperations, FindMissingOperation) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    auto bucket = no.create("bucket");
    auto l = bucket->find("no");
    ASSERT_EQ(nullptr, l.mCreator);
}

TEST(NativeOperations, FindPresentOperation) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    NativeOperations::NativeOperationLoader l = {
            [] () -> std::shared_ptr<Native> { return nullptr; },
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->registerOperation("op", l));

    auto fl = bucket->find("op");
    ASSERT_NE(nullptr, fl.mCreator);
}

TEST(NativeOperations, GetLoaderNoBucket) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    auto fl = no.getLoader("no::op");
    ASSERT_EQ(nullptr, fl.mCreator);
}

TEST(NativeOperations, GetLoaderNoOp) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    auto bucket = no.create("bucket");

    auto fl = no.getLoader("bucket::noop");
    ASSERT_EQ(nullptr, fl.mCreator);
}

TEST(NativeOperations, GetLoaderNoSeparator) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    NativeOperations::NativeOperationLoader l = {
            [] () -> std::shared_ptr<Native> { return nullptr; },
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->registerOperation("op", l));

    auto fl = no.getLoader("bucketop");
    ASSERT_EQ(nullptr, fl.mCreator);
}

TEST(NativeOperations, GetLoaderOk) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    NativeOperations::NativeOperationLoader l = {
            [] () -> std::shared_ptr<Native> { return nullptr; },
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->registerOperation("op", l));

    auto fl = no.getLoader("bucket::op");
    ASSERT_NE(nullptr, fl.mCreator);
}
