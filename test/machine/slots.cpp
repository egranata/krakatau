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
#include <machine/slots_handler.h>
#include <operation/block.h>
#include <machine/events.h>
#include <parser/parser.h>
#include <gtest/gtest.h>
#include <value/block.h>

TEST(Slots, IsSlotEntered) {
    class Listener : public MachineEventsListener {
        public:
            Listener(MachineState& ms) : MachineEventsListener(ms) {}
            void onEnteringBlock(std::shared_ptr<Block> b) override {
                ASSERT_EQ(getMachineState().currentSlot(), b->currentSlot());
                ASSERT_NE(nullptr, b->currentSlot());
                ASSERT_NE(nullptr, getMachineState().currentSlot());
            }
    };

    MachineState ms;
    auto l = std::make_shared<Listener>(ms);
    ms.appendListener(l);
    Parser p("value n0 block { load n1 exec } value n1 block { dup } value main block { load n0 dup exec exec }");
    ASSERT_EQ(3, ms.load(&p));
    ms.execute();
    ASSERT_EQ(nullptr, ms.currentSlot());
}
