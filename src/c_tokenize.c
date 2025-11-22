#include "common.h"

// TODO: Evaluate literals, escapes, numbers into right types, etc.

#define C_Token_Type(X)                 \
  X(C_TOKEN_NONE)                       \
  X(C_TOKEN_BEGIN_PARENTHESIS)           \
  X(C_TOKEN_BEGIN_CURLY_BRACE)           \
  X(C_TOKEN_BEGIN_SQUARE_BRACE)          \
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
  X(C_TOKEN_NOT)                        \
  X(C_TOKEN_DECREMENT)                  \
  X(C_TOKEN_INCREMENT)                  \
  X(C_TOKEN_COMPARE_EQUAL)              \
  X(C_TOKEN_LOGICAL_AND)                \
  X(C_TOKEN_LOGICAL_OR)                 \
  X(C_TOKEN_LITERAL_STRING)             \
  X(C_TOKEN_LITERAL_CHAR)               \
  X(C_TOKEN_LITERAL_INT)                \
  X(C_TOKEN_LITERAL_UNSIGNED_INT)       \
  X(C_TOKEN_LITERAL_LONG)               \
  X(C_TOKEN_LITERAL_LONG_LONG)          \
  X(C_TOKEN_LITERAL_UNSIGNED_LONG)      \
  X(C_TOKEN_LITERAL_UNSIGNED_LONG_LONG) \
  X(C_TOKEN_LITERAL_FLOAT)              \
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
  X(C_TOKEN_EOF)

ENUM_TABLE(C_Token_Type);

typedef struct C_Token C_Token;
struct C_Token
{
  C_Token_Type type;
  String raw;

  union
  {
    u64 _int;
    f64 _float;
  } value;
};

typedef struct C_Lexer C_Lexer;
struct C_Lexer
{
  String source;
  usize  at;
};

// Single character tokens... some special logic required for e.g. =,&,| (could be ==,&&,||)
// In those cases we check and then grab the 2nd token type of the triple
// As well for operation assignment tokens (+=, -=, &=, etc.)
// We use the 3rd of the triple
static C_Token_Type c_token_table[][3] =
{
  ['('] = {C_TOKEN_BEGIN_PARENTHESIS,   C_TOKEN_NONE,          C_TOKEN_NONE},
  [')'] = {C_TOKEN_CLOSE_PARENTHESIS,  C_TOKEN_NONE,          C_TOKEN_NONE},
  ['{'] = {C_TOKEN_BEGIN_CURLY_BRACE,   C_TOKEN_NONE,          C_TOKEN_NONE},
  ['}'] = {C_TOKEN_CLOSE_CURLY_BRACE,  C_TOKEN_NONE,          C_TOKEN_NONE},
  ['['] = {C_TOKEN_BEGIN_SQUARE_BRACE,  C_TOKEN_NONE,          C_TOKEN_NONE},
  [']'] = {C_TOKEN_CLOSE_SQUARE_BRACE, C_TOKEN_NONE,          C_TOKEN_NONE},

  ['+'] = {C_TOKEN_ADD,                C_TOKEN_INCREMENT,     C_TOKEN_ADD_ASSIGN},
  ['-'] = {C_TOKEN_MINUS,              C_TOKEN_DECREMENT,     C_TOKEN_SUBTRACT_ASSIGN},
  ['*'] = {C_TOKEN_STAR,               C_TOKEN_NONE,          C_TOKEN_MULTIPLY_ASSIGN},
  ['/'] = {C_TOKEN_DIVIDE,             C_TOKEN_NONE,          C_TOKEN_DIVIDE_ASSIGN},
  ['%'] = {C_TOKEN_MODULO,             C_TOKEN_NONE,          C_TOKEN_MODULO_ASSIGN},
  ['~'] = {C_TOKEN_NOT,                C_TOKEN_NONE,          C_TOKEN_NONE},
  ['^'] = {C_TOKEN_XOR,                C_TOKEN_NONE,          C_TOKEN_XOR_ASSIGN},
  ['&'] = {C_TOKEN_BITWISE_AND,        C_TOKEN_LOGICAL_AND,   C_TOKEN_AND_ASSIGN},
  ['|'] = {C_TOKEN_BITWISE_OR,         C_TOKEN_LOGICAL_OR,    C_TOKEN_OR_ASSIGN},
  ['='] = {C_TOKEN_ASSIGN,             C_TOKEN_COMPARE_EQUAL, C_TOKEN_NONE},
  [','] = {C_TOKEN_COMMA,              C_TOKEN_NONE,          C_TOKEN_NONE},
  [';'] = {C_TOKEN_SEMICOLON,          C_TOKEN_NONE,          C_TOKEN_NONE},
  ['.'] = {C_TOKEN_DOT,                C_TOKEN_NONE,          C_TOKEN_NONE},
};

typedef struct C_Keyword_Info C_Keyword_Info;
struct C_Keyword_Info
{
  String string;
  C_Token_Type type;
};

static C_Keyword_Info c_keywords[] =
{
  {STR("for"),      C_TOKEN_KEYWORD_FOR},
  {STR("while"),    C_TOKEN_KEYWORD_WHILE},
  {STR("do"),       C_TOKEN_KEYWORD_DO},
  {STR("if"),       C_TOKEN_KEYWORD_IF},
  {STR("else"),     C_TOKEN_KEYWORD_ELSE},
  {STR("switch"),   C_TOKEN_KEYWORD_SWITCH},
  {STR("case"),     C_TOKEN_KEYWORD_CASE},
  {STR("default"),  C_TOKEN_KEYWORD_DEFAULT},
  {STR("break"),    C_TOKEN_KEYWORD_BREAK},
  {STR("continue"), C_TOKEN_KEYWORD_CONTINUE},
  {STR("return"),   C_TOKEN_KEYWORD_RETURN},
  {STR("goto"),     C_TOKEN_KEYWORD_GOTO},
  {STR("struct"),   C_TOKEN_KEYWORD_STRUCT},
  {STR("enum"),     C_TOKEN_KEYWORD_ENUM},
  {STR("union"),    C_TOKEN_KEYWORD_UNION},
  {STR("typedef"),  C_TOKEN_KEYWORD_TYPEDEF},
  {STR("const"),    C_TOKEN_KEYWORD_CONST},
  {STR("static"),   C_TOKEN_KEYWORD_STATIC},
  {STR("extern"),   C_TOKEN_KEYWORD_EXTERN},
  {STR("inline"),   C_TOKEN_KEYWORD_INLINE},
  {STR("sizeof"),   C_TOKEN_KEYWORD_SIZEOF},
  {STR("register"), C_TOKEN_KEYWORD_REGISTER},
  {STR("void"),     C_TOKEN_KEYWORD_VOID},
  {STR("char"),     C_TOKEN_KEYWORD_CHAR},
  {STR("short"),    C_TOKEN_KEYWORD_SHORT},
  {STR("int"),      C_TOKEN_KEYWORD_INT},
  {STR("long"),     C_TOKEN_KEYWORD_LONG},
  {STR("float"),    C_TOKEN_KEYWORD_FLOAT},
  {STR("double"),   C_TOKEN_KEYWORD_DOUBLE},
  {STR("unsigned"), C_TOKEN_KEYWORD_UNSIGNED},
  {STR("signed"),   C_TOKEN_KEYWORD_SIGNED},
};

DEFINE_ARRAY(C_Token);

static
b32 c_lexer_in_bounds(C_Lexer lexer, usize at)
{
  return at < lexer.source.count;
}

static
b32 c_lexer_incomplete(C_Lexer lexer)
{
  return c_lexer_in_bounds(lexer, lexer.at);
}

static
u8 *c_lexer_at(C_Lexer lexer)
{
  ASSERT(c_lexer_incomplete(lexer), "Out of bounds access in C lexer.");
  return lexer.source.v + lexer.at;
}

static
void c_lexer_advance(C_Lexer *lexer, usize count)
{
  lexer->at += count;
}

static
void c_lexer_eat_whitespace_and_comments(C_Lexer *lexer)
{
  while (c_lexer_incomplete(*lexer))
  {
    u8 curr_char = *c_lexer_at(*lexer);
    if (curr_char == '/')
    {
      c_lexer_advance(lexer, 1);

      if (c_lexer_incomplete(*lexer))
      {
        u8 next_char = *c_lexer_at(*lexer);

        if (next_char == '/') // Single line
        {
          while (c_lexer_incomplete(*lexer))
          {
            u8 c = *c_lexer_at(*lexer);
            c_lexer_advance(lexer, 1);

            if (c == '\n')
            {
              break;
            }
          }
        }
        else if (next_char == '*') // Multiple line
        {
          u8 prev = 0;

          while (c_lexer_incomplete(*lexer))
          {
            u8 c = *c_lexer_at(*lexer);
            c_lexer_advance(lexer, 1);

            if (prev == '*' && c == '/')
            {
              break;
            }

            prev = c;
          }
        }
      }
    }
    else if (char_is_whitespace(curr_char))
    {
      c_lexer_advance(lexer, 1);
    }
    else
    {
      break;
    }
  }
}

static
C_Token_Array tokenize_c_code(Arena *arena, String code)
{
  C_Token_Array result = {0};

  C_Lexer lexer =
  {
    .source = code,
    .at = 0,
  };

  // TODO: Not thrilled about this comma operator here, but can't think of anything nicer
  // to do yet
  while (c_lexer_eat_whitespace_and_comments(&lexer), c_lexer_incomplete(lexer))
  {
    u8 curr_char = *c_lexer_at(lexer);

    C_Token token = {0};

    if (curr_char < STATIC_COUNT(c_token_table))
    {
      token.type = c_token_table[curr_char][0];
    }

    if (token.type != C_TOKEN_NONE) // Match from the table!
    {
      b32 could_be_double = c_token_table[curr_char][1] != C_TOKEN_NONE; // e.g. &&
      b32 could_be_assign = c_token_table[curr_char][2] != C_TOKEN_NONE; // e.g. &=

      usize token_length = 1; // Could actually be 2 after we check the above cases!
      if ((could_be_double || could_be_assign) && c_lexer_in_bounds(lexer, lexer.at + 1))
      {
        u8 next_char = lexer.source.v[lexer.at + 1];
        if (could_be_double && next_char == curr_char)
        {
          token.type = c_token_table[curr_char][1];
          token_length = 2;
        }
        else if (could_be_assign && next_char == '=')
        {
          token.type = c_token_table[curr_char][2];
          token_length = 2;
        }
      }

      token.raw = string_substring(lexer.source, lexer.at, lexer.at + token_length);
    }
    else
    {
      if (char_is_alphabetic(curr_char) || curr_char == '_') // Identifier or keyword
      {
        usize end = lexer.at + 1;

        while (c_lexer_in_bounds(lexer, end))
        {
          u8 c = lexer.source.v[end];
          if (char_is_alphabetic(c) || char_is_digit(c) || c == '_')
          {
            end++;
          }
          else
          {
            break;
          }
        }

        token.raw  = string_substring(lexer.source, lexer.at, end);

        // Check for keyword match, probably too few keywords to see benefit from hashing
        for (usize keyword_idx = 0; keyword_idx < STATIC_COUNT(c_keywords); keyword_idx += 1)
        {
          C_Keyword_Info keyword = c_keywords[keyword_idx];

          if (string_match(keyword.string, token.raw))
          {
            token.type = c_keywords[keyword_idx].type;
          }
        }

        // Not a keyword, must be an identifier
        if (token.type == C_TOKEN_NONE)
        {
          token.type = C_TOKEN_IDENTIFIER;
        }
      }
      else if (curr_char == '"') // String literal
      {
        usize end = lexer.at + 1;

        usize escape_count = 0;
        while (c_lexer_in_bounds(lexer, end))
        {
          u8 c = lexer.source.v[end];
          if (c == '"' && escape_count % 2 == 0)
          {
            end++;
            break;
          }
          else if (c == '\\')
          {
            end++;
            escape_count++;
          }
          else
          {
            end++;
            escape_count = 0; // We encountered something else, reset escape count
          }
        }

        token.type = C_TOKEN_LITERAL_STRING;
        token.raw  = string_substring(lexer.source, lexer.at, end);
      }
      else if (curr_char == '\'') // Character literal
      {
        usize end = lexer.at + 1;

        u8 prev = 0;
        while (c_lexer_in_bounds(lexer, end))
        {
          u8 c = lexer.source.v[end];
          if (c == '\'' && prev != '\\')
          {
            end++;
            break;
          }
          prev = c;
          end++;
        }

        token.type = C_TOKEN_LITERAL_CHAR;
        token.raw  = string_substring(lexer.source, lexer.at, end);
      }

      // TODO: Simplify?
      else if (char_is_digit(curr_char)) // Number literal
      {
        token.type = C_TOKEN_LITERAL_INT; // By default

        usize end = lexer.at + 1;
        while (c_lexer_in_bounds(lexer, end))
        {
          u8 c = lexer.source.v[end];
          if (c == 'x' || c == 'X')
          {
            end++;
            // TODO: Base
          }
          else if (c == 'b' || c == 'B')
          {
            end++;
            // TODO: Base
          }
          else if (char_is_digit(c))
          {
            end++;
          }
          else if (c == '.' || c == 'E' || c == 'e')
          {
            token.type = C_TOKEN_LITERAL_DOUBLE;
            end++;
          }
          else if (c == 'f' || c == 'F') // float, marks end, so break
          {
            token.type = C_TOKEN_LITERAL_FLOAT;
            end++;
            break;
          }
          else if (c == 'u' || c == 'U') // unsigned, unsigned long, or unsigned long long, marks the end, so break after
          {
            token.type = C_TOKEN_LITERAL_UNSIGNED_INT;
            end++;

            if (c_lexer_in_bounds(lexer, end))
            {
              c = lexer.source.v[end];
              if (c == 'l' || c == 'L')
              {
                token.type = C_TOKEN_LITERAL_UNSIGNED_LONG;
                end++;

                if (c_lexer_in_bounds(lexer, end))
                {
                  c = lexer.source.v[end];
                  if (c == 'L')
                  {
                    end++;
                    token.type = C_TOKEN_LITERAL_UNSIGNED_LONG_LONG;
                  }
                }
              }
            }

            break;
          }
          else if (c == 'l' || c == 'L') // long or long long, marks the end, so break after
          {
            token.type = C_TOKEN_LITERAL_LONG;
            end++;

            if (c_lexer_in_bounds(lexer, end))
            {
              c = lexer.source.v[end];
              if (c == 'l' || c == 'L')
              {
                token.type = C_TOKEN_LITERAL_LONG_LONG;
                end++;
              }
            }

            break;
          }
          else
          {
            break;
          }
        }

        token.raw = string_substring(lexer.source, lexer.at, end);
      }
    }

    usize advance = 1; // We'll just skip ahead if we haven't gotten a real token... maybe not best plan?
    if (token.type != C_TOKEN_NONE)
    {
      array_add(arena, result, token);
      advance = token.raw.count;
    }

    c_lexer_advance(&lexer, advance);
  }

  return result;
}
