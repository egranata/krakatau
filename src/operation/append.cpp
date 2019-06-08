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

#include <operation/append.h>
#include <rtti/rtti.h>
#include <value/tuple.h>
#include <value/table.h>
#include <value/appendable.h>
#include <machine/state.h>

Operation::Result Append::doExecute(MachineState& s) {
    if (!s.stack().hasAtLeast(2)) {
        s.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto item = s.stack().pop();
    auto container = s.stack().pop();
    auto cloned = container->clone();

    if (auto app = Appendable::asAppendable(cloned)) {
        auto ret = app->appendValue(item);
        if (auto err = std::get_if<ErrorCode>(&ret)) {
            s.stack().push(container);
            s.stack().push(item);
            s.stack().push(Value::error(*err));
            return Operation::Result::ERROR;
        } else if (auto val = std::get_if<Value*>(&ret)) {
            s.stack().push((*val)->shared_from_this());
            return Operation::Result::SUCCESS;
        } else {
            s.stack().push(container);
            s.stack().push(item);
            s.stack().push(Value::error(ErrorCode::NOT_IMPLEMENTED));
            return Operation::Result::ERROR;
        }
    } else {
            s.stack().push(container);
            s.stack().push(item);
            s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
            return Operation::Result::ERROR;
    }
}
