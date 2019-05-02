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

#ifndef STUFF_RTTI_ENUM
#define STUFF_RTTI_ENUM

#include <type_traits>
#include <limits>

#define NUMERIC_LIMITS(Ty) \
namespace std { \
    template<> \
    struct numeric_limits<Ty> { \
        static constexpr Ty min() noexcept { return Ty ::MIN_VALUE; } \
        static constexpr Ty max() noexcept { return Ty ::MAX_VALUE; } \
    }; \
}

template<typename T, typename U = typename std::underlying_type<T>::type>
static constexpr U enumToNumber(T value) {
    return (U)value;
}

template<typename T, typename U = typename std::underlying_type<T>::type>
static constexpr bool enumInRange(U value) {
    if (value < enumToNumber(std::numeric_limits<T>::min())) return false;
    if (value > enumToNumber(std::numeric_limits<T>::max())) return false;

    return true;
}

#endif
