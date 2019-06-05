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

#ifndef STUFF_VALUE_SET
#define STUFF_VALUE_SET

#include <value/value.h>
#include <value/value_set.h>
#include <initializer_list>
#include <value/iterable.h>
#include <value/appendable.h>

class Value_Set : public Value, public IterableValue, public SafeAppendableValue<Value_Set*, std::shared_ptr<Value>>, public Appendable {
    public:
        static constexpr uint8_t MARKER = 'U';

        static std::shared_ptr<Value> fromByteStream(ByteStream* bs);
        static std::shared_ptr<Value> fromParser(Parser*);

        Value_Set();
        Value_Set(std::initializer_list<std::shared_ptr<Value>>);

        SafeAppendableValue<Value_Set*, std::shared_ptr<Value>>::BaseRetType tryAppend(std::shared_ptr<Value>) override;
        Appendable::RetType appendValue(std::shared_ptr<Value>) override;

        size_t size() const override;
        std::shared_ptr<Value> valueAt(size_t i) const;
        std::shared_ptr<Value> at(size_t i) const override { return valueAt(i); }
        bool find(std::shared_ptr<Value>) const;
        virtual std::string describe() const override;
        bool equals(std::shared_ptr<Value>) const override;
        size_t serialize(Serializer*) override;

        size_t hash() const override;
        std::shared_ptr<Value> clone() const override;

        VALUE_SUBCLASS(ValueType::SET, Value);

    protected:
        std::shared_ptr<Value> doTypecast(ValueType) override;
    private:
        ValueSet mSet;
};

#endif
