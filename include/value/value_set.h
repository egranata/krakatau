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

#ifndef STUFF_VALUE_VALUESET
#define STUFF_VALUE_VALUESET

#include <unordered_set>
#include <value/value.h>

class ValueSet {
    public:
        ValueSet();
        bool add(std::shared_ptr<Value>);
        bool find(std::shared_ptr<Value>) const;
        size_t size() const;
        std::shared_ptr<Value> at(size_t) const;

    private:
        struct ValueHasher {
            size_t operator()(const std::shared_ptr<Value>& val) const {
                return val->hash();
            }
        };
        struct ValueEquater {
            bool operator()(const std::shared_ptr<Value>& a, const std::shared_ptr<Value>& b) const {
                return a->equals(b);
            }
        };
        using Set = std::unordered_set<std::shared_ptr<Value>, ValueHasher, ValueEquater>;
        Set mSet;
};

#endif
