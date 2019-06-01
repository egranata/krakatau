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
#include <operation/typecast.h>
#include <value/value.h>
#include <value/string.h>
#include <rtti/rtti.h>
#include <parser/parser.h>
#include <value/boolean.h>
#include <value/error.h>
#include <value/type.h>
#include <gtest/gtest.h>
#include <value/tuple.h>
#include <value/block.h>
#include <value/number.h>
#include <value/table.h>
#include <value/empty.h>

TEST(Typecast, BooleanToString) {
    Parser p("value main block { push boolean true push type string typecast }");
    MachineState ms;
    ms.load(&p);
    ms.execute();
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_String>());
    ASSERT_EQ(U"true", runtime_ptr_cast<Value_String>(ms.stack().peek())->value());
}

TEST(Typecast, BooleanToSelf) {
    Parser p("value main block { push boolean true push type boolean typecast }");
    MachineState ms;
    ms.load(&p);
    ms.execute();
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Boolean>());
    ASSERT_EQ(true, runtime_ptr_cast<Value_Boolean>(ms.stack().peek())->value());
}

TEST(Typecast, InvalidType) {
    Parser p("value main block { push operation at push type error typecast }");
    MachineState ms;
    ms.load(&p);
    ms.execute();
    ASSERT_EQ(3, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Error>());
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(ms.stack().peek())->value());
}

TEST(Typecast, TupleToBlock) {
    Parser p("value main block { push tuple (operation push number 123, operation dup,), push type block, typecast, exec }");
    MachineState ms;
    ms.load(&p);
    ms.execute();
    ASSERT_EQ(2, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(123, runtime_ptr_cast<Value_Number>(ms.stack().peek())->value());
    ms.stack().pop();
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Number>());
    ASSERT_EQ(123, runtime_ptr_cast<Value_Number>(ms.stack().peek())->value());
}

TEST(Typecast, TupleToBoolean) {
    Parser p("value main block { push tuple (operation push number 123, operation dup,), push type boolean, typecast }");
    MachineState ms;
    ms.load(&p);
    ms.execute();
    ASSERT_EQ(3, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Error>());
    ASSERT_EQ(ErrorCode::TYPE_MISMATCH, runtime_ptr_cast<Value_Error>(ms.stack().peek())->value());
    ms.stack().pop();
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Type>());
    ms.stack().pop();
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Tuple>());
}

TEST(Typecast, TableToTuple) {
    Parser p("value main block { push table [ number 1 -> boolean true, number 2 -> boolean false, ] push type tuple typecast }");
    MachineState ms;
    ms.load(&p);
    ms.execute();
    ASSERT_EQ(1, ms.stack().size());
    auto tpl = ms.stack().peek()->asClass<Value_Tuple>();
    ASSERT_NE(nullptr, tpl);
    ASSERT_EQ(2, tpl->size());
    ASSERT_TRUE(tpl->at(0)->isOfClass<Value_Tuple>());
    ASSERT_TRUE(tpl->at(1)->isOfClass<Value_Tuple>());
    ASSERT_EQ(2, tpl->at(0)->asClass<Value_Tuple>()->size());
    ASSERT_EQ(2, tpl->at(1)->asClass<Value_Tuple>()->size());
    auto t0 = tpl->at(0)->asClass<Value_Tuple>();
    auto t1 = tpl->at(0)->asClass<Value_Tuple>();
    ASSERT_TRUE(t0->at(0)->isOfClass<Value_Number>());
    ASSERT_TRUE(t1->at(0)->isOfClass<Value_Number>());
    ASSERT_TRUE(t0->at(1)->isOfClass<Value_Boolean>());
    ASSERT_TRUE(t1->at(1)->isOfClass<Value_Boolean>());
}

TEST(Typecast, InvalidTupleToTable) {
    Typecast tc;
    auto vtpl = Value::tuple({});
    auto tpl = runtime_ptr_cast<Value_Tuple>(vtpl);
    tpl->append(Value::fromNumber(123));
    tpl->append(Value::empty());
    MachineState ms;
    ms.stack().push(vtpl);
    ms.stack().push(Value::type(ValueType::TABLE));
    ASSERT_EQ(Operation::Result::ERROR, tc.execute(ms));
    vtpl = Value::tuple({});
    tpl = runtime_ptr_cast<Value_Tuple>(vtpl);
    tpl->append(Parser("tuple (number 123, boolean true,)").parseValuePayload());
    tpl->append(Parser("tuple (boolean false,empty,empty,)").parseValuePayload());
    ms.stack().push(vtpl);
    ms.stack().push(Value::type(ValueType::TABLE));
    ASSERT_EQ(Operation::Result::ERROR, tc.execute(ms));
}

TEST(Typecast, EmptyTupleToTable) {
    Typecast tc;
    auto vtpl = Value::tuple({});
    MachineState ms;
    ms.stack().push(vtpl);
    ms.stack().push(Value::type(ValueType::TABLE));
    ASSERT_EQ(Operation::Result::SUCCESS, tc.execute(ms));
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Table>());
    ASSERT_EQ(0, ms.stack().peek()->asClass<Value_Table>()->size());
}

TEST(Typecast, TupleToTable) {
    Parser p("value main block { push tuple (tuple (number 1, string \"one\"), tuple (number 2, string \"two\"), tuple (number 3, string \"three\"), )" +
                                 std::string(" push type table") +
                                 " typecast }");
    MachineState ms;
    ms.load(&p);
    ms.execute();
    ASSERT_EQ(1, ms.stack().size());
    ASSERT_TRUE(ms.stack().peek()->isOfClass<Value_Table>());
    ASSERT_EQ(3, ms.stack().peek()->asClass<Value_Table>()->size());
    ASSERT_TRUE(ms.stack().peek()->asClass<Value_Table>()->keyAt(0)->isOfClass<Value_Number>());
    ASSERT_TRUE(ms.stack().peek()->asClass<Value_Table>()->keyAt(1)->isOfClass<Value_Number>());
    ASSERT_TRUE(ms.stack().peek()->asClass<Value_Table>()->keyAt(2)->isOfClass<Value_Number>());
    ASSERT_TRUE(ms.stack().peek()->asClass<Value_Table>()->valueAt(0)->isOfClass<Value_String>());
    ASSERT_TRUE(ms.stack().peek()->asClass<Value_Table>()->valueAt(1)->isOfClass<Value_String>());
    ASSERT_TRUE(ms.stack().peek()->asClass<Value_Table>()->valueAt(2)->isOfClass<Value_String>());
}
