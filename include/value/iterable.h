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

#ifndef STUFF_VALUE_ITERABLE
#define STUFF_VALUE_ITERABLE

#include <stdint.h>
#include <memory>

class ValueIterator;
class Value;

class IterableValue {
    public:
        virtual size_t size() const = 0;
        virtual std::shared_ptr<Value> at(size_t) const = 0;
        virtual ~IterableValue();

        ValueIterator begin();
        ValueIterator end();

        bool equals(std::shared_ptr<IterableValue>) const;

        template<typename T>
        static std::shared_ptr<IterableValue> asIterable(std::shared_ptr<T> val) {
            return std::dynamic_pointer_cast<IterableValue>(val);
        }

        std::shared_ptr<Value> asValue();
    protected:
        IterableValue();
};

#endif
