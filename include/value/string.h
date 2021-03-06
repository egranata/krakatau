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

#ifndef STUFF_VALUE_STRING
#define STUFF_VALUE_STRING

#include <value/value.h>
#include <string>
#include <value/iterable.h>
#include <value/appendable.h>

class Value_String : public Value, public IterableValue, public AppendableValue<Value_String*, std::shared_ptr<Value>>, public Appendable {
    public:
        static constexpr uint8_t MARKER = '$';

        static std::shared_ptr<Value> fromByteStream(ByteStream* bs);
        static std::shared_ptr<Value> fromParser(Parser*);

        std::shared_ptr<Value> newEmptyOfSameType() override;

        Value_String(const std::u32string&);
        std::u32string value() const;
        std::string utf8() const;
        virtual std::string describe() const override;
        bool equals(std::shared_ptr<Value>) const override;
        size_t serialize(Serializer*) override;

        Value_String* append(char32_t);
        Value_String* append(const std::u32string&);

        AppendableValue<Value_String*, std::shared_ptr<Value>>::RetType tryAppend(std::shared_ptr<Value>) override;
        Appendable::RetType appendValue(std::shared_ptr<Value>) override;

        size_t size() const override;
        std::shared_ptr<Value> at(size_t) const override;

        size_t hash() const override;
        std::shared_ptr<Value> clone() const override;

        VALUE_SUBCLASS(ValueType::STRING, Value);
    private:
        std::u32string mValue;
};

#endif
