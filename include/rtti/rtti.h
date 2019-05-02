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

#ifndef STUFF_RTTI_RTTI
#define STUFF_RTTI_RTTI

#include <type_traits>
#include <memory>

template<typename T, typename BaseType>
T* runtime_ptr_cast(const std::shared_ptr<BaseType>& p) {
    return runtime_ptr_cast<T>(p.get());
}

template <typename T, typename BaseType>
T* runtime_ptr_cast(BaseType* p) {
    if (p == nullptr) return nullptr;

    if (p->template isOfClass<T>())
        return static_cast<T*>(p);
    else
      return nullptr;
};

#endif
