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

#include <gtest/gtest.h>
#include <parser/parser.h>

TEST(ParserErrors, MissingValue) {
    Parser p("number foo 12");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(1, p.errorsCount());
    ASSERT_EQ("expected 'value'", p.errorAt(0));
    ASSERT_EQ("no context", p.context());
}

TEST(ParserErrors, MissingIdentifier) {
    Parser p("value number 12");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(1, p.errorsCount());
    ASSERT_EQ("expected 'identifier'", p.errorAt(0));
    ASSERT_EQ("context: value number", p.context());
}

TEST(ParserErrors, InvalidBooleanValue) {
    Parser p("value foo boolean nope");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(1, p.errorsCount());
    ASSERT_EQ("expected 'boolean value'", p.errorAt(0));
    ASSERT_EQ("context: value foo boolean", p.context());
}

TEST(ParserErrors, MissingError) {
    Parser p("value foo error");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(1, p.errorsCount());
    ASSERT_EQ("expected 'identifier'", p.errorAt(0));
    ASSERT_EQ("context: value foo error", p.context());
}

TEST(ParserErrors, NotAnError) {
    Parser p("value foo error allgood");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(1, p.errorsCount());
    ASSERT_EQ("not a valid error code", p.errorAt(0));
    ASSERT_EQ("context: value foo error allgood", p.context());
}

TEST(ParserErrors, NotAString) {
    Parser p("value foo string foobar");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(1, p.errorsCount());
    ASSERT_EQ("expected 'string value'", p.errorAt(0));
    ASSERT_EQ("context: value foo string", p.context());
}

TEST(ParserErrors, InvalidTuple) {
    Parser p("value foo tuple ( number 12, foo)");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(1, p.errorsCount());
    ASSERT_EQ("expected value", p.errorAt(0).value_or("fail"));
    ASSERT_EQ("context: number 12 , foo", p.context());
}

TEST(ParserErrors, NotAType) {
    Parser p("value foo type int");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(1, p.errorsCount());
    ASSERT_EQ("expected value type keyword", p.errorAt(0).value_or("fail"));
    ASSERT_EQ("context: value foo type", p.context());
}

TEST(ParserErrors, UnarrowedTable) {
    Parser p("value foo table [number 123 -> number 1 boolean true boolean false");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(1, p.errorsCount());
    ASSERT_EQ("expected '->'", p.errorAt(0));
    ASSERT_EQ("context: number 1 boolean true", p.context());
}

TEST(ParserErrors, IncompleteOperation) {
    Parser p("value foo block { push");
    ASSERT_EQ(nullptr, p.parseValue());
    ASSERT_EQ(2, p.errorsCount());
    ASSERT_EQ("expected 'identifier'", p.errorAt(0).value_or("fail"));
    ASSERT_EQ("expected operation", p.errorAt(1).value_or("fail"));
    ASSERT_EQ("context: foo block { push", p.context());
}
