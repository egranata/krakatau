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

#ifndef STUFF_MACHINE_STATE
#define STUFF_MACHINE_STATE

#include <stack/stack.h>
#include <value/value_store.h>
#include <stream/byte_stream.h>
#include <stream/serializer.h>
#include <parser/parser.h>
#include <operations/op.h>
#include <vector>
#include <optional>

class Block;
class MachineEventsListener;

class MachineState {
    public:
        static constexpr uint32_t FORMAT_VERSION = 1;

        MachineState();

        Stack& stack();
        ValueStore& value_store();

        void onEnteringBlock(std::shared_ptr<Block>);
        void onExecutingOperation(size_t);
        void onLeavingBlock();

        size_t listenersCount() const;
        std::shared_ptr<MachineEventsListener> listenerAt(size_t) const;
        auto listeners_begin() const { return mListeners.begin(); }
        auto listeners_end() const { return mListeners.end(); }

        void appendListener(std::shared_ptr<MachineEventsListener>);
        void removeListener(std::shared_ptr<MachineEventsListener>);

        size_t load(ByteStream*);
        size_t load(Parser*);
        size_t serialize(Serializer*);

        std::optional<Operation::Result> execute(const std::string& block = "main");
    private:
        bool loadOneValue(ByteStream*);
        bool loadOneValue(Parser*);

        MachineState(const MachineState&) = delete;
        MachineState& operator=(const MachineState&) = delete;

        Stack mStack;
        ValueStore mValueStore;

        std::vector<std::shared_ptr<MachineEventsListener>> mListeners;
};

#endif
