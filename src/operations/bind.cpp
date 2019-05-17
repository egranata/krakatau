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

#include <operations/bind.h>
#include <rtti/rtti.h>
#include <value/tuple.h>
#include <function/bind.h>
#include <value/bind.h>
#include <machine/state.h>
#include <value/operation.h>

Operation::Result Bind::execute(MachineState& s) {
    if (!s.stack().hasAtLeast(2)) {
        s.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto val = s.stack().pop();
    auto cvl = s.stack().pop();
    auto clb = cvl->asClass<Value_Operation>();

    if (val && clb) {
        s.stack().push(Value::fromBind(std::make_shared<PartialBind>(val, clb->value())));
        return Operation::Result::SUCCESS;
    } else {
        s.stack().push(cvl);
        s.stack().push(val);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }
}
