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

#include <args/args.h>
#include <gtest/gtest.h>

TEST(Args, ShortOption) {
    const char* args[] = {
        "test",
        "-a",
        "-b",
        nullptr
    };
    ArgumentParser ap;
    ap.addOption('a', "");
    ap.addOption('b', "");
    ap.addOption('c', "");
    ASSERT_EQ(sizeof(args)/sizeof(args[0]), ap.parse(sizeof(args)/sizeof(args[0]), args));
    ASSERT_TRUE(ap.isOptionSet("-a"));
    ASSERT_TRUE(ap.isOptionSet("-b"));
    ASSERT_FALSE(ap.isOptionSet("-c"));
    ASSERT_EQ(0, ap.getFreeInputs().size());
}

TEST(Args, LongOption) {
    const char* args[] = {
        "test",
        "--opt1",
        "--opt2",
        nullptr
    };
    ArgumentParser ap;
    ap.addOption('a', "opt1");
    ap.addOption('b', "opt2");
    ap.addOption('c', "opt3");
    ASSERT_EQ(sizeof(args)/sizeof(args[0]), ap.parse(sizeof(args)/sizeof(args[0]), args));
    ASSERT_TRUE(ap.isOptionSet("-a"));
    ASSERT_TRUE(ap.isOptionSet("--opt2"));
    ASSERT_FALSE(ap.isOptionSet("--opt3"));
    ASSERT_EQ(0, ap.getFreeInputs().size());
}

TEST(Args, ShortArgument) {
    const char* args[] = {
        "test",
        "-a",
        "value",
        nullptr
    };
    ArgumentParser ap;
    ap.addArgument('a', "", 1);
    ap.addArgument('b', "", 1);
    ASSERT_EQ(sizeof(args)/sizeof(args[0]), ap.parse(sizeof(args)/sizeof(args[0]), args));
    auto vec_a = ap.getArgument("-a");
    auto vec_b = ap.getArgument("-b");
    ASSERT_EQ(0, vec_b.size());
    ASSERT_EQ(1, vec_a.size());
    ASSERT_EQ("value", vec_a[0]);
    ASSERT_EQ(0, ap.getFreeInputs().size());
}

TEST(Args, LongArgument) {
    const char* args[] = {
        "test",
        "--arg",
        "value",
        nullptr
    };
    ArgumentParser ap;
    ap.addArgument('x', "arg", 1);
    ap.addArgument('b', "other", 1);
    ASSERT_EQ(sizeof(args)/sizeof(args[0]), ap.parse(sizeof(args)/sizeof(args[0]), args));
    auto vec_a = ap.getArgument("--arg");
    auto vec_b = ap.getArgument("-b");
    ASSERT_EQ(0, vec_b.size());
    ASSERT_EQ(1, vec_a.size());
    ASSERT_EQ("value", vec_a[0]);
    ASSERT_EQ(0, ap.getFreeInputs().size());
}

TEST(Args, TwoValues) {
    const char* args[] = {
        "test",
        "--arg",
        "value",
        "--arg",
        "more_value",
        nullptr
    };
    ArgumentParser ap;
    ap.addArgument(0, "arg", 2);
    ASSERT_EQ(sizeof(args)/sizeof(args[0]), ap.parse(sizeof(args)/sizeof(args[0]), args));
    auto vec_a = ap.getArgument("--arg");
    ASSERT_EQ(2, vec_a.size());
    ASSERT_EQ("value", vec_a[0]);
    ASSERT_EQ("more_value", vec_a[1]);
    ASSERT_EQ(0, ap.getFreeInputs().size());
}

TEST(Args, FreeValues) {
    const char* args[] = {
        "test",
        "value",
        "more_value",
        nullptr
    };
    ArgumentParser ap;
    ASSERT_EQ(sizeof(args)/sizeof(args[0]), ap.parse(sizeof(args)/sizeof(args[0]), args));
    ap.addArgument(0, "arg", 1);
    ap.addOption('a', "opt1");
    ASSERT_FALSE(ap.isOptionSet("--opt1"));
    ASSERT_EQ(0, ap.getArgument("--arg").size());
    ASSERT_EQ(2, ap.getFreeInputs().size());
    ASSERT_EQ("value", ap.getFreeInputs()[0]);
    ASSERT_EQ("more_value", ap.getFreeInputs()[1]);
}
