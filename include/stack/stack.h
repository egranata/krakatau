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

#ifndef STUFF_STACK_STACK
#define STUFF_STACK_STACK

#include <value/value.h>

#include <stack>
#include <memory>

class Stack {
    public:
        Stack();
        bool empty() const;
        void push(std::shared_ptr<Value>);
        std::shared_ptr<Value> peek() const;
        std::shared_ptr<Value> pop();
        size_t size() const;
        bool hasAtLeast(size_t) const;
        void reset();
        std::string describe();
    private:
        std::stack<std::shared_ptr<Value>> mValues;
};

#endif
