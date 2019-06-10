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
#include <value/iterable.h>
#include <machine/state.h>
#include <value/number.h>

Operation::Result At::doExecute(MachineState& s) {
    auto nval = s.stack().pop();
    auto obj = s.stack().pop();

    auto n = runtime_ptr_cast<Value_Number>(nval);

    auto iter = IterableValue::asIterable(obj);

    if (n && iter) {
        if (n->value() >= iter->size()) {
            s.stack().push(obj);
            s.stack().push(nval);
            s.stack().push(Value::error(ErrorCode::OUT_OF_BOUNDS));
            return Operation::Result::ERROR;
        }
        s.stack().push(iter->at(n->value()));
        return Operation::Result::SUCCESS;
    } else {
        s.stack().push(obj);
        s.stack().push(nval);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }
}
