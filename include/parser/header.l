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

%{
#define YY_NO_INPUT

#include <parser/token.h>
#include <parser/lexer.h>
%}

%option nounput reentrant

HDIGIT	[0-9A-Fa-f]
DIGIT	[0-9] 
ALPHA	[a-zA-Z_$] 
IDENT	[0-9a-zA-Z$_:]
%%
{DIGIT}+        {
                    return std::make_unique<Token>(TokenKind::NUMBER, yytext);
                }
\\u{HDIGIT}+    {
                    return std::make_unique<Token>(TokenKind::UNICODE, yytext+2);
                }
'.'             {
                    std::string s(yytext+1, 1);
                    return std::make_unique<Token>(TokenKind::LITCHAR, s);
                }
value           {
                    return std::make_unique<Token>(TokenKind::KW_VALUE, yytext);
                }
true            {
                    return std::make_unique<Token>(TokenKind::BOOLEAN, yytext);
                }
false           {
                    return std::make_unique<Token>(TokenKind::BOOLEAN, yytext);
                }
slots           {
                    return std::make_unique<Token>(TokenKind::KW_SLOTS, yytext);
                }
->              {
                    return std::make_unique<Token>(TokenKind::ARROW, yytext);
                }
{ALPHA}{IDENT}* {
                    return std::make_unique<Token>(TokenKind::IDENTIFIER, yytext);
                }
[\(]            {
                    return std::make_unique<Token>(TokenKind::OPEN_PAREN, yytext);
                }
[)]             {
                    return std::make_unique<Token>(TokenKind::CLOSE_PAREN, yytext);
                }
[\[]            {
                    return std::make_unique<Token>(TokenKind::OPEN_SQUARE, yytext);
                }
[\]]            {
                    return std::make_unique<Token>(TokenKind::CLOSE_SQUARE, yytext);
                }
[\{]            {
                    return std::make_unique<Token>(TokenKind::OPEN_BLOCK, yytext);
                }
[}]             {
                    return std::make_unique<Token>(TokenKind::CLOSE_BLOCK, yytext);
                }
[,]             {
                    return std::make_unique<Token>(TokenKind::COMMA, yytext);
                }
[;]             {
                    return std::make_unique<Token>(TokenKind::SEMICOLON, yytext);
                }
\"(\\.|[^"\\])*\" {
                    std::string s(yytext);
                    if (s.size() < 2) {
                        s = "";
                    } else {
                        s = s.substr(1);
                        s.pop_back();
                    }
                    return std::make_unique<Token>(TokenKind::STRING, s);
                  }
[ \t\n]+          /* do nothing */
<<EOF>>         { return nullptr; }
