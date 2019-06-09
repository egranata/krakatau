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

#include <uniq/uniq.h>
#include <gtest/gtest.h>

TEST(Uniq, NewEmpty) {
    Uniq<int> q;
    ASSERT_EQ(0, q.size());
}

TEST(Uniq, AddOne) {
    Uniq<int> q;
    ASSERT_NE(nullptr, q.add(123));
    ASSERT_EQ(1, q.size());
    ASSERT_NE(nullptr, q.add(123));
    ASSERT_EQ(1, q.size());
}

TEST(Uniq, FindPresent) {
    Uniq<int> q;
    ASSERT_NE(nullptr, q.add(123));
    ASSERT_NE(nullptr, q.find(123));
}

TEST(Uniq, FindAbsent) {
    Uniq<int> q;
    ASSERT_EQ(nullptr, q.find(34));
}

TEST(Uniq, FindEquals) {
    Uniq<int> q;
    auto val = q.add(123);
    ASSERT_EQ(val, q.find(123));
    ASSERT_EQ(val, q.add(123));
    ASSERT_EQ(1, q.size());
}
