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

#include <value/iterable.h>
#include <value/value.h>
#include <value/iterator.h>
#include <type_traits>

IterableValue::IterableValue() {
    using SelfType = std::remove_reference<decltype(*this)>::type;
    using IsAbstract = std::is_abstract<SelfType>;
    using IsValue = std::is_base_of<Value, SelfType>;

    using IsValid = std::disjunction<IsAbstract, IsValue>;

    static_assert(IsValid::value);
}

IterableValue::~IterableValue() = default;

bool IterableValue::equals(std::shared_ptr<IterableValue> rhs) const {
    auto vt = dynamic_cast<const Value*>(this);
    auto vr = std::dynamic_pointer_cast<Value>(rhs);
    return vt->equals(vr);
}

ValueIterator IterableValue::begin() {
    return ValueIterator(this, 0);
}
ValueIterator IterableValue::end() {
    return ValueIterator(this, size());
}

std::shared_ptr<Value> IterableValue::asValue() {
    return dynamic_cast<Value*>(this)->shared_from_this();
}
