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

#ifndef STUFF_VALUE_VALUELOADER
#define STUFF_VALUE_VALUELOADER

#include <functional>
#include <stream/byte_stream.h>
#include <unordered_map>
#include <memory>

enum class ValueType : uint32_t;
class Value;
class Parser;

class ValueLoader {
    public:
        static ValueLoader* loader();
        using LoaderFunction = std::function<std::shared_ptr<Value>(ByteStream*)>;
        using ParserFunction = std::function<std::shared_ptr<Value>(Parser*)>;
        void addLoader(uint8_t, LoaderFunction);
        void addParser(ValueType, ParserFunction);
        std::shared_ptr<Value> fromByteStream(ByteStream*);
        std::shared_ptr<Value> fromParser(Parser*);

    private:
        std::unordered_map<uint8_t, LoaderFunction> mLoaders;
        std::unordered_map<ValueType, ParserFunction> mParsers;
        ValueLoader();
        ~ValueLoader();
};

#define VALUE_TYPE(ID, TOKEN, STRING, NUMBER, CLASS) \
    void ValueLoader ## CLASS () __attribute__((constructor, weak));
#include <value/value_types.def>

#endif
