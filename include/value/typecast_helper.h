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

#ifndef STUFF_VALUE_TYPECASTHELPER
#define STUFF_VALUE_TYPECASTHELPER

#include <value/value.h>
#include <functional>
#include <memory>
#include <unordered_map>

template<typename T>
class TypecastHelper {
    public:
        using TypecastFunction = std::function<std::shared_ptr<Value>(T*)>;

        TypecastHelper() : mDefault([] (T*) -> std::shared_ptr<Value> {
            return nullptr;
        }) {}

        std::shared_ptr<Value> doTypecast(T* self, ValueType vt) const {
            auto tf = mConversions.find(vt), e = mConversions.end();
            if (tf == e) return mDefault(self);
            else return tf->second(self);
        }
        TypecastHelper& onType(ValueType vt, TypecastFunction f) {
            mConversions.emplace(vt, f);
            return *this;
        }
        TypecastHelper& orElse(TypecastFunction f) {
            mDefault = f;
            return *this;
        }

    private:
        std::unordered_map<ValueType, TypecastFunction> mConversions;
        TypecastFunction mDefault;
};

#endif
