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

#include <stack/stack.h>
#include <stream/indenting_stream.h>

Stack::Stack() = default;

bool Stack::empty() const {
    return mValues.empty();
}

void Stack::push(std::shared_ptr<Value> v) {
    mValues.push(v);
}

std::shared_ptr<Value> Stack::peek() const {
    if (mValues.size()) return mValues.top();
    return nullptr;
}

std::shared_ptr<Value> Stack::pop() {
    if (empty()) return nullptr;
    auto sp = peek();
    return mValues.pop(), sp;
}

size_t Stack::size() const {
    return mValues.size();
}

bool Stack::hasAtLeast(size_t n) const {
    return size() >= n;
}

void Stack::reset() {
    while(!mValues.empty()) mValues.pop();
}

std::string Stack::describe() {
    IndentingStream is;
    std::stack<std::shared_ptr<Value>> other(mValues);
    bool first = true;
    while(other.size()) {
        auto item = other.top();
        if (first) {
            is.append("%s", item->describe().c_str());
            first = false;
        } else {
            is.append("\n%s", item->describe().c_str());
        }
        other.pop();
    }
    return is.str();
}
