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

#ifndef STUFF_VALUE_VALUESTORE
#define STUFF_VALUE_VALUESTORE

#include <value/value.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <stream/serializer.h>

class ValueStore {
    public:
        ValueStore();
        bool store(const std::string&, std::shared_ptr<Value>, bool overwrite = false);
        std::shared_ptr<Value> retrieve(const std::string&);
        bool clear(const std::string&);
        size_t serialize(Serializer*);

    private:
        ValueStore(const ValueStore&) = delete;
        ValueStore& operator=(const ValueStore&) = delete;
        std::unordered_map<std::string, std::shared_ptr<Value>> mStore;
};

#endif
