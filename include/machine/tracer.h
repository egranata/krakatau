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

#ifndef STUFF_MACHINE_TRACER
#define STUFF_MACHINE_TRACER

#include <machine/state.h>
#include <machine/events.h>
#include <operation/block.h>
#include <operation/op.h>
#include <optional>
#include <string>

class MachineTracer : public MachineEventsListener {
    public:
        struct ActivationRecord {
            std::shared_ptr<Block> block;
            size_t index;
        };

        MachineTracer(MachineState&);

        void onEnteringBlock(std::shared_ptr<Block>) override;
        void onExecutingOperation(size_t) override;
        void onLeavingBlock() override;

        size_t numRecords() const;
        std::optional<ActivationRecord> recordAt(size_t) const;
        auto records_begin() const { return mRecords.begin(); }
        auto records_end() const { return mRecords.end(); }

        std::string describe();

    private:
        std::vector<ActivationRecord> mRecords;
};

#endif
