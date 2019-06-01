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

#ifndef STUFF_VALUE_ITERATOR
#define STUFF_VALUE_ITERATOR

#include <value/iterable.h>

class ValueIterator {
    public:
        ValueIterator(IterableValue*, size_t = 0);
        ValueIterator& operator++();
        ValueIterator& operator--();
        ValueIterator operator--(int);
        std::shared_ptr<Value> operator*() const;
        std::shared_ptr<Value> operator->() const;
        bool operator==(const ValueIterator&) const;
        bool operator!=(const ValueIterator&) const;

    private:
        IterableValue* mValue;
        size_t mIndex;
};

#endif
