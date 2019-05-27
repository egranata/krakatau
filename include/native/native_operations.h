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
#include <stream/byte_stream.h>
#include <parser/parser.h>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <optional>
#include <memory>

class Native;
class Value_Operation;
class MachineState;

class NativeOperations {
    public:
        NativeOperations(MachineState&);

        class Bucket;
        using CreateFunction = std::function<std::shared_ptr<Native>(std::shared_ptr<Bucket>)>;

        using NativeOperationLoader = struct {
            CreateFunction mCreator;
        };

        class Bucket : public std::enable_shared_from_this<Bucket> {
            public:
                bool newOperation(NativeOperationLoader);

                std::string name() const;
                std::shared_ptr<Value_Operation> find(const std::string&) const;

            private:
                Bucket(const std::string&, MachineState&);

                MachineState& mMachineState;

                std::string mName;
                std::vector<NativeOperationLoader> mLoaders;
                std::unordered_map<std::string, std::shared_ptr<Value_Operation>> mOperations;

                friend class NativeOperations;
        };

        struct LibraryDescriptor {
            std::string bucket;
            std::vector<NativeOperationLoader> loaders;

            bool load(MachineState&) const;
        };

        using LibraryEntryPoint = std::optional<LibraryDescriptor>(*)();

        bool loadNativeLibrary(const std::string&);

        std::shared_ptr<Bucket> find(const std::string&) const;
        std::shared_ptr<Bucket> create(const std::string&);

        std::shared_ptr<Value_Operation> getOperation(const std::string&) const;

    private:
        MachineState& mMachineState;

        std::unordered_map<std::string, std::shared_ptr<Bucket>> mLoaderNamespaces;
        std::unordered_map<std::string, std::string> mNativeLibraries;

        friend class Bucket;

        NativeOperations(const NativeOperations&) = delete;
        NativeOperations& operator=(const NativeOperations&) = delete;
};

#endif
