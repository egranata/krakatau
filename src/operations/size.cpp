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

#include <operations/size.h>
#include <rtti/rtti.h>
#include <value/tuple.h>
#include <value/string.h>
#include <value/table.h>
#include <machine/state.h>

Operation::Result Size::execute(MachineState& s) {
    if (!s.stack().hasAtLeast(1)) {
        s.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto value = s.stack().pop();
    auto tpl = runtime_ptr_cast<Value_Tuple>(value);
    auto str = runtime_ptr_cast<Value_String>(value);
    auto tbl = runtime_ptr_cast<Value_Table>(value);
    if (tpl) {
        s.stack().push(Value::fromNumber(tpl->size()));
    } else if (str) {
        s.stack().push(Value::fromNumber(str->value().size()));
    } else if (tbl) {
        s.stack().push(Value::fromNumber(tbl->size()));
    } else {
        s.stack().push(Value::fromNumber(1));
    }

    return Operation::Result::SUCCESS;
}
