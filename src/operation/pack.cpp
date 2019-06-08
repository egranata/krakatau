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

#include <operation/pack.h>
#include <value/number.h>
#include <value/tuple.h>
#include <rtti/rtti.h>
#include <machine/state.h>

Operation::Result Pack::doExecute(MachineState& ms) {
    auto val_cnt = ms.stack().pop();
    auto num_cnt = runtime_ptr_cast<Value_Number>(val_cnt);
    if (num_cnt == nullptr) {
        ms.stack().push(val_cnt);
        ms.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }
    size_t cnt = num_cnt->value();

    if (!ms.stack().hasAtLeast(cnt)) {
        ms.stack().push(val_cnt);
        ms.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto valtpl = Value::tuple({});
    auto tpl = runtime_ptr_cast<Value_Tuple>(valtpl);
    for(size_t i = 0; i < cnt; ++i) {
        tpl->append(ms.stack().pop());
    }

    ms.stack().push(valtpl);
    return Operation::Result::SUCCESS;
}
