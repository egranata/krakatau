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
#include <operation/logical.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/number.h>
#include <value/empty.h>
#include <value/boolean.h>
#include <value/error.h>
#include <rtti/rtti.h>

#define TEST_BINARY_LOGICAL_BADTYPE(Type) \
TEST(Type, BadType) { \
    MachineState ms; \
    ms.stack().push(Value::empty()); \
    Type a; \
    ASSERT_EQ(Operation::Result::ERROR, a.execute(ms)); \
    ASSERT_EQ(2, ms.stack().size()); \
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(ms.stack().pop())->value()); \
    ms.stack().push(Value::fromBoolean(false)); \
    ASSERT_EQ(Operation::Result::ERROR, a.execute(ms)); \
    ASSERT_EQ(3, ms.stack().size()); \
    ASSERT_TRUE(ms.stack().pop()->isOfClass<Value_Error>()); \
}

TEST_BINARY_LOGICAL_BADTYPE(And);
TEST_BINARY_LOGICAL_BADTYPE(Or);
TEST_BINARY_LOGICAL_BADTYPE(Xor);

#undef TEST_BINARY_LOGICAL_BADTYPE

#define TEST_BINARY_LOGICAL_TRUTH(Type, ff, ft, tf, tt) \
TEST(Type, TruthTable) { \
    MachineState ms; \
    Type a; \
    ms.stack().push(Value::fromBoolean(false)); \
    ms.stack().push(Value::fromBoolean(false)); \
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(ms)); \
    ASSERT_EQ(ff, runtime_ptr_cast<Value_Boolean>(ms.stack().pop())->value()); \
    ms.stack().push(Value::fromBoolean(false)); \
    ms.stack().push(Value::fromBoolean(true)); \
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(ms)); \
    ASSERT_EQ(ft, runtime_ptr_cast<Value_Boolean>(ms.stack().pop())->value()); \
    ms.stack().push(Value::fromBoolean(true)); \
    ms.stack().push(Value::fromBoolean(false)); \
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(ms)); \
    ASSERT_EQ(tf, runtime_ptr_cast<Value_Boolean>(ms.stack().pop())->value()); \
    ms.stack().push(Value::fromBoolean(true)); \
    ms.stack().push(Value::fromBoolean(true)); \
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(ms)); \
    ASSERT_EQ(tt, runtime_ptr_cast<Value_Boolean>(ms.stack().pop())->value()); \
}

TEST_BINARY_LOGICAL_TRUTH(And, false, false, false, true);
TEST_BINARY_LOGICAL_TRUTH(Or, false, true, true, true);
TEST_BINARY_LOGICAL_TRUTH(Xor, false, true, true, false);

#undef TEST_BINARY_LOGICAL_TRUTH

#define TEST_UNARY_LOGICAL_BADTYPE(Type) \
TEST(Type, BadType) { \
    MachineState ms; \
    ms.stack().push(Value::empty()); \
    Type a; \
    ASSERT_EQ(Operation::Result::ERROR, a.execute(ms)); \
    ASSERT_EQ(2, ms.stack().size()); \
    ASSERT_TRUE(ms.stack().pop()->isOfClass<Value_Error>()); \
}

TEST_UNARY_LOGICAL_BADTYPE(Not);

#undef TEST_UNARY_LOGICAL_BADTYPE

#define TEST_UNARY_LOGICAL_TRUTH(Type, f, t) \
TEST(Type, TruthTable) { \
    MachineState ms; \
    Type a; \
    ms.stack().push(Value::fromBoolean(false)); \
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(ms)); \
    ASSERT_EQ(f, runtime_ptr_cast<Value_Boolean>(ms.stack().pop())->value()); \
    ms.stack().push(Value::fromBoolean(true)); \
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(ms)); \
    ASSERT_EQ(t, runtime_ptr_cast<Value_Boolean>(ms.stack().pop())->value()); \
}

TEST_UNARY_LOGICAL_TRUTH(Not, true, false);

#undef TEST_UNARY_LOGICAL_TRUTH
