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

#include <operation/filter.h>
#include <rtti/rtti.h>
#include <value/operation.h>
#include <value/boolean.h>
#include <value/appendable.h>
#include <value/iterable.h>
#include <machine/state.h>

Operation::Result Filter::doExecute(MachineState& s) {
    auto vpred = s.stack().pop();
    auto vcnt = s.stack().pop();

    auto cbk = vpred->asClass<Value_Operation>();

    if(cbk == nullptr) {
        s.stack().push(vcnt);
        s.stack().push(vpred);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }

    auto iter = IterableValue::asIterable(vcnt);

    if(iter == nullptr) {
        s.stack().push(vcnt);
        s.stack().push(vpred);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }

    auto newval = Appendable::asAppendable(iter->asValue())->newEmptyOfSameType();
    auto newapp = Appendable::asAppendable(newval);

    if (newapp == nullptr) {
        s.stack().push(vcnt);
        s.stack().push(vpred);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }

    for(size_t i = 0; i < iter->size(); ++i) {
        auto itm = iter->at(i);
        s.stack().push(itm);
        auto ok = cbk->execute(s);
        if (ok != Operation::Result::SUCCESS) {
            s.stack().push(vcnt);
            s.stack().push(vpred);
            s.stack().push(Value::error(ErrorCode::UNEXPECTED_RESULT));
            return Operation::Result::ERROR;
        }
        auto res = runtime_ptr_cast<Value_Boolean>(s.stack().pop());
        if (res == nullptr) {
            s.stack().push(vcnt);
            s.stack().push(vpred);
            s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
            return Operation::Result::ERROR;
        } else if (res->value()) newapp->appendValue(itm);
    }

    s.stack().push(newval);
    return Operation::Result::SUCCESS;
}
