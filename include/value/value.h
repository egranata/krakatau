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

#ifndef STUFF_VALUE_VALUE
#define STUFF_VALUE_VALUE

#include <inttypes.h>
#include <stdint.h>
#include <memory>
#include <value/value_types.h>
#include <string>
#include <error/error_codes.h>
#include <rtti/rtti.h>
#include <iostream>
#include <initializer_list>
#include <utility>

class PartialBind;
class Block;
class ByteStream;
class Serializer;
class Parser;
class Operation;

class Value_Number;
class Value_Empty;
class Value_Operation;
class Value_Boolean;
class Value_Tuple;
class Value_Table;
class Value_Type;
class Value_Character;
class Value_Error;
class Value_Set;
class Value_String;

class Value : public std::enable_shared_from_this<Value> {
    public:
        static std::shared_ptr<Value_Empty> empty();
        static std::shared_ptr<Value_Boolean> fromBoolean(bool);
        static std::shared_ptr<Value_Number> fromNumber(uint64_t);
        static std::shared_ptr<Value_Operation> fromBlock(std::shared_ptr<Block>);
        static std::shared_ptr<Value_Operation> fromOperation(std::shared_ptr<Operation>);
        static std::shared_ptr<Value_String> fromString(const std::string&);
        static std::shared_ptr<Value_String> fromString(const std::u32string&);
        static std::shared_ptr<Value_Character> fromCharacter(char32_t);
        static std::shared_ptr<Value_Error> error(ErrorCode);
        static std::shared_ptr<Value_Type> type(ValueType);
        static std::shared_ptr<Value_Operation> fromBind(std::shared_ptr<PartialBind>);
        static std::shared_ptr<Value_Tuple> tuple(std::initializer_list<std::shared_ptr<Value>>);
        static std::shared_ptr<Value_Table> table(std::initializer_list<std::pair<std::shared_ptr<Value>,std::shared_ptr<Value>>>);
        static std::shared_ptr<Value_Set> set(std::initializer_list<std::shared_ptr<Value>>);

        static std::shared_ptr<Value> fromByteStream(ByteStream*);
        static std::shared_ptr<Value> fromParser(Parser*);

        virtual ~Value();
        virtual ValueType getClassId() const { return ValueType::EMPTY; }

        static ValueType getStaticClassId() { return ValueType::EMPTY; }

        virtual bool isOfType(ValueType) const;

        template <typename T>
        bool isOfClass() const { return isOfType( T::getStaticClassId() ); };

        template <typename T>
        auto asClass() {
            if (isOfClass<T>()) return runtime_ptr_cast<T>(this);
            return (T*)nullptr;
        }

        virtual std::string describe() const = 0;
        virtual size_t serialize(Serializer*) = 0;
        virtual bool equals(std::shared_ptr<Value>) const = 0;

        virtual size_t hash() const = 0;
        virtual std::shared_ptr<Value> clone() const = 0;

        std::shared_ptr<Value> typecast(ValueType);

    protected:
        virtual std::shared_ptr<Value> doTypecast(ValueType);
        Value();

    private:
        Value& operator=(const Value&) = delete;
        Value(const Value&) = delete;
};

std::ostream& operator<<(std::ostream&, std::shared_ptr<Value>);
std::ostream& operator<<(std::ostream&, Value*);

#endif

