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
#include <operation/filter.h>
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
#include <value/empty.h>
#include <value/string.h>
#include <value/set.h>

TEST(Filter, ZeroArgs) {
    MachineState s;
    Filter flt;
    ASSERT_EQ(Operation::Result::ERROR, flt.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(0, s.stack().size());
}

TEST(Filter, OneArg) {
    MachineState s;
    Filter flt;
    s.stack().push(Value::tuple({}));
    ASSERT_EQ(Operation::Result::ERROR, flt.execute(s));
    ASSERT_EQ(ErrorCode::INSUFFICIENT_ARGUMENTS, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(1, s.stack().size());
}

TEST(Filter, MistypedArgs) {
    MachineState s;
    Filter flt;
    s.stack().push(Value::tuple({}));
    s.stack().push(Value::empty());
    ASSERT_EQ(Operation::Result::ERROR, flt.execute(s));
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_EQ(2, s.stack().size());
}

TEST(Filter, TupleFilterTakeAll) {
    MachineState s;
    Filter flt;
    auto vtpl = Value::tuple({});
    auto tpl = runtime_ptr_cast<Value_Tuple>(vtpl);
    tpl->append(Value::empty())->append(Value::fromNumber(123))->append(Value::fromBoolean(false));
    auto vop = Value::fromOperation(std::shared_ptr<Operation>(new Push(Value::fromBoolean(true))));
    s.stack().push(vtpl);
    s.stack().push(vop);
    ASSERT_EQ(Operation::Result::SUCCESS, flt.execute(s));
    ASSERT_TRUE(vtpl->equals(s.stack().pop()));
}

TEST(Filter, TupleFilterTakeNone) {
    MachineState s;
    Filter flt;
    auto vtpl = Value::tuple({});
    auto tpl = runtime_ptr_cast<Value_Tuple>(vtpl);
    tpl->append(Value::empty())->append(Value::fromNumber(123))->append(Value::fromBoolean(false));
    auto vop = Value::fromOperation(std::shared_ptr<Operation>(new Push(Value::fromBoolean(false))));
    s.stack().push(vtpl);
    s.stack().push(vop);
    ASSERT_EQ(Operation::Result::SUCCESS, flt.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Tuple>());
    ASSERT_EQ(0, runtime_ptr_cast<Value_Tuple>(s.stack().peek())->size());
}

TEST(Filter, TupleOpTypeMismatch) {
    MachineState s;
    Filter flt;
    auto vtpl = Value::tuple({});
    auto tpl = runtime_ptr_cast<Value_Tuple>(vtpl);
    tpl->append(Value::empty())->append(Value::fromNumber(123))->append(Value::fromBoolean(false));
    auto vop = Value::fromOperation(std::shared_ptr<Operation>(new Push(Value::empty())));
    s.stack().push(vtpl);
    s.stack().push(vop);
    ASSERT_EQ(Operation::Result::ERROR, flt.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Error>());
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_TRUE(vop->equals(s.stack().pop()));
    ASSERT_TRUE(vtpl->equals(s.stack().pop()));
}

TEST(Filter, TupleBlock) {
    MachineState s;
    Filter flt;
    auto vtpl = Value::tuple({});
    auto tpl = runtime_ptr_cast<Value_Tuple>(vtpl);
    tpl->append(Value::empty())->append(Value::fromNumber(123))->append(Value::fromBoolean(false));
    auto vblk = Parser("block { typeof push type number eq }").parseValuePayload();
    s.stack().push(vtpl);
    s.stack().push(vblk);
    ASSERT_EQ(Operation::Result::SUCCESS, flt.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Tuple>());
    ASSERT_EQ(1, runtime_ptr_cast<Value_Tuple>(s.stack().peek())->size());
    ASSERT_TRUE(runtime_ptr_cast<Value_Tuple>(s.stack().peek())->at(0)->isOfClass<Value_Number>());
}

TEST(Filter, TableFilterTakeAll) {
    MachineState s;
    Filter flt;
    auto vtbl = Value::table({});
    auto tbl = runtime_ptr_cast<Value_Table>(vtbl);
    tbl->append(Value::fromNumber(1), Value::fromString("one"))->append(Value::fromNumber(2), Value::fromString("two"));
    auto vop = Value::fromOperation(std::shared_ptr<Operation>(new Push(Value::fromBoolean(true))));
    s.stack().push(vtbl);
    s.stack().push(vop);
    ASSERT_EQ(Operation::Result::SUCCESS, flt.execute(s));
    ASSERT_TRUE(vtbl->equals(s.stack().pop()));
}

TEST(Filter, TableFilterTakeNone) {
    MachineState s;
    Filter flt;
    auto vtbl = Value::table({});
    auto tbl = runtime_ptr_cast<Value_Table>(vtbl);
    tbl->append(Value::fromNumber(1), Value::fromString("one"))->append(Value::fromNumber(2), Value::fromString("two"));
    auto vop = Value::fromOperation(std::shared_ptr<Operation>(new Push(Value::fromBoolean(false))));
    s.stack().push(vtbl);
    s.stack().push(vop);
    ASSERT_EQ(Operation::Result::SUCCESS, flt.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Table>());
    ASSERT_EQ(0, runtime_ptr_cast<Value_Table>(s.stack().peek())->size());
}

TEST(Filter, TableOpTypeMismatch) {
    MachineState s;
    Filter flt;
    auto vtbl = Value::table({});
    auto tbl = runtime_ptr_cast<Value_Table>(vtbl);
    tbl->append(Value::fromNumber(1), Value::fromString("one"))->append(Value::fromNumber(2), Value::fromString("two"));
    auto vop = Value::fromOperation(std::shared_ptr<Operation>(new Push(Value::empty())));
    s.stack().push(vtbl);
    s.stack().push(vop);
    ASSERT_EQ(Operation::Result::ERROR, flt.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Error>());
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(s.stack().pop())->value());
    ASSERT_TRUE(vop->equals(s.stack().pop()));
    ASSERT_TRUE(vtbl->equals(s.stack().pop()));
}

TEST(Filter, TableBlock) {
    MachineState s;
    Filter flt;
    auto vtbl = Value::table({});
    auto tbl = runtime_ptr_cast<Value_Table>(vtbl);
    tbl->append(Value::fromNumber(1), Value::fromString("one"))->append(Value::fromNumber(2), Value::fromString("two"));
    auto vblk = Parser("block { push number 0 at push number 2 eq }").parseValuePayload();
    s.stack().push(vtbl);
    s.stack().push(vblk);
    ASSERT_EQ(Operation::Result::SUCCESS, flt.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Table>());
    ASSERT_EQ(1, runtime_ptr_cast<Value_Table>(s.stack().peek())->size());
    ASSERT_TRUE(Value::fromNumber(2)->equals(runtime_ptr_cast<Value_Table>(s.stack().peek())->keyAt(0)));
    ASSERT_TRUE(Value::fromString("two")->equals(runtime_ptr_cast<Value_Table>(s.stack().peek())->valueAt(0)));
}

TEST(Filter, StringTakeL) {
    MachineState s;
    Filter flt;
    auto vstr = Value::fromString("hello world");
    auto vblk = Parser("block { push character 108 eq }").parseValuePayload();
    s.stack().push(vstr);
    s.stack().push(vblk);
    ASSERT_EQ(Operation::Result::SUCCESS, flt.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_String>());
    ASSERT_TRUE(s.stack().peek()->equals(Value::fromString("lll")));
}

TEST(Filter, Set) {
    MachineState s;
    Filter flt;
    auto vset = Value::set( {Value::fromNumber(1), Value::fromNumber(2), Value::fromNumber(3)} );
    auto vblk = Parser("block { push number 2 eq not }").parseValuePayload();
    s.stack().push(vset);
    s.stack().push(vblk);
    ASSERT_EQ(Operation::Result::SUCCESS, flt.execute(s));
    ASSERT_TRUE(s.stack().peek()->isOfClass<Value_Set>());
    ASSERT_EQ(2, s.stack().peek()->asClass<Value_Set>()->size());
    ASSERT_EQ(3, vset->size());
}
