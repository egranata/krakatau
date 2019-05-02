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

#include <gtest/gtest.h>
#include <operations/at.h>
#include <operations/dup.h>
#include <operations/load.h>
#include <operations/op.h>

TEST(Operations, Equal) {
    std::shared_ptr<Operation> at1(new At());
    std::shared_ptr<Operation> at2(new At());
    ASSERT_TRUE(at1->equals(at2));
    ASSERT_TRUE(at2->equals(at1));
}

TEST(Operations, Unequal) {
    std::shared_ptr<Operation> at(new At());
    std::shared_ptr<Operation> dup(new Dup());
    ASSERT_FALSE(at->equals(dup));
    ASSERT_FALSE(dup->equals(at));
}

TEST(Operations, ParametricEqual) {
    std::shared_ptr<Operation> l1(new Load("foo"));
    std::shared_ptr<Operation> l2(new Load("foo"));
    ASSERT_TRUE(l1->equals(l2));
    ASSERT_TRUE(l2->equals(l1));
}

TEST(Operations, ParametricUnequal) {
    std::shared_ptr<Operation> l1(new Load("foo"));
    std::shared_ptr<Operation> l2(new Load("bar"));
    ASSERT_FALSE(l1->equals(l2));
    ASSERT_FALSE(l2->equals(l1));
}

TEST(Operations, MixedTypes) {
    std::shared_ptr<Operation> l1(new Load("foo"));
    std::shared_ptr<Operation> at(new At());
    ASSERT_FALSE(l1->equals(at));
    ASSERT_FALSE(at->equals(l1));
}
