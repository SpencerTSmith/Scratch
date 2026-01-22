#ifndef C_TOKENIZE
#define C_TOKENIZE

#include "common.h"

#define C_Token_Type(X)                 \
  X(C_TOKEN_NONE)                       \
  X(C_TOKEN_BEGIN_PARENTHESIS)          \
  X(C_TOKEN_BEGIN_CURLY_BRACE)          \
  X(C_TOKEN_BEGIN_SQUARE_BRACE)         \
  X(C_TOKEN_CLOSE_PARENTHESIS)          \
  X(C_TOKEN_CLOSE_CURLY_BRACE)          \
  X(C_TOKEN_CLOSE_SQUARE_BRACE)         \
  X(C_TOKEN_STAR)                       \
  X(C_TOKEN_ADD)                        \
  X(C_TOKEN_MINUS)                      \
  X(C_TOKEN_DIVIDE)                     \
  X(C_TOKEN_MODULO)                     \
  X(C_TOKEN_XOR)                        \
  X(C_TOKEN_BITWISE_AND)                \
  X(C_TOKEN_BITWISE_OR)                 \
  X(C_TOKEN_ASSIGN)                     \
  X(C_TOKEN_ADD_ASSIGN)                 \
  X(C_TOKEN_SUBTRACT_ASSIGN)            \
  X(C_TOKEN_MULTIPLY_ASSIGN)            \
  X(C_TOKEN_DIVIDE_ASSIGN)              \
  X(C_TOKEN_MODULO_ASSIGN)              \
  X(C_TOKEN_AND_ASSIGN)                 \
  X(C_TOKEN_OR_ASSIGN)                  \
  X(C_TOKEN_XOR_ASSIGN)                 \
  X(C_TOKEN_COMMA)                      \
  X(C_TOKEN_SEMICOLON)                  \
  X(C_TOKEN_DOT)                        \
  X(C_TOKEN_ARROW)                      \
  X(C_TOKEN_BITWISE_NOT)                \
  X(C_TOKEN_DECREMENT)                  \
  X(C_TOKEN_INCREMENT)                  \
  X(C_TOKEN_COMPARE_EQUAL)              \
  X(C_TOKEN_LESS_THAN)                  \
  X(C_TOKEN_LESS_THAN_EQUAL)            \
  X(C_TOKEN_GREATER_THAN)               \
  X(C_TOKEN_GREATER_THAN_EQUAL)         \
  X(C_TOKEN_LOGICAL_NOT)                \
  X(C_TOKEN_COMPARE_NOT_EQUAL)          \
  X(C_TOKEN_LOGICAL_AND)                \
  X(C_TOKEN_LOGICAL_OR)                 \
  X(C_TOKEN_QUESTION)                   \
  X(C_TOKEN_COLON)                      \
  X(C_TOKEN_LITERAL_STRING)             \
  X(C_TOKEN_LITERAL_CHAR)               \
  X(C_TOKEN_LITERAL_INT)                \
  X(C_TOKEN_LITERAL_DOUBLE)             \
  X(C_TOKEN_KEYWORD_FOR)                \
  X(C_TOKEN_KEYWORD_WHILE)              \
  X(C_TOKEN_KEYWORD_DO)                 \
  X(C_TOKEN_KEYWORD_IF)                 \
  X(C_TOKEN_KEYWORD_ELSE)               \
  X(C_TOKEN_KEYWORD_SWITCH)             \
  X(C_TOKEN_KEYWORD_CASE)               \
  X(C_TOKEN_KEYWORD_DEFAULT)            \
  X(C_TOKEN_KEYWORD_BREAK)              \
  X(C_TOKEN_KEYWORD_CONTINUE)           \
  X(C_TOKEN_KEYWORD_RETURN)             \
  X(C_TOKEN_KEYWORD_GOTO)               \
  X(C_TOKEN_KEYWORD_STRUCT)             \
  X(C_TOKEN_KEYWORD_ENUM)               \
  X(C_TOKEN_KEYWORD_UNION)              \
  X(C_TOKEN_KEYWORD_TYPEDEF)            \
  X(C_TOKEN_KEYWORD_CONST)              \
  X(C_TOKEN_KEYWORD_STATIC)             \
  X(C_TOKEN_KEYWORD_RESTRICT)           \
  X(C_TOKEN_KEYWORD_VOLATILE)           \
  X(C_TOKEN_KEYWORD_EXTERN)             \
  X(C_TOKEN_KEYWORD_INLINE)             \
  X(C_TOKEN_KEYWORD_SIZEOF)             \
  X(C_TOKEN_KEYWORD_REGISTER)           \
  X(C_TOKEN_KEYWORD_VOID)               \
  X(C_TOKEN_KEYWORD_CHAR)               \
  X(C_TOKEN_KEYWORD_SHORT)              \
  X(C_TOKEN_KEYWORD_INT)                \
  X(C_TOKEN_KEYWORD_LONG)               \
  X(C_TOKEN_KEYWORD_FLOAT)              \
  X(C_TOKEN_KEYWORD_DOUBLE)             \
  X(C_TOKEN_KEYWORD_UNSIGNED)           \
  X(C_TOKEN_KEYWORD_SIGNED)             \
  X(C_TOKEN_IDENTIFIER)                 \
  X(C_TOKEN_EOF)                        \
  X(C_TOKEN_COUNT)

ENUM_TABLE(C_Token_Type);

typedef enum C_Token_Flags
{
  C_TOKEN_FLAG_LITERAL_UNSIGNED = 1 << 0,
  C_TOKEN_FLAG_LITERAL_LONG     = 1 << 1,
  C_TOKEN_FLAG_LITERAL_2ND_LONG = 1 << 2,
  C_TOKEN_FLAG_LITERAL_FLOAT    = 1 << 3,
} C_Token_Flags;

typedef struct C_Token C_Token;
struct C_Token
{
  C_Token_Type  type;
  C_Token_Flags flags;

  String raw;
  usize  line;
  usize  column;

  union
  {
    String string_literal;
    struct
    {
      u8  base;
      u64 v;
    } int_literal;
    u8  char_literal;
    f64 float_literal;
  };
};

typedef struct C_Lexer C_Lexer;
struct C_Lexer
{
  String source;
  usize  at;
  usize  lines_processed;
  usize  columns_processed;
};

DEFINE_ARRAY(C_Token);
DEFINE_CHUNK_LIST(C_Token, 4096);

static
C_Token_Array tokenize_c_code(Arena *arena, String code);

#endif // C_TOKENIZE
