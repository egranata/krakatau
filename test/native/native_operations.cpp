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

namespace {
    class TestOp : public Native {
    public:
        TestOp(std::shared_ptr<NativeOperations::Bucket> b) : Native(b, "testop") {}

        Operation::Result execute(MachineState&) override {
            return Operation::Result::SUCCESS;
        }
        std::shared_ptr<Operation> clone() const override {
            return std::make_shared<TestOp>(bucket());
        }
    };
}

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
            [] (std::shared_ptr<NativeOperations::Bucket> b) -> std::shared_ptr<Native> { return std::make_shared<TestOp>(b); },
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->newOperation(l));
}

TEST(NativeOperations, DisallowDuplicates) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    NativeOperations::NativeOperationLoader l1 = {
            [] (std::shared_ptr<NativeOperations::Bucket> b) -> std::shared_ptr<Native> { return std::make_shared<TestOp>(b); },
    };

    NativeOperations::NativeOperationLoader l2 = {
            [] (std::shared_ptr<NativeOperations::Bucket> b) -> std::shared_ptr<Native> { return std::make_shared<TestOp>(b); },
    };

    auto bucket = no.create("bucket");

    ASSERT_TRUE(bucket->newOperation(l1));
    ASSERT_FALSE(bucket->newOperation(l1));
    ASSERT_FALSE(bucket->newOperation(l2));
}

TEST(NativeOperations, FindMissingOperation) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    auto bucket = no.create("bucket");
    auto l = bucket->find("no");
    ASSERT_EQ(nullptr, l);
}

TEST(NativeOperations, FindPresentOperation) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    NativeOperations::NativeOperationLoader l = {
            [] (std::shared_ptr<NativeOperations::Bucket> b) -> std::shared_ptr<Native> { return std::make_shared<TestOp>(b); },
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->newOperation(l));

    auto fl = bucket->find("testop");
    ASSERT_NE(nullptr, fl);
}

TEST(NativeOperations, GetLoaderNoBucket) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    auto fl = no.getOperation("no::op");
    ASSERT_EQ(nullptr, fl);
}

TEST(NativeOperations, GetLoaderNoOp) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    auto bucket = no.create("bucket");

    auto fl = no.getOperation("bucket::noop");
    ASSERT_EQ(nullptr, fl);
}

TEST(NativeOperations, GetLoaderNoSeparator) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    NativeOperations::NativeOperationLoader l = {
            [] (std::shared_ptr<NativeOperations::Bucket> b) -> std::shared_ptr<Native> { return std::make_shared<TestOp>(b); },
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->newOperation(l));

    auto fl = no.getOperation("bucketop");
    ASSERT_EQ(nullptr, fl);
}

TEST(NativeOperations, GetLoaderOk) {
    MachineState ms;
    NativeOperations& no = ms.native_operations();
    NativeOperations::NativeOperationLoader l = {
            [] (std::shared_ptr<NativeOperations::Bucket> b) -> std::shared_ptr<Native> { return std::make_shared<TestOp>(b); },
    };
    auto bucket = no.create("bucket");
    ASSERT_TRUE(bucket->newOperation(l));

    auto fl = no.getOperation("bucket::testop");
    ASSERT_NE(nullptr, fl);
}
