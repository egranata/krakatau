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

#ifndef STUFF_PARSER_LEXER
#define STUFF_PARSER_LEXER

#include <stdio.h>
#include <memory>
#include <parser/token.h>

#define YY_DECL std::unique_ptr<Token> yylex(yyscan_t yyscanner)

struct yy_buffer_state;
typedef yy_buffer_state* YY_BUFFER_STATE;

typedef void* yyscan_t;

extern "C" {
    std::unique_ptr<Token> yylex(yyscan_t yyscanner);
}

YY_BUFFER_STATE yy_scan_string ( const char *yy_str , yyscan_t yyscanner );
void yy_delete_buffer ( YY_BUFFER_STATE b , yyscan_t yyscanner );
YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size , yyscan_t yyscanner );
void yy_switch_to_buffer ( YY_BUFFER_STATE new_buffer , yyscan_t yyscanner );
int yylex_init (yyscan_t* scanner);
int yylex_destroy ( yyscan_t yyscanner );

#endif
