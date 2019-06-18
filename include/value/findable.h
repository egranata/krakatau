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

#ifndef STUFF_VALUE_FINDABLE
#define STUFF_VALUE_FINDABLE

#include <memory>

class Value;

class FindableValue {
    public:
        virtual bool contains(std::shared_ptr<Value>) const = 0;
        virtual std::shared_ptr<Value> retrieve(std::shared_ptr<Value>) const = 0;

        virtual ~FindableValue();

        template<typename T>
        static std::shared_ptr<FindableValue> asFindable(std::shared_ptr<T> val) {
            return std::dynamic_pointer_cast<FindableValue>(val);
        }

        std::shared_ptr<Value> asValue();
    protected:
        FindableValue();
};

#endif
