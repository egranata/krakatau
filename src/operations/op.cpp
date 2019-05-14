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

#include <operations/op.h>
#include <stream/serializer.h>
#include <rtti/enum.h>

Operation::Operation() = default;
Operation::~Operation() = default;

bool Operation::isOfType(OperationType vt) const {
    return (vt == OperationType::NONE);
}

size_t Operation::serialize(Serializer* s) const {
    return s->writeNumber(enumToNumber(getClassId()), sizeof(std::underlying_type<OperationType>::type));
}

bool Operation::equals(std::shared_ptr<Operation> rhs) const {
    return getClassId() == rhs->getClassId();
}

std::string operationResultToString(Operation::Result r) {
    switch (r) {
        case Operation::Result::ERROR: return "error";
        case Operation::Result::SUCCESS: return "success";
        case Operation::Result::RESTART_BLOCK: return "restart";
        case Operation::Result::EXIT_BLOCK: return "exit";
        case Operation::Result::AGAIN: return "again";
        default: return "unknown";
    }
}

std::ostream& operator<<(std::ostream& os, std::shared_ptr<Operation> sp) {
    return (os << sp.get());
}
std::ostream& operator<<(std::ostream& os, Operation* v) {
    if (v) return (os << v->describe());
    return (os << "(null) operation");
}
