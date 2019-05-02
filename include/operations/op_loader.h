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

#ifndef STUFF_OPERATION_OPLOADER
#define STUFF_OPERATION_OPLOADER

#include <functional>
#include <stream/byte_stream.h>
#include <unordered_map>
#include <memory>

enum class OperationType : uint32_t;
class Operation;
class Parser;

class OperationLoader {
    public:
        static OperationLoader* loader();
        using LoaderFunction = std::function<std::shared_ptr<Operation>(ByteStream*)>;
        using ParserFunction = std::function<std::shared_ptr<Operation>(Parser*)>;
        void addLoader(OperationType, LoaderFunction);
        void addParser(OperationType, ParserFunction);
        std::shared_ptr<Operation> fromByteStream(ByteStream*);
        std::shared_ptr<Operation> fromParser(Parser*);

    private:
        std::unordered_map<OperationType, LoaderFunction> mLoaders;
        std::unordered_map<OperationType, ParserFunction> mParsers;
        OperationLoader();
        ~OperationLoader();
};

#define OP_LOADER(Ty, Class) \
void OpLoader ## Class () __attribute__((constructor, weak)); \
void OpLoader ## Class () { \
            OperationLoader::loader()->addLoader(Ty, [] (ByteStream* bs) -> std::shared_ptr<Operation> { \
                return Class ::fromByteStream(bs); \
            }); \
            OperationLoader::loader()->addParser(Ty, [] (Parser* p) -> std::shared_ptr<Operation> { \
                return Class ::fromParser(p); \
            }); \

#endif
