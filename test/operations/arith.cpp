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
#include <operations/arith.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/number.h>
#include <value/empty.h>
#include <value/boolean.h>
#include <value/error.h>
#include <rtti/rtti.h>

#define BINARY_ARIT_OPERATION_TEST(Class, n1, n2, result) \
TEST(Class, OkType) { \
    MachineState s; \
    s.stack().push(Value::fromNumber(n1)); \
    s.stack().push(Value::fromNumber(n2)); \
    Class ## _Binary_Arithmetic_Operation a; \
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(s)); \
    ASSERT_EQ(1, s.stack().size()); \
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>()); \
    ASSERT_EQ(result, runtime_ptr_cast<Value_Number>(s.stack().peek())->value()); \
} \
TEST(Class, BadType) { \
    MachineState s; \
    s.stack().push(Value::fromNumber(n1)); \
    s.stack().push(Value::empty()); \
    ASSERT_EQ(2, s.stack().size()); \
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Empty>()); \
    Class ## _Binary_Arithmetic_Operation a; \
    ASSERT_EQ(Operation::Result::ERROR, a.execute(s)); \
    ASSERT_EQ(3, s.stack().size()); \
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Error>()); \
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().peek())->value()); \
    s.stack().pop(); \
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Empty>()); \
}

BINARY_ARIT_OPERATION_TEST(Add, 12, 30, 42);
BINARY_ARIT_OPERATION_TEST(Subtract, 12, 30, 18);
BINARY_ARIT_OPERATION_TEST(Multiply, 4, 44, 176);
BINARY_ARIT_OPERATION_TEST(Divide, 5, 25, 5);
BINARY_ARIT_OPERATION_TEST(Modulo, 7, 31, 3);

TEST(Divide, ErrorOnZero) {
    MachineState s;
    s.stack().push(Value::fromNumber(0));
    s.stack().push(Value::fromNumber(12));
    Divide_Binary_Arithmetic_Operation a;
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(s));
    ASSERT_EQ(3, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Error>());
    s.stack().pop();
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(12, runtime_ptr_cast<Value_Number>(s.stack().peek())->value());
}

TEST(Modulo, ErrorOnZero) {
    MachineState s;
    s.stack().push(Value::fromNumber(0));
    s.stack().push(Value::fromNumber(12));
    Modulo_Binary_Arithmetic_Operation a;
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(s));
    ASSERT_EQ(3, s.stack().size());
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Error>());
    s.stack().pop();
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(12, runtime_ptr_cast<Value_Number>(s.stack().peek())->value());
}

#define UNARY_ARIT_OPERATION_TEST(Class, id, n, result) \
TEST(Class ## id, OkType) { \
    MachineState s; \
    s.stack().push(Value::fromNumber(n)); \
    Class ## _Unary_Arithmetic_Operation a; \
    ASSERT_EQ(Operation::Result::SUCCESS, a.execute(s)); \
    ASSERT_EQ(1, s.stack().size()); \
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Boolean>()); \
    ASSERT_EQ(result, runtime_ptr_cast<Value_Boolean>(s.stack().peek())->value()); \
} \
TEST(Class ## id, BadType) { \
    MachineState s; \
    s.stack().push(Value::empty()); \
    Class ## _Unary_Arithmetic_Operation a; \
    ASSERT_EQ(Operation::Result::ERROR, a.execute(s)); \
    ASSERT_EQ(2, s.stack().size()); \
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Error>()); \
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().peek())->value()); \
    s.stack().pop(); \
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Empty>()); \
}

UNARY_ARIT_OPERATION_TEST(Positive, T, (uint64_t)+1, true);
UNARY_ARIT_OPERATION_TEST(Negative, T, (uint64_t)-1, true);

UNARY_ARIT_OPERATION_TEST(Positive, F, (uint64_t)-1, false);
UNARY_ARIT_OPERATION_TEST(Negative, F, (uint64_t)+1, false);

UNARY_ARIT_OPERATION_TEST(Zero, T, 0, true);
UNARY_ARIT_OPERATION_TEST(Zero, FN, (uint64_t)-1, false);
UNARY_ARIT_OPERATION_TEST(Zero, FP, (uint64_t)+1, false);
