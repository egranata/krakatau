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

#ifndef STUFF_NATIVE_NATIVEOPERATIONS
#define STUFF_NATIVE_NATIVEOPERATIONS

#include <operation/op.h>
#include <operation/native.h>
#include <stream/byte_stream.h>
#include <parser/parser.h>
#include <string>
#include <unordered_set>
#include <functional>
#include <unordered_map>
#include <memory>

class MachineState;

class NativeOperations {
    public:
        NativeOperations(MachineState&);

        using CreateFunction = std::function<std::shared_ptr<Native>()>;

        using NativeOperationLoader = struct {
            CreateFunction mCreator;
        };

        class Bucket {
            public:
                bool registerOperation(const std::string&, NativeOperationLoader);
                NativeOperationLoader find(const std::string&) const;

                std::string name() const;

            private:
                Bucket(const std::string&, MachineState&);

                MachineState& mMachineState;

                std::string mName;
                std::unordered_map<std::string, NativeOperationLoader> mLoaders;

                friend class NativeOperations;
        };

        std::shared_ptr<Bucket> find(const std::string&) const;
        std::shared_ptr<Bucket> create(const std::string&);

        NativeOperationLoader getLoader(const std::string&) const;

    private:
        MachineState& mMachineState;

        std::unordered_map<std::string, std::shared_ptr<Bucket>> mLoaderNamespaces;

        friend class Bucket;

        NativeOperations(const NativeOperations&) = delete;
        NativeOperations& operator=(const NativeOperations&) = delete;
};

#endif
