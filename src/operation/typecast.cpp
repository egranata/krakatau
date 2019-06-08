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

#include <operation/typecast.h>
#include <rtti/rtti.h>
#include <value/tuple.h>
#include <value/number.h>
#include <value/string.h>
#include <value/type.h>
#include <machine/state.h>

Operation::Result Typecast::doExecute(MachineState& s) {
    auto tyval = s.stack().pop();
    auto obj = s.stack().pop();

    auto ty = runtime_ptr_cast<Value_Type>(tyval);
    if (ty == nullptr) {
        s.stack().push(obj);
        s.stack().push(tyval);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }

    auto cv = obj->typecast(ty->value());
    if (cv == nullptr) {
        s.stack().push(obj);
        s.stack().push(tyval);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    } else {
        s.stack().push(cv);
        return Operation::Result::SUCCESS;
    }
}
