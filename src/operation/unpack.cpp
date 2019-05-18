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

#include <operation/unpack.h>
#include <value/number.h>
#include <value/tuple.h>
#include <rtti/rtti.h>
#include <machine/state.h>

Operation::Result Unpack::execute(MachineState& ms) {
    if (!ms.stack().hasAtLeast(1)) {
        ms.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto val_tpl = ms.stack().pop();
    auto tpl = runtime_ptr_cast<Value_Tuple>(val_tpl);
    if (tpl == nullptr) {
        ms.stack().push(val_tpl);
        ms.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }

    for(size_t i = 0; i < tpl->size(); ++i) {
        ms.stack().push(tpl->at(i));
    }

    return Operation::Result::SUCCESS;
}
