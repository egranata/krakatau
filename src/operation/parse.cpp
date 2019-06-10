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

#include <operation/parse.h>
#include <parser/parser.h>
#include <rtti/rtti.h>
#include <error/error_codes.h>
#include <value/string.h>
#include <operation/op.h>
#include <machine/state.h>

Operation::Result Parse::doExecute(MachineState& s) {
    auto val = s.stack().pop();
    Value_String* str = runtime_ptr_cast<Value_String>(val);

    if (str) {
        Parser p(str->utf8());
        auto res = p.parseValuePayload();
        if (!res) {
            s.stack().push(val);
            s.stack().push(Value::error(ErrorCode::SYNTAX_ERROR));
            return Operation::Result::ERROR;
        } else {
            s.stack().push(res);
            return Operation::Result::SUCCESS;
        }
    }

    s.stack().push(val);
    s.stack().push(Value::error(ErrorCode::TYPE_MISMATCH));
    return Operation::Result::ERROR;
}
