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

#include <value/value_set.h>
#include <iterator>
#include <value/tuple.h>
#include <rtti/rtti.h>

ValueSet::ValueSet() = default;

bool ValueSet::add(std::shared_ptr<Value> v) {
    return mSet.emplace(v).second;
}

bool ValueSet::find(std::shared_ptr<Value> k) const {
    return mSet.count(k) == 1;
}

size_t ValueSet::size() const {
    return mSet.size();
}

std::shared_ptr<Value> ValueSet::at(size_t n) const {
    if (n >= size()) return nullptr;
    auto iter = mSet.begin();
    std::advance(iter, n);
    return *iter;
}
