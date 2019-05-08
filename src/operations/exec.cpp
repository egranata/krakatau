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

#include <operations/exec.h>
#include <function/block.h>
#include <rtti/rtti.h>
#include <value/block.h>
#include <value/boolean.h>
#include <error/error_codes.h>
#include <value/operation.h>
#include <operations/op.h>
#include <machine/state.h>
#include <value/bind.h>
#include <function/bind.h>

Operation::Result Exec::execute(MachineState& s) {
    if (!s.stack().hasAtLeast(1)) {
        s.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto val = s.stack().pop();

    Value_Block* blk = runtime_ptr_cast<Value_Block>(val);
    Value_Operation* oper = runtime_ptr_cast<Value_Operation>(val);
    Value_Bind* bind = runtime_ptr_cast<Value_Bind>(val);

    if (oper) {
        auto ret = oper->execute(s);
        return ret;
    }
    if (blk) {
        auto ret = blk->execute(s);
        return ret;
    }
    if (bind) {
        auto ret = bind->execute(s);
        return ret;
    }

    s.stack().push(val);
    s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
    return Operation::Result::ERROR;
}
