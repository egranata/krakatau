{DIGIT}+        {
                    return std::make_unique<Token>(TokenKind::NUMBER, yytext);
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
                    return std::make_unique<Token>(TokenKind::STRING, yytext);
                  }
[ \t\n]+          /* do nothing */
<<EOF>>         { return nullptr; }
