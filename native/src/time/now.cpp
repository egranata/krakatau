/*
 * Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <time/now.h>
#include <time.h>
#include <value/value.h>
#include <value/number.h>
#include <machine/state.h>

Now::Now(std::shared_ptr<NativeOperations::Bucket> b) : Native(b, "now") {}

Operation::Result Now::execute(MachineState& ms) {
    ms.stack().push(Value::fromNumber(::time(nullptr)));
    return Operation::Result::SUCCESS;
}

std::shared_ptr<Operation> Now::clone() const {
    return std::make_shared<Now>(bucket());
}
