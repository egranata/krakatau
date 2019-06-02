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

#include <operation/find.h>
#include <rtti/rtti.h>
#include <value/table.h>
#include <value/set.h>
#include <machine/state.h>
#include <value/empty.h>
#include <value/boolean.h>

Operation::Result Find::execute(MachineState& s) {
    if (!s.stack().hasAtLeast(2)) {
        s.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto key = s.stack().pop();
    auto obj = s.stack().pop();

    auto tbl = runtime_ptr_cast<Value_Table>(obj);
    auto set = runtime_ptr_cast<Value_Set>(obj);

    if (key && tbl) {
        if (auto val = tbl->find(key, nullptr)) s.stack().push(val);
        else s.stack().push(Value::error(ErrorCode::NOT_FOUND));

        return Operation::Result::SUCCESS;
    } else if (key && set) {
        if (set->find(key)) s.stack().push(Value::fromBoolean(true));
        else s.stack().push(Value::error(ErrorCode::NOT_FOUND));

        return Operation::Result::SUCCESS;
    } else {
        s.stack().push(obj);
        s.stack().push(key);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }
}
