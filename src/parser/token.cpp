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

#include <parser/token.h>
#include <stream/indenting_stream.h>

#define VALUE_TYPE(NAME, TOKEN, STRING, NUMBER) \
    case TokenKind::KW_ ## NAME : return #TOKEN;

std::string tokenKindToString(TokenKind tk) {
    switch (tk) {
#include <value/value_types.def>
        case TokenKind::KW_VALUE: return "value";
        case TokenKind::IDENTIFIER: return "identifier";
        case TokenKind::NUMBER: return "numeric value";
        case TokenKind::OPEN_BLOCK: return "{";
        case TokenKind::CLOSE_BLOCK: return "}";
        case TokenKind::OPEN_PAREN: return "(";
        case TokenKind::CLOSE_PAREN: return ")";
        case TokenKind::COMMA: return ",";
        case TokenKind::BOOLEAN: return "boolean value";
        case TokenKind::STRING: return "string value";
        case TokenKind::OPEN_SQUARE: return "[";
        case TokenKind::CLOSE_SQUARE: return "]";
        case TokenKind::ARROW: return "->";
        case TokenKind::SEMICOLON: return ";";
        case TokenKind::KW_SLOTS: return "slots";
    }

    return "unknown";
}

Token::Token(TokenKind tk, std::string v) : mValue(v), mKind(tk) {}

TokenKind Token::kind() const {
    return mKind;
}
std::string Token::value() const {
    return mValue;
}

bool Token::operator==(const Token& tok) const {
    return (kind() == tok.kind()) && (value() == tok.value());
}

std::string Token::describe() const {
    IndentingStream is;
    is.append("%s(value=%s)", tokenKindToString(kind()).c_str(), value().c_str());
    return is.str();
}
