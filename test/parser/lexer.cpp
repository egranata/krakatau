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
#include <parser/lexer.h>
#include <parser/token.h>
#include <vector>
#include <string>

TEST(Lexer, LexString) {
    std::vector<Token> expected = {
        {TokenKind::STRING, "\"test string\""},
        {TokenKind::IDENTIFIER, "_is_a_valid_prefix"},
        {TokenKind::IDENTIFIER, "$he:llo"},
        {TokenKind::NUMBER, "123456"},
        {TokenKind::IDENTIFIER, "hello::1234"},
        {TokenKind::KW_SLOTS, "slots"},
    };
    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE buf = yy_scan_string("slots hello::1234 123456 $he:llo _is_a_valid_prefix \"test string\"", scanner);
    while(!expected.empty()) {
        auto parsed = yylex(scanner);
        ASSERT_NE(parsed, nullptr);
        ASSERT_EQ(*parsed, expected.back());
        expected.pop_back();
    }
    ASSERT_EQ(nullptr, yylex(scanner));
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
}

TEST(Lexer, LexKeywords) {
    std::vector<Token> expected = {
        {TokenKind::BOOLEAN, "true"},
        {TokenKind::BOOLEAN, "false"},
        {TokenKind::NUMBER, "123"},
        {TokenKind::IDENTIFIER, "hello"},
        {TokenKind::KW_VALUE, "value"},
        {TokenKind::IDENTIFIER, "hello1234"},
    };
    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE buf = yy_scan_string("hello1234 value hello 123 false true", scanner);
    while(!expected.empty()) {
        auto parsed = yylex(scanner);
        ASSERT_NE(parsed, nullptr);
        ASSERT_EQ(*parsed, expected.back());
        expected.pop_back();
    }
    ASSERT_EQ(nullptr, yylex(scanner));
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
}

TEST(Lexer, LexOperations) {
    std::vector<Token> expected = {
        {TokenKind::IDENTIFIER, "dup"},
        {TokenKind::IDENTIFIER, "load"},
        {TokenKind::IDENTIFIER, "add"},
    };
    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE buf = yy_scan_string("add load dup", scanner);
    while(!expected.empty()) {
        auto parsed = yylex(scanner);
        ASSERT_NE(parsed, nullptr);
        ASSERT_EQ(*parsed, expected.back());
        expected.pop_back();
    }
    ASSERT_EQ(nullptr, yylex(scanner));
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
}

TEST(Lexer, LexPunctuation) {
    std::vector<Token> expected = {
        {TokenKind::CLOSE_BLOCK, "}"},
        {TokenKind::SEMICOLON, ";"},
        {TokenKind::CLOSE_PAREN, ")"},
        {TokenKind::OPEN_BLOCK, "{"},
        {TokenKind::CLOSE_PAREN, ")"},
        {TokenKind::ARROW, "->"},
        {TokenKind::COMMA, ","},
        {TokenKind::OPEN_PAREN, "("},
        {TokenKind::OPEN_PAREN, "("},
        {TokenKind::OPEN_SQUARE, "["},
        {TokenKind::COMMA, ","},
        {TokenKind::CLOSE_SQUARE, "]"},
        {TokenKind::COMMA, ","},
    };
    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE buf = yy_scan_string(",],[(( ,  -> ){);}", scanner);
    while(!expected.empty()) {
        auto parsed = yylex(scanner);
        ASSERT_NE(parsed, nullptr);
        ASSERT_EQ(*parsed, expected.back());
        expected.pop_back();
    }
    ASSERT_EQ(nullptr, yylex(scanner));
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
}
