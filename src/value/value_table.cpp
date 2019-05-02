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

#include <value/value_table.h>
#include <iterator>
#include <value/tuple.h>
#include <rtti/rtti.h>

ValueTable::ValueTable() = default;

void ValueTable::add(std::shared_ptr<Value> k, std::shared_ptr<Value> v) {
    mMap.try_emplace(k, v);
}

std::shared_ptr<Value> ValueTable::find(std::shared_ptr<Value> k) const {
    auto i = mMap.find(k), e = mMap.end();
    if (i == e) return nullptr;
    return i->second;
}

size_t ValueTable::size() const {
    return mMap.size();
}

std::shared_ptr<Value> ValueTable::keyAt(size_t n) const {
    if (n >= size()) return nullptr;
    auto iter = mMap.begin();
    std::advance(iter, n);
    return iter->first;
}

std::shared_ptr<Value> ValueTable::valueAt(size_t n) const {
    if (n >= size()) return nullptr;
    auto iter = mMap.begin();
    std::advance(iter, n);
    return iter->second;
}

std::shared_ptr<Value> ValueTable::at(size_t n) const {
    if (n >= size()) return nullptr;
    auto k = keyAt(n);
    auto v = valueAt(n);
    auto vtpl = Value::tuple();
    auto tpl = runtime_ptr_cast<Value_Tuple>(vtpl);
    tpl->append(k);
    tpl->append(v);
    return vtpl;
}

