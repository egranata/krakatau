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

#include <operation/exec.h>
#include <rtti/rtti.h>
#include <value/boolean.h>
#include <error/error_codes.h>
#include <value/operation.h>
#include <operation/op.h>
#include <machine/state.h>

Operation::Result Exec::doExecute(MachineState& s) {
    auto val = s.stack().pop();

    if (auto oper = runtime_ptr_cast<Value_Operation>(val)) {
        auto ret = oper->execute(s);
        return ret;
    } else {
        s.stack().push(val);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }
}
