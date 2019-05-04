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

#ifndef STUFF_MACHINE_SLOTSHANDLER
#define STUFF_MACHINE_SLOTSHANDLER

#include <machine/state.h>
#include <machine/events.h>
#include <function/block.h>

class SlotsHandler : public MachineEventsListener {
    public:
        SlotsHandler(MachineState&);

        void onEnteringBlock(std::shared_ptr<Block>) override;
        void onLeavingBlock() override;
};

#endif
