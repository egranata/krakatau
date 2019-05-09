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

#include <parser/parser.h>
#include <stdlib.h>
#include <value/tuple.h>
#include <function/block.h>
#include <rtti/rtti.h>
#include <operations/op.h>
#include <stream/indenting_stream.h>

Parser::Buffer::~Buffer() = default;

namespace {
class StringBuffer : public Parser::Buffer {
    public:
        StringBuffer(yyscan_t scanner, const std::string& s) {
            mData = s;
            mScanner = scanner;
            mBufferState = yy_scan_string(mData.c_str(), scanner);
        }

        YY_BUFFER_STATE buf() override {
            return mBufferState;
        }

        ~StringBuffer() { yy_delete_buffer(mBufferState, mScanner); }
    private:
        yyscan_t mScanner;
        std::string mData;
        YY_BUFFER_STATE mBufferState;
};
class FileBuffer : public Parser::Buffer {
    static constexpr size_t YY_BUF_SIZE = 16384;
    public:
        FileBuffer(yyscan_t scanner, SharedFile sf) {
            mFile = sf;
            mScanner = scanner;
            mBufferState = yy_create_buffer(mFile.file(), YY_BUF_SIZE, scanner);
            yy_switch_to_buffer(mBufferState, mScanner);
        }

        YY_BUFFER_STATE buf() override {
            return mBufferState;
        }

        ~FileBuffer() { yy_delete_buffer(mBufferState, mScanner); }
    private:
        yyscan_t mScanner;
        SharedFile mFile;
        YY_BUFFER_STATE mBufferState;
};
}

Parser::Parser(const std::string& s) {
    yylex_init(&mScanner);
    mBufferState.reset(new StringBuffer(mScanner, s));
    mPeeked = nullptr;
}

Parser::Parser(SharedFile sf) {
    yylex_init(&mScanner);
    mBufferState.reset(new FileBuffer(mScanner, sf));
    mPeeked = nullptr;
}

Parser::~Parser() {
    yylex_destroy(mScanner);
    mBufferState.release();
    mPeeked = nullptr;
}

Token* Parser::nextToken() {
    if (mPeeked) return mPeeked.get();
    mPeeked = yylex(mScanner);
    return mPeeked.get();
}
void Parser::consumeToken() {
    if (mPeeked.get()) mParsedTokens.push_back(*mPeeked);
    mPeeked.reset();
}

std::optional<Token> Parser::next() {
    auto tok = nextToken();
    if (tok) {
        std::optional<Token> ret = *tok;
        consumeToken();
        return ret;
    }
    return std::nullopt;
}

std::optional<Token> Parser::nextIf(TokenKind tk) {
    auto tok = nextToken();
    if (tok) {
        if (tok->kind() == tk) {
            std::optional<Token> ret = *tok;
            consumeToken();
            return ret;
        }
    }
    return std::nullopt;
}
std::optional<Token> Parser::nextIf(const Token& rhs) {
    auto tok = nextToken();
    if (tok) {
        if (*tok == rhs) {
            std::optional<Token> ret = *tok;
            consumeToken();
            return ret;
        }
    }
    return std::nullopt;
}

bool Parser::nextIf(std::nullptr_t) {
    auto tok = nextToken();
    if (tok) return false;
    return true;
}

bool Parser::peekIf(TokenKind tk) {
    auto tok = nextToken();
    return (tok && tok->kind() == tk);
}

Parser::NamedValue::NamedValue(const std::string& n, std::shared_ptr<Value> v) {
    name = n;
    value = v;
}

std::shared_ptr<Value> Parser::parseValuePayload() {
    return Value::fromParser(this);
}

std::shared_ptr<Parser::NamedValue> Parser::parseValue() {
    if (nextIf(nullptr)) return nullptr;
    if (!expectedError(TokenKind::KW_VALUE)) return nullptr;
    auto ident = expectedError(TokenKind::IDENTIFIER);
    if (!ident) return nullptr;
    auto val = parseValuePayload();
    if (!val) return nullptr;
    return std::make_shared<NamedValue>(ident->value(), val);
}

void Parser::error(const std::string& err) {
    mErrors.push_back(err);
}

size_t Parser::errorsCount() const {
    return mErrors.size();
}

std::optional<Token> Parser::expectedError(TokenKind tk) {
    if (auto tok = nextIf(tk)) return tok;
    IndentingStream is;
    is.append("expected '%s'", tokenKindToString(tk).c_str());
    error(is.str());
    return std::nullopt;
}
std::optional<Token> Parser::expectedError(const Token& tok) {
    if (auto val = nextIf(tok)) return val;
    IndentingStream is;
    is.append("expected '%s'", tok.describe().c_str());
    error(is.str());
    return std::nullopt;
}

std::optional<std::string> Parser::errorAt(size_t i) const {
    if (i >= errorsCount()) return std::nullopt;
    return mErrors.at(i);
}

size_t Parser::parsedTokensCount() const {
    return mParsedTokens.size();
}

std::optional<Token> Parser::parsedTokenAt(size_t i) const {
    if (i >= parsedTokensCount()) return std::nullopt;
    return mParsedTokens.at(i);
}

std::string Parser::context(size_t n) const {
    const size_t count = parsedTokensCount();
    if (count == 0) return "no context";
    if (n > count) n = count;

    IndentingStream is;
    is.append("context: ");

    bool first = true;
    for(size_t i = n;;--i) {
        if (!first) is.append(" ");
        if (auto tok = parsedTokenAt(count - 1 - i)) {
            is.append("%s", tok->value().c_str());
            first = false;
        }
        if (i == 0) break;
    }

    return is.str();
}
