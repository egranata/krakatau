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
#include <operations/map.h>
#include <value/value.h>
#include <gtest/gtest.h>
#include <value/boolean.h>
#include <value/tuple.h>
#include <value/table.h>
#include <value/block.h>
#include <value/operation.h>
#include <operations/push.h>
#include <operations/equals.h>
#include <value/error.h>
#include <rtti/rtti.h>
#include <parser/parser.h>
#include <value/number.h>

TEST(Map, ZeroArgs) {
    MachineState s;
    Map mp;
    ASSERT_EQ(Operation::Result::ERROR, mp.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(0, s.stack().size());
}

TEST(Map, OneArg) {
    MachineState s;
    Map mp;
    s.stack().push(Value::tuple());
    ASSERT_EQ(Operation::Result::ERROR, mp.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(1, s.stack().size());
}

TEST(Map, MistypedArgs) {
    MachineState s;
    Map mp;
    s.stack().push(Value::tuple());
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, mp.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
}

TEST(Map, TupleOperation) {
    MachineState s;
    Map mp;
    auto val_tpl = Parser("tuple ( number 3 empty boolean false )").parseValuePayload();
    auto val_op = Parser("operation typeof").parseValuePayload();
    auto res_tpl = Parser("tuple ( type number type empty type boolean )").parseValuePayload();
    s.stack().push(val_tpl);
    s.stack().push(val_op);
    ASSERT_EQ(Operation::Result::SUCCESS, mp.execute(s));
    ASSERT_TRUE(res_tpl->equals(s.stack().peek()));
}

TEST(Map, TupleBlock) {
    MachineState s;
    Map mp;
    auto val_tpl = Parser("tuple ( number 3 number 12 number 0 )").parseValuePayload();
    auto val_blk = Parser("block { push number 2 add push number 2 mul }").parseValuePayload();
    auto res_tpl = Parser("tuple ( number 10 number 28 number 4 )").parseValuePayload();
    s.stack().push(val_tpl);
    s.stack().push(val_blk);
    ASSERT_EQ(Operation::Result::SUCCESS, mp.execute(s));
    ASSERT_TRUE(res_tpl->equals(s.stack().peek()));
}

TEST(Map, TableBlock) {
    MachineState s;
    Map mp;
    auto val_tbl = Parser("table [ number 123 -> number 321, number 451 -> number 154, boolean false -> empty ]").parseValuePayload();
    auto val_blk = Parser("block { dup push number 0 at swap push number 1 at push number 2 pack }").parseValuePayload();
    auto res_tbl = Parser("table [ empty -> boolean false, number 321 -> number 123, number 154 -> number 451 ]").parseValuePayload();
    s.stack().push(val_tbl);
    s.stack().push(val_blk);
    ASSERT_EQ(Operation::Result::SUCCESS, mp.execute(s));
    ASSERT_TRUE(res_tbl->equals(s.stack().peek()));
}

TEST(Map, OperationTypeMismatch) {
    MachineState s;
    Map mp;
    auto val_tbl = Parser("table [ number 123 -> number 321, number 451 -> number 154, boolean false -> empty ]").parseValuePayload();
    auto val_op = Parser("operation push empty").parseValuePayload();
    s.stack().push(val_tbl);
    s.stack().push(val_op);
    ASSERT_EQ(Operation::Result::ERROR, mp.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
}

TEST(Map, BlockTypeMismatch) {
    MachineState s;
    Map mp;
    auto val_tbl = Parser("table [ number 123 -> number 321, number 451 -> number 154, boolean false -> empty ]").parseValuePayload();
    auto val_blk = Parser("block { push empty }").parseValuePayload();
    s.stack().push(val_tbl);
    s.stack().push(val_blk);
    ASSERT_EQ(Operation::Result::ERROR, mp.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
}
