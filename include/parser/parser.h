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

#ifndef STUFF_PARSER_PARSER
#define STUFF_PARSER_PARSER

#include <parser/lexer.h>
#include <parser/token.h>
#include <string>
#include <optional>
#include <value/value.h>
#include <memory>
#include <vector>
#include <stream/shared_file.h>

class Operation;

class Parser {
    public:
        struct Buffer {
            virtual YY_BUFFER_STATE buf() = 0;
            virtual ~Buffer();
        };

        struct NamedValue {
            NamedValue(const std::string&, std::shared_ptr<Value>);
            std::string name;
            std::shared_ptr<Value> value;
        };
        Parser(const std::string&);
        Parser(SharedFile);
        ~Parser();
        std::optional<Token> next();
        std::optional<Token> nextIf(TokenKind);
        std::optional<Token> nextIf(const Token&);
        bool nextIf(std::nullptr_t);

        std::shared_ptr<NamedValue> parseValue();
        std::shared_ptr<Value> parseValuePayload();

        size_t errorsCount() const;

        auto error_begin() const { return mErrors.begin(); }
        auto error_end() const { return mErrors.end(); }

        std::optional<std::string> errorAt(size_t) const;

        std::optional<Token> expectedError(TokenKind);
        std::optional<Token> expectedError(const Token&);

        void error(const std::string&);

    private:
        Token* nextToken();
        void consumeToken();
        std::vector<std::string> mErrors;

        yyscan_t mScanner;

        std::unique_ptr<Buffer> mBufferState;
        std::unique_ptr<Token> mPeeked;
};

#endif
