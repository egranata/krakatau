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
#include <operation/pop.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/error.h>
#include <error/error_codes.h>
#include <rtti/rtti.h>
#include <value/empty.h>

TEST(Pop, Test) {
    MachineState s;
    Pop p;
    ASSERT_EQ(Operation::Result::ERROR, p.execute(s));
    ASSERT_EQ(1, s.stack().size());
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::SUCCESS, p.execute(s));
    ASSERT_TRUE(s.stack().empty());
}

