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

#ifndef STUFF_PARSER_TOKEN
#define STUFF_PARSER_TOKEN

#include <string>

enum class TokenKind {
    KW_VALUE,
    OPEN_BLOCK,
    CLOSE_BLOCK,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_SQUARE,
    CLOSE_SQUARE,
    COMMA,
    ARROW,
    IDENTIFIER,
    NUMBER,
    BOOLEAN,
    UNICODE,
    LITCHAR,
    STRING,
    SEMICOLON,
    KW_SLOTS,
};

std::string tokenKindToString(TokenKind tk);

class Token {
    public:
        Token(TokenKind, std::string);

        static Token identifier(const std::string&);

        TokenKind kind() const;
        std::string value() const;
        std::string describe() const;

        bool operator==(const Token& tok) const;
    private:
        std::string mValue;
        TokenKind mKind;
};

#endif
