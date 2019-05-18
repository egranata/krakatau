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

#include <operation/append.h>
#include <rtti/rtti.h>
#include <value/tuple.h>
#include <value/table.h>
#include <machine/state.h>

Operation::Result Append::execute(MachineState& s) {
    if (!s.stack().hasAtLeast(2)) {
        s.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto item = s.stack().pop();
    auto container = s.stack().pop();

    auto tpl = runtime_ptr_cast<Value_Tuple>(container);
    auto tbl = runtime_ptr_cast<Value_Table>(container);

    if (tbl) {
        auto item_tpl = runtime_ptr_cast<Value_Tuple>(item);
        if (item_tpl && item_tpl->size() == 2) {
            container = container->clone();
            tbl = runtime_ptr_cast<Value_Table>(container);
            tbl->append(item_tpl->at(0), item_tpl->at(1));
            s.stack().push(container);
            return Operation::Result::SUCCESS;
        } else {
            s.stack().push(container);
            s.stack().push(item);
            s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
            return Operation::Result::ERROR;
        }
    } else if (tpl) {
        container = container->clone();
        tpl = runtime_ptr_cast<Value_Tuple>(container);
        tpl->append(item);
        s.stack().push(container);
        return Operation::Result::SUCCESS;
    }

    s.stack().push(container);
    s.stack().push(item);
    s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
    return Operation::Result::ERROR;
}
