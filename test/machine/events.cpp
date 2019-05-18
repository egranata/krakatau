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
#include <gtest/gtest.h>
#include <machine/events.h>
#include <parser/parser.h>
#include <value/value.h>
#include <rtti/rtti.h>
#include <value/block.h>
#include <operation/block.h>
#include <value/value_store.h>
#include <machine/tracer.h>
#include <value/operation.h>

TEST(Events, NumBlocksEntered) {
    class Listener : public MachineEventsListener {
        public:
            Listener(MachineState& ms) : MachineEventsListener(ms) {}
            size_t numBlocks = 0;

            void onEnteringBlock(std::shared_ptr<Block>) override {
                ++numBlocks;
            }
    };

    MachineState ms;
    auto l = std::make_shared<Listener>(ms);
    ms.appendListener(l);
    Parser p("value n0 block { load n1 exec } value n1 block { dup } value main block { load n0 dup exec exec }");
    ASSERT_EQ(3, ms.load(&p));
    auto vblk = ms.value_store().retrieve("main");
    ASSERT_NE(nullptr, vblk);
    auto blk = runtime_ptr_cast<Value_Operation>(vblk);
    ASSERT_NE(nullptr, blk);
    blk->execute(ms);
    ASSERT_EQ(5, l->numBlocks);
}

TEST(Events, NumBlocksExited) {
    class Listener : public MachineEventsListener {
        public:
            Listener(MachineState& ms) : MachineEventsListener(ms) {}
            size_t numBlocks = 0;

            void onLeavingBlock() override {
                ++numBlocks;
            }
    };

    MachineState ms;
    auto l = std::make_shared<Listener>(ms);
    ms.appendListener(l);
    Parser p("value n0 block { load n1 exec } value n1 block { dup } value main block { load n0 dup exec exec }");
    ASSERT_EQ(3, ms.load(&p));
    auto vblk = ms.value_store().retrieve("main");
    ASSERT_NE(nullptr, vblk);
    auto blk = runtime_ptr_cast<Value_Operation>(vblk);
    ASSERT_NE(nullptr, blk);
    blk->execute(ms);
    ASSERT_EQ(5, l->numBlocks);
}

TEST(Events, NumInstructionsRan) {
    class Listener : public MachineEventsListener {
        public:
            Listener(MachineState& ms) : MachineEventsListener(ms) {}
            size_t numInstructions = 0;

            void onExecutingOperation(size_t) override {
                ++numInstructions;
            }
    };

    MachineState ms;
    auto l = std::make_shared<Listener>(ms);
    ms.appendListener(l);
    Parser p("value n0 block { load n1 exec } value n1 block { dup } value main block { load n0 dup exec exec }");
    ASSERT_EQ(3, ms.load(&p));
    auto vblk = ms.value_store().retrieve("main");
    ASSERT_NE(nullptr, vblk);
    auto blk = runtime_ptr_cast<Value_Operation>(vblk);
    ASSERT_NE(nullptr, blk);
    blk->execute(ms);
    ASSERT_EQ(10, l->numInstructions);
}

TEST(Tracer, Description) {
    class Listener : public MachineEventsListener {
        public:
            std::shared_ptr<MachineTracer> tracer;
            size_t numInstructions = 0;
            std::string description;

            Listener(MachineState& ms, std::shared_ptr<MachineTracer> t) : MachineEventsListener(ms), tracer(t) {}

            void onExecutingOperation(size_t) override {
               if (6 == ++numInstructions) description = tracer->describe();
            }
    };

    MachineState ms;
    auto tr = std::make_shared<MachineTracer>(ms);
    auto l = std::make_shared<Listener>(ms, tr);
    ms.appendListener(tr);
    ms.appendListener(l);
    Parser p("value n0 block { load n1 exec } value n1 block { dup } value main block { load n0 dup exec exec }");
    ASSERT_EQ(3, ms.load(&p));
    auto vblk = ms.value_store().retrieve("main");
    auto blk = runtime_ptr_cast<Value_Operation>(vblk);
    blk->execute(ms);
    ASSERT_EQ("block {\n >>  dup\n}\n block {\n      load \"n1\"\n  >>  exec\n }\n  block {\n       load \"n0\"\n       dup\n   >>  exec\n       exec\n  }", l->description);
}

TEST(Events, CorrectMachineState) {
    class Listener : public MachineEventsListener {
        public:
            bool check = true;
            Listener(MachineState& ms) : MachineEventsListener(ms) {}
            size_t numInstructions = 0;

            void onExecutingOperation(size_t) override {
                if (numInstructions++ > 1) check = check && getMachineState().stack().size() > 0;
            }
    };

    MachineState ms;
    auto l = std::make_shared<Listener>(ms);
    ms.appendListener(l);
    Parser p("value main block { push number 1 dup dup add }");
    ASSERT_EQ(1, ms.load(&p));
    auto vblk = ms.value_store().retrieve("main");
    ASSERT_NE(nullptr, vblk);
    auto blk = runtime_ptr_cast<Value_Operation>(vblk);
    ASSERT_NE(nullptr, blk);
    blk->execute(ms);
    ASSERT_EQ(4, l->numInstructions);
    ASSERT_TRUE(l->check);
}