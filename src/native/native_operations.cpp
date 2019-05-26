// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <native/native_operations.h>
#include <machine/state.h>
#include <value/value_store.h>
#include <value/operation.h>
#include <value/value.h>
#include <stream/indenting_stream.h>

NativeOperations::NativeOperations(MachineState& ms) : mMachineState(ms) {}

NativeOperations::Bucket::Bucket(const std::string& name, MachineState& ms) : mMachineState(ms), mName(name), mLoaders() {}

std::shared_ptr<NativeOperations::Bucket> NativeOperations::create(const std::string& name) {
    auto i = mLoaderNamespaces.find(name);
    if (i != mLoaderNamespaces.end()) return i->second;

    mLoaderNamespaces.emplace(name, std::shared_ptr<Bucket>(new Bucket(name, mMachineState)));

    return find(name);
}

std::shared_ptr<NativeOperations::Bucket> NativeOperations::find(const std::string& name) const {
    auto i = mLoaderNamespaces.find(name);
    if (i != mLoaderNamespaces.end()) return i->second;
    return nullptr;
}

std::string NativeOperations::Bucket::name() const {
    return mName;
}

bool NativeOperations::Bucket::registerOperation(const std::string& name, NativeOperationLoader loader) {
    auto i = mLoaders.find(name);
    if (i == mLoaders.end()) {
        mLoaders.emplace(name, loader);
        IndentingStream is;
        is.append("%s::%s", this->name().c_str(), name.c_str());
        return mMachineState.value_store().store(is.str(), Value::fromOperation(loader.mCreator()));
    } else return false;
}

NativeOperations::NativeOperationLoader NativeOperations::Bucket::find(const std::string& name) const {
    auto i = mLoaders.find(name);
    if (i == mLoaders.end()) return {nullptr};
    return i->second;
}

NativeOperations::NativeOperationLoader NativeOperations::getLoader(const std::string& name) const {
    auto i = name.find("::");
    if (i <= 0) return {nullptr,};
    auto bn = name.substr(0,i);
    auto ln = name.substr(i+2);
    auto bucket = find(bn);
    if (bucket) return bucket->find(ln);
    else return {nullptr,};
}
