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
#include <machine/state.h>
#include <value/boolean.h>
#include <value/findable.h>
#include <value/error.h>

Operation::Result Find::doExecute(MachineState& s) {
    auto key = s.stack().pop();
    auto obj = s.stack().pop();

    auto fnd = FindableValue::asFindable(obj);

    if (fnd && key) {
        const bool there = fnd->contains(key);

        if (!there) s.stack().push(Value::error(ErrorCode::NOT_FOUND));
        else s.stack().push(fnd->retrieve(key));

        return Operation::Result::SUCCESS;
    } else {
        s.stack().push(obj);
        s.stack().push(key);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }
}
