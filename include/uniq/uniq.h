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

#ifndef STUFF_UNIQ_UNIQ
#define STUFF_UNIQ_UNIQ

#include <unordered_map>
#include <memory>

template<typename ValueType>
class Uniq {
    private:
        using VT = std::shared_ptr<ValueType>;

    public:
        Uniq() = default;

        VT add(const ValueType& v) {
            VT sp(nullptr);

            auto i = mUniques.find(v), e = mUniques.end();
            if (i == e) mUniques.emplace(v, sp = std::make_shared<ValueType>(v));
            else sp = i->second;

            return sp;
        }

        size_t size() const {
            return mUniques.size();
        }

        VT find(const ValueType& v) {
            VT sp(nullptr);

            auto i = mUniques.find(v), e = mUniques.end();
            if (i != e) sp = i->second;

            return sp;
        }

    private:
        using SelfType = Uniq<ValueType>;

        std::unordered_map<ValueType, VT> mUniques;

        Uniq(const SelfType&) = delete;
        SelfType& operator=(const SelfType&) = delete;
};

#endif
