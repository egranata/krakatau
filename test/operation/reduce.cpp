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
#include <operation/reduce.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/boolean.h>
#include <value/tuple.h>
#include <value/table.h>
#include <value/block.h>
#include <value/operation.h>
#include <operation/push.h>
#include <operation/equals.h>
#include <value/error.h>
#include <rtti/rtti.h>
#include <parser/parser.h>
#include <value/number.h>

TEST(Reduce, ZeroArgs) {
    MachineState s;
    Reduce r;
    ASSERT_EQ(Operation::Result::ERROR, r.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(0, s.stack().size());
}

TEST(Reduce, OneArg) {
    MachineState s;
    Reduce r;
    s.stack().push(Value::tuple({}));
    ASSERT_EQ(Operation::Result::ERROR, r.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(1, s.stack().size());
}

TEST(Reduce, TwoArgs) {
    MachineState s;
    Reduce r;
    s.stack().push(Value::tuple({}));
    s.stack().push(Value::tuple({}));
    ASSERT_EQ(Operation::Result::ERROR, r.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
}

TEST(Reduce, MistypedArgs) {
    MachineState s;
    Reduce r;
    s.stack().push(Value::fromBoolean(false));
    s.stack().push(Value::tuple({}));
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, r.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(3, s.stack().size());
}

TEST(Reduce, TupleBlock) {
    MachineState s;
    Reduce r;
    auto val_tpl = Parser("tuple ( number 3 number 12 number 0 )").parseValuePayload();
    auto val_blk = Parser("block { add }").parseValuePayload();
    auto res_val = Parser("number 16").parseValuePayload();
    s.stack().push(val_tpl);
    s.stack().push(val_blk);
    s.stack().push(Value::fromNumber(1));
    ASSERT_EQ(Operation::Result::SUCCESS, r.execute(s));
    ASSERT_TRUE(res_val->equals(s.stack().peek()));
}

TEST(Reduce, TableBlock) {
    MachineState s;
    Reduce r;
    auto val_tpl = Parser("table [ number 3 -> number 4, number 4 -> number 5, number 0 -> number 1 ]").parseValuePayload();
    auto val_blk = Parser("block { swap dup push number 0 at swap push number 1 at mul add }").parseValuePayload();
    auto res_val = Parser("number 37").parseValuePayload();
    s.stack().push(val_tpl);
    s.stack().push(val_blk);
    s.stack().push(Value::fromNumber(5));
    ASSERT_EQ(Operation::Result::SUCCESS, r.execute(s));
    ASSERT_TRUE(res_val->equals(s.stack().peek()));
}
