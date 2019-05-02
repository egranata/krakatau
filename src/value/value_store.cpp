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

#include <value/value_store.h>

ValueStore::ValueStore() = default;

bool ValueStore::store(const std::string& k, std::shared_ptr<Value> v, bool overwrite) {
    if (overwrite) {
        mStore.insert_or_assign(k, v);
        return true;
    } else {
        auto r = mStore.emplace(k,v);
        return r.second;
    }
}

std::shared_ptr<Value> ValueStore::retrieve(const std::string& k) {
    auto i = mStore.find(k), e = mStore.end();
    if (i == e) return nullptr;
    return i->second;
}

bool ValueStore::clear(const std::string& s) {
    return mStore.erase(s) > 0;
}

size_t ValueStore::serialize(Serializer* s) {
    size_t wr = 0;
    for (auto kv : mStore) {
        wr += s->writeIdentifier(kv.first);
        wr += kv.second->serialize(s);
    }

    return wr;
}

