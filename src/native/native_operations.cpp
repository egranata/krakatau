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
#include <operation/native.h>

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

bool NativeOperations::Bucket::newOperation(NativeOperationLoader loader) {
    if (auto newop = loader.mCreator(shared_from_this())) {
        mLoaders.push_back(loader);
        auto opval = Value::fromOperation(newop);
        mOperations.emplace(newop->name(), opval);
        return mMachineState.value_store().store(newop->fullyQualifiedName(), opval);
    }
    return false;
}

std::shared_ptr<Value_Operation> NativeOperations::Bucket::find(const std::string& name) const {
    auto i = mOperations.find(name);
    if (i == mOperations.end()) return nullptr;
    return i->second;
}

bool NativeOperations::LibraryDescriptor::load(MachineState& ms) const {
    if (auto bucket = ms.native_operations().create(this->bucket)) {
        for(const auto& loader : this->loaders) {
            if (false == bucket->newOperation(loader)) return false;
        }
        return true;
    } else return false;
}

std::shared_ptr<Value_Operation> NativeOperations::getOperation(const std::string& name) const {
    auto i = name.find("::");
    if (i <= 0) return {nullptr,};
    auto bn = name.substr(0,i);
    auto ln = name.substr(i+2);
    auto bucket = find(bn);
    if (bucket) return bucket->find(ln);
    else return nullptr;
}
