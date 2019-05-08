#define VALUE_TYPE(NAME, TOKEN, STRING, ID) \
TOKEN { \
                    return std::make_unique<Token>(TokenKind::KW_ ## NAME, yytext); \
      } \

#include <value/value_types.def>
