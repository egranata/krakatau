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

#include <value/value.h>
#include <value/appendable.h>
#include <value/tuple.h>
#include <value/empty.h>
#include <value/boolean.h>
#include <gtest/gtest.h>

TEST(Appendable, AsValue) {
    auto tpl = Value::tuple({Value::empty(), Value::fromBoolean(false)});
    auto itpl = Appendable::asAppendable(tpl);

    ASSERT_NE(nullptr, itpl->asValue());
    ASSERT_TRUE(tpl->equals(itpl->asValue()));
}
