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

#include <value/iterator.h>
#include <value/value.h>

ValueIterator::ValueIterator(IterableValue* v, size_t i) : mValue(v), mIndex(i) {
    if (mIndex > mValue->size()) mIndex = mValue->size();
}

ValueIterator& ValueIterator::operator++() {
    if(mIndex < mValue->size()) ++mIndex;
    return *this;
}
ValueIterator& ValueIterator::operator--() {
    if (mIndex) --mIndex;
    return *this;
}
ValueIterator ValueIterator::operator--(int) {
    return ValueIterator(mValue, mIndex-1);
}

std::shared_ptr<Value> ValueIterator::operator*() const {
    return mValue->at(mIndex);
}
std::shared_ptr<Value> ValueIterator::operator->() const {
    return mValue->at(mIndex);
}

bool ValueIterator::operator==(const ValueIterator& rhs) const {
    return mIndex == rhs.mIndex &&
           dynamic_cast<Value*>(mValue)->equals(dynamic_cast<Value*>(rhs.mValue)->shared_from_this());
}
bool ValueIterator::operator!=(const ValueIterator& rhs) const {
    return mIndex != rhs.mIndex ||
           !dynamic_cast<Value*>(mValue)->equals(dynamic_cast<Value*>(rhs.mValue)->shared_from_this());
}
