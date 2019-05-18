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

#include <operation/at.h>
#include <rtti/rtti.h>
#include <value/tuple.h>
#include <value/number.h>
#include <value/string.h>
#include <value/table.h>
#include <machine/state.h>

Operation::Result At::execute(MachineState& s) {
    if (!s.stack().hasAtLeast(2)) {
        s.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto nval = s.stack().pop();
    auto obj = s.stack().pop();

    auto n = runtime_ptr_cast<Value_Number>(nval);
    auto tpl = runtime_ptr_cast<Value_Tuple>(obj);
    auto str = runtime_ptr_cast<Value_String>(obj);
    auto tbl = runtime_ptr_cast<Value_Table>(obj);

    if (n && tpl) {
        if (n->value() >= tpl->size()) {
            s.stack().push(obj);
            s.stack().push(nval);
            s.stack().push(Value::error(ErrorCode::OUT_OF_BOUNDS));
            return Operation::Result::ERROR;
        }
        s.stack().push(tpl->at(n->value()));
        return Operation::Result::SUCCESS;
    }

    if (n && str) {
        if (n->value() >= str->value().size()) {
            s.stack().push(obj);
            s.stack().push(nval);
            s.stack().push(Value::error(ErrorCode::OUT_OF_BOUNDS));
            return Operation::Result::ERROR;
        }
        auto ch = str->value().at(n->value());
        s.stack().push(Value::fromNumber(ch));
        return Operation::Result::SUCCESS;
    }

    if (tbl) {
        s.stack().push(tbl->find(nval));
        return Operation::Result::SUCCESS;
    }

    s.stack().push(obj);
    s.stack().push(nval);
    s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
    return Operation::Result::ERROR;
}
