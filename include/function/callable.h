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

#ifndef STUFF_FUNCTION_CALLABLE
#define STUFF_FUNCTION_CALLABLE

#include <function/block.h>
#include <operations/op.h>
#include <variant>
#include <memory.h>

class PartialBind;
class Serializer;
class ByteStream;
class Value;
class Parser;

class Callable {
    public:
        static constexpr uint8_t MARKER_EMPTY = '/';
        static constexpr uint8_t MARKER_OPERATION = 'O';
        static constexpr uint8_t MARKER_BLOCK = 'B';
        static constexpr uint8_t MARKER_VALUE = 'V';
        static constexpr uint8_t MARKER_BIND = 'D';

        static Callable fromByteStream(ByteStream* bs);
        static Callable fromParser(Parser* p);

        Callable(std::shared_ptr<Operation>);
        Callable(std::shared_ptr<Value>);

        explicit operator bool() const;
        std::string describe() const;
        size_t serialize(Serializer*);
        bool equals(Callable) const;
        Callable clone() const;
        size_t hash() const;

        std::shared_ptr<Operation> operation() const;
        std::shared_ptr<Block> block() const;
        std::shared_ptr<PartialBind> bind() const;

        Operation::Result execute(MachineState&) const;
    private:
        Callable(std::nullptr_t);
        std::shared_ptr<Operation> mPayload;
};

#endif
