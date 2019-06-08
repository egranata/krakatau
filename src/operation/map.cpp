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

#include <operation/map.h>
#include <rtti/rtti.h>
#include <value/tuple.h>
#include <value/number.h>
#include <value/string.h>
#include <value/table.h>
#include <value/operation.h>
#include <value/block.h>
#include <operation/block.h>
#include <value/boolean.h>
#include <machine/state.h>
#include <value/set.h>

Operation::Result Map::doExecute(MachineState& s) {
    if (!s.stack().hasAtLeast(2)) {
        s.stack().push(Value::error(ErrorCode::INSUFFICIENT_ARGUMENTS));
        return Operation::Result::ERROR;
    }

    auto vpred = s.stack().pop();
    auto vcnt = s.stack().pop();

    auto tpl = runtime_ptr_cast<Value_Tuple>(vcnt);
    auto tbl = runtime_ptr_cast<Value_Table>(vcnt);
    auto cbk = vpred->asClass<Value_Operation>();

    if(cbk == nullptr) {
        s.stack().push(vcnt);
        s.stack().push(vpred);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }

    if(tpl == nullptr && tbl == nullptr) {
        s.stack().push(vcnt);
        s.stack().push(vpred);
        s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
        return Operation::Result::ERROR;
    }

    if (tpl) {
        auto vtpl_result = Value::tuple({});
        auto tpl_result = runtime_ptr_cast<Value_Tuple>(vtpl_result);
        for(size_t i = 0; i < tpl->size(); ++i) {
            auto itm = tpl->at(i);
            s.stack().push(itm);
            auto ok = cbk->execute(s);
            if (ok != Operation::Result::SUCCESS) {
                s.stack().push(vcnt);
                s.stack().push(vpred);
                s.stack().push(Value::error(ErrorCode::UNEXPECTED_RESULT));
                return Operation::Result::ERROR;
            } else tpl_result->append(s.stack().pop());
        }
        s.stack().push(vtpl_result);
        return Operation::Result::SUCCESS;
    } else {
        auto vtbl_result = Value::table({});
        auto tbl_result = runtime_ptr_cast<Value_Table>(vtbl_result);
        for(size_t i = 0; i < tbl->size(); ++i) {
            auto itm = tbl->pairAt(i);
            s.stack().push(itm);
            auto ok = cbk->execute(s);
            if (ok != Operation::Result::SUCCESS) {
                s.stack().push(vcnt);
                s.stack().push(vpred);
                s.stack().push(Value::error(ErrorCode::UNEXPECTED_RESULT));
                return Operation::Result::ERROR;
            }
            auto vres = s.stack().pop();
            auto res = runtime_ptr_cast<Value_Tuple>(vres);
            if (res == nullptr || res->size() != 2) {
                s.stack().push(vcnt);
                s.stack().push(vpred);
                s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
                return Operation::Result::ERROR;
            } else tbl_result->append(res->at(0), res->at(1));
        }
        s.stack().push(vtbl_result);
        return Operation::Result::SUCCESS;
    }
}
