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

#include <value/empty.h>
#include <rtti/rtti.h>
#include <stream/serializer.h>
#include <parser/parser.h>

Value_Empty::Value_Empty() = default;

std::string Value_Empty::describe() const {
    return "empty";
}

bool Value_Empty::equals(std::shared_ptr<Value> v) const {
    auto emp = runtime_ptr_cast<Value_Empty>(v);
    if (emp == nullptr) return false;

    return true;
}

std::shared_ptr<Value> Value_Empty::fromByteStream(ByteStream*) {
    return Value::empty();
}

size_t Value_Empty::serialize(Serializer* s) {
    return s->writeNumber(MARKER, 1);
}

std::shared_ptr<Value> Value_Empty::fromParser(Parser*) {
    return Value::empty();
}

size_t Value_Empty::hash() const {
    return 0xE101;
}

std::shared_ptr<Value> Value_Empty::clone() const {
    return Value::empty();
}
