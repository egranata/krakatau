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

#ifndef STUFF_VALUE_APPENDABLE
#define STUFF_VALUE_APPENDABLE

#include <stdint.h>
#include <memory>
#include <variant>
#include <error/error_codes.h>

class Value;

template<typename SuccessReturn, typename... Args>
class AppendableValue {
    public:
        using RetType = std::variant<SuccessReturn, ErrorCode>;
        virtual RetType tryAppend(Args...) = 0;

        template<typename ConcreteType>
        static std::shared_ptr<AppendableValue> asAppendableValue(std::shared_ptr<ConcreteType> val) {
            return std::dynamic_pointer_cast<AppendableValue>(val);
        }

    protected:
        AppendableValue() = default;
        virtual ~AppendableValue() = default;
};

template<typename SuccessReturn, typename... Args>
class SafeAppendableValue : public AppendableValue<SuccessReturn, Args...> {
    public:
        using BaseRetType = typename AppendableValue<SuccessReturn, Args...>::RetType;

        using RetType = SuccessReturn;
        RetType append(Args... args) {
            return std::get<SuccessReturn>(this->tryAppend(args...));
        }

        template<typename ConcreteType>
        static std::shared_ptr<SafeAppendableValue> asSafeAppendableValue(std::shared_ptr<ConcreteType> val) {
            return std::dynamic_pointer_cast<SafeAppendableValue>(val);
        }

    protected:
        SafeAppendableValue() = default;
        virtual ~SafeAppendableValue() = default;
};

class Appendable {
    public:
        using RetType = AppendableValue<Value*, std::shared_ptr<Value>>::RetType;
        virtual RetType appendValue(std::shared_ptr<Value>) = 0;

        template<typename ConcreteType>
        static std::shared_ptr<Appendable> asAppendable(std::shared_ptr<ConcreteType> val) {
            return std::dynamic_pointer_cast<Appendable>(val);
        }

        std::shared_ptr<Value> asValue();

        virtual std::shared_ptr<Value> newEmptyOfSameType() = 0;
    protected:
        Appendable();
};

#endif
