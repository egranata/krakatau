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

#include <machine/state.h>
#include <algorithm>
#include <rtti/rtti.h>
#include <value/tuple.h>
#include <machine/events.h>
#include <value/block.h>
#include <machine/slots_handler.h>
#include <value/operation.h>

MachineState::MachineState() {
    appendListener(std::make_shared<SlotsHandler>(*this));
}

Stack& MachineState::stack() {
    return mStack;
}

ValueStore& MachineState::value_store() {
    return mValueStore;
}

NativeOperations& MachineState::native_operations() {
    return mNativeOperations;
}

bool MachineState::loadOneValue(ByteStream* bs) {
    auto rname = bs->readIdentifier();
    if (!rname) return false;

    if (auto val = Value::fromByteStream(bs)) {
        value_store().store(rname.value(), val);
        return true;
    }

    return false;
}

bool MachineState::loadOneValue(Parser* p) {
    auto val = p->parseValue();
    if (val == nullptr) return false;

    value_store().store(val->name, val->value);
    return true;
}

size_t MachineState::load(ByteStream* bs) {
    auto ver = bs->readNumber(1);
    if (ver.value_or(0) != FORMAT_VERSION) return 0;

    size_t n = 0;
    while(loadOneValue(bs)) ++n;
    return n;
}

size_t MachineState::serialize(Serializer* s) {
    s->writeNumber(FORMAT_VERSION, 1);
    return value_store().serialize(s);
}

size_t MachineState::load(Parser* p) {
    size_t n = 0;
    while(loadOneValue(p)) ++n;
    return n;
}

void MachineState::onEnteringBlock(std::shared_ptr<Block> b) {
    for (auto el : mListeners) {
        el->onEnteringBlock(b);
    }
}

void MachineState::onExecutingOperation(size_t i) {
    for (auto el : mListeners) {
        el->onExecutingOperation(i);
    }
}

void MachineState::onLeavingBlock() {
    for (auto el : mListeners) {
        el->onLeavingBlock();
    }
}

size_t MachineState::listenersCount() const {
    return mListeners.size();
}
std::shared_ptr<MachineEventsListener> MachineState::listenerAt(size_t i) const {
    if (i >= listenersCount()) return nullptr;
    return mListeners.at(i);
}

void MachineState::appendListener(std::shared_ptr<MachineEventsListener> l) {
    mListeners.push_back(l);
}
void MachineState::removeListener(std::shared_ptr<MachineEventsListener> l) {
    std::remove(mListeners.begin(), mListeners.end(), l);
}

std::optional<Operation::Result> MachineState::execute(const std::string& name) {
    auto vblk = value_store().retrieve(name);
    if (auto clb = vblk->asClass<Value_Operation>()) return clb->execute(*this);
    else return std::nullopt;
}

void MachineState::pushSlot(std::shared_ptr<Block> blk) {
    mSlots.push(blk->currentSlot());
}
void MachineState::popSlot() {
    if (!mSlots.empty()) mSlots.pop();
}
std::shared_ptr<ValueTable> MachineState::currentSlot() const {
    if (mSlots.empty()) return nullptr;
    return mSlots.top();
}
