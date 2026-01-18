#include "common.h"

#include "c_tokenize.h"

static
void c_lexer_push_token(Arena *arena, C_Token_Chunk_List *list, C_Token token)
{
  // Check if no chunks or need a new chunk
  if (!list->last || list->last->count >= STATIC_COUNT(list->last->values))
  {
    C_Token_Chunk *new_chunk = arena_new(arena, C_Token_Chunk);
    list_push_last(list, new_chunk);
  }

  list->last->values[list->last->count] = token;
  list->last->count += 1;
}

// Single character tokens... some special logic required for e.g. =,&,| (could be ==,&&,||)
// In those cases we check and then grab the 2nd token type of the triple
// As well for operation assignment tokens (+=, -=, &=, etc.)
// We use the 3rd of the triple
static C_Token_Type c_token_table[][3] =
{
  ['('] = {C_TOKEN_BEGIN_PARENTHESIS,  C_TOKEN_NONE,          C_TOKEN_NONE},
  [')'] = {C_TOKEN_CLOSE_PARENTHESIS,  C_TOKEN_NONE,          C_TOKEN_NONE},
  ['{'] = {C_TOKEN_BEGIN_CURLY_BRACE,  C_TOKEN_NONE,          C_TOKEN_NONE},
  ['}'] = {C_TOKEN_CLOSE_CURLY_BRACE,  C_TOKEN_NONE,          C_TOKEN_NONE},
  ['['] = {C_TOKEN_BEGIN_SQUARE_BRACE, C_TOKEN_NONE,          C_TOKEN_NONE},
  [']'] = {C_TOKEN_CLOSE_SQUARE_BRACE, C_TOKEN_NONE,          C_TOKEN_NONE},

  ['+'] = {C_TOKEN_ADD,                C_TOKEN_INCREMENT,     C_TOKEN_ADD_ASSIGN},
  ['-'] = {C_TOKEN_MINUS,              C_TOKEN_DECREMENT,     C_TOKEN_SUBTRACT_ASSIGN},
  ['*'] = {C_TOKEN_STAR,               C_TOKEN_NONE,          C_TOKEN_MULTIPLY_ASSIGN},
  ['/'] = {C_TOKEN_DIVIDE,             C_TOKEN_NONE,          C_TOKEN_DIVIDE_ASSIGN},
  ['%'] = {C_TOKEN_MODULO,             C_TOKEN_NONE,          C_TOKEN_MODULO_ASSIGN},
  ['~'] = {C_TOKEN_BITWISE_NOT,        C_TOKEN_NONE,          C_TOKEN_NONE},
  ['^'] = {C_TOKEN_XOR,                C_TOKEN_NONE,          C_TOKEN_XOR_ASSIGN},
  ['&'] = {C_TOKEN_BITWISE_AND,        C_TOKEN_LOGICAL_AND,   C_TOKEN_AND_ASSIGN},
  ['|'] = {C_TOKEN_BITWISE_OR,         C_TOKEN_LOGICAL_OR,    C_TOKEN_OR_ASSIGN},
  ['='] = {C_TOKEN_ASSIGN,             C_TOKEN_COMPARE_EQUAL, C_TOKEN_NONE},
  [','] = {C_TOKEN_COMMA,              C_TOKEN_NONE,          C_TOKEN_NONE},
  [';'] = {C_TOKEN_SEMICOLON,          C_TOKEN_NONE,          C_TOKEN_NONE},
  ['.'] = {C_TOKEN_DOT,                C_TOKEN_NONE,          C_TOKEN_NONE},

  ['<'] = {C_TOKEN_LESS_THAN,          C_TOKEN_NONE,          C_TOKEN_LESS_THAN_EQUAL},
  ['>'] = {C_TOKEN_GREATER_THAN,       C_TOKEN_NONE,          C_TOKEN_GREATER_THAN_EQUAL},
  ['!'] = {C_TOKEN_LOGICAL_NOT,        C_TOKEN_NONE,          C_TOKEN_COMPARE_NOT_EQUAL},

  ['?'] = {C_TOKEN_QUESTION,           C_TOKEN_NONE,          C_TOKEN_NONE},
  [':'] = {C_TOKEN_COLON,              C_TOKEN_NONE,          C_TOKEN_NONE},
};

// For checking character escapes, just took the list from wikipedia
static u8 c_char_escape_table[] =
{
  ['a'] = '\a',
  ['b'] = '\b',
  ['e'] = '\e',
  ['f'] = '\f',
  ['n'] = '\n',
  ['r'] = '\r',
  ['t'] = '\t',
  ['v'] = '\v',
  ['\\'] = '\\',
  ['\''] = '\'',
  ['\"'] = '\"',
  ['\?'] = '\?',
};

typedef struct C_Keyword_Info C_Keyword_Info;
struct C_Keyword_Info
{
  String string;
  C_Token_Type type;
};

static C_Keyword_Info c_keyword_table[] =
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
  {STR("restrict"), C_TOKEN_KEYWORD_RESTRICT},
  {STR("volatile"), C_TOKEN_KEYWORD_VOLATILE},
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

static
b32 c_lexer_in_bounds(C_Lexer lexer, usize at)
{
  return at < lexer.source.count;
}

// Returns 0 if not in bounds
u8 c_lexer_peek_at(C_Lexer lexer, usize at)
{
  u8 result = 0;

  if (c_lexer_in_bounds(lexer, at))
  {
    result = lexer.source.v[at];
  }

  return result;
}

static
b32 c_lexer_incomplete(C_Lexer lexer)
{
  return c_lexer_in_bounds(lexer, lexer.at);
}

static
u8 *c_lexer_curr_at(C_Lexer lexer)
{
  ASSERT(c_lexer_incomplete(lexer), "Out of bounds access in C lexer.");
  return lexer.source.v + lexer.at;
}

static
void c_lexer_advance(C_Lexer *lexer, usize count)
{
  lexer->columns_processed += count;
  lexer->at += count;
}

// NOTE: Will move at forward if needed, why it takes a pointer to at.
static
u8 c_lexer_evaluate_escape(C_Lexer *lexer, usize *peek)
{
  u8 result = 0;

  if (c_lexer_in_bounds(*lexer, *peek))
  {
    u8 c = lexer->source.v[*peek];

    u8 byte_base = 0;
    if (c == 'x') // Hex byte escape
    {
      byte_base = 16;
      *peek += 1; // Skip over the x
    }
    else if (char_is_digit_base(c, 8)) // Octal byte escape
    {
      byte_base = 8;
    }

    if (byte_base) // We have a raw byte escape sequence, grab them digits
    {
      while (c_lexer_in_bounds(*lexer, *peek) &&
             char_is_digit_base(lexer->source.v[*peek], byte_base))
      {
        u8 digit = char_to_digit_base(lexer->source.v[*peek], byte_base);
        result = (byte_base * result) + digit;
        *peek += 1;
      }
    }
    else if (c < STATIC_COUNT(c_char_escape_table)) // Just a normal escape
    {
      result = c_char_escape_table[c];
      *peek += 1;
    }
  }

  return result;
}

static
void c_lexer_eat_whitespace_comments_preprocessor(C_Lexer *lexer)
{
  while (c_lexer_incomplete(*lexer))
  {
    u8 curr_char = *c_lexer_curr_at(*lexer);

    // Count lines... reset columns, advance
    if (curr_char == '\n')
    {
      c_lexer_advance(lexer, 1);
      lexer->lines_processed += 1;
      lexer->columns_processed = 0;
    }
    else if (curr_char == '/')
    {
      u8 next_char = c_lexer_peek_at(*lexer, lexer->at + 1);

      if (next_char == '/') // Single line
      {
        while (c_lexer_incomplete(*lexer))
        {
          u8 c = *c_lexer_curr_at(*lexer);

          if (c == '\n')
          {
            // Leave dealing with new line to outer loop...
            // centralize logic related to counting lines and columns
            break;
          }

          c_lexer_advance(lexer, 1);
        }
      }
      else if (next_char == '*') // Multiple line
      {
        u8 prev = 0;

        while (c_lexer_incomplete(*lexer))
        {
          u8 c = *c_lexer_curr_at(*lexer);

          // FIXME: Not thrilled about having to handle new lines in more than one place.
          if (c == '\n')
          {
            lexer->lines_processed += 1;
            lexer->columns_processed = 0;
          }

          c_lexer_advance(lexer, 1);

          // Stick with the normal c behavior where you can't nest these :(
          // could just keep a counter of how many begin blocks we find...
          // but that's not what c does...
          if (prev == '*' && c == '/')
          {
            break;
          }

          prev = c;
        }
      }
      else
      {
        break;
      }
    }
    else if (char_is_whitespace(curr_char)) // Catch all other whitespace
    {
      c_lexer_advance(lexer, 1);
    }
    else if (curr_char == '#')
    {
      while (c_lexer_incomplete(*lexer))
      {
        u8 c = *c_lexer_curr_at(*lexer);

        if (c == '\n')
        {
          break;
        }

        c_lexer_advance(lexer, 1);
      }
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
  C_Token_Chunk_List chunks = {0};

  C_Lexer lexer =
  {
    .source = code,
    .at = 0,
  };

  while (c_lexer_eat_whitespace_comments_preprocessor(&lexer), c_lexer_incomplete(lexer))
  {
    u8 curr_char = *c_lexer_curr_at(lexer);

    C_Token token = {0};
    token.line   = lexer.lines_processed + 1;
    token.column = lexer.columns_processed + 1;

    if (curr_char < STATIC_COUNT(c_token_table))
    {
      token.type = c_token_table[curr_char][0];
    }

    if (token.type != C_TOKEN_NONE) // Match from the table!
    {
      b32 could_be_arrow  = c_token_table[curr_char][0] == C_TOKEN_MINUS; // only ->
      b32 could_be_double = c_token_table[curr_char][1] != C_TOKEN_NONE;  // e.g. &&
      b32 could_be_equals = c_token_table[curr_char][2] != C_TOKEN_NONE;  // e.g. &=

      usize token_length = 1; // Could actually be 2 after we check the above cases!
      if ((could_be_double || could_be_equals || could_be_arrow))
      {
        u8 next_char = c_lexer_peek_at(lexer, lexer.at + 1);

        if (could_be_double && next_char == curr_char)
        {
          token.type = c_token_table[curr_char][1];
          token_length = 2;
        }
        else if (could_be_equals && next_char == '=')
        {
          token.type = c_token_table[curr_char][2];
          token_length = 2;
        }
        else if (could_be_arrow && next_char == '>')
        {
          token.type = C_TOKEN_ARROW;
          token_length = 2;
        }
      }

      token.raw = string_substring(lexer.source, lexer.at, lexer.at + token_length);
    }
    else if (char_is_alphabetic(curr_char) || curr_char == '_') // Identifier or keyword
    {
      token.type = C_TOKEN_IDENTIFIER; // By default an identifier

      usize end = lexer.at + 1;

      while (c_lexer_in_bounds(lexer, end))
      {
        u8 c = lexer.source.v[end];
        if (char_is_alphabetic(c) || char_is_digit(c) || c == '_')
        {
          end += 1;
        }
        else
        {
          break;
        }
      }

      token.raw  = string_substring(lexer.source, lexer.at, end);

      // Check for keyword match, probably too few keywords to see benefit from hashing
      for (usize keyword_idx = 0; keyword_idx < STATIC_COUNT(c_keyword_table); keyword_idx += 1)
      {
        C_Keyword_Info keyword = c_keyword_table[keyword_idx];

        if (string_match(keyword.string, token.raw))
        {
          token.type = c_keyword_table[keyword_idx].type;
        }
      }
    }
    else if (curr_char == '"') // String literal
    {
      token.type = C_TOKEN_LITERAL_STRING;

      usize end = lexer.at + 1;

      String literal = {0};

      while (c_lexer_in_bounds(lexer, end))
      {
        u8 c = lexer.source.v[end];
        end += 1;

        if (c == '"')
        {
          break;
        }
        else if (c == '\\') // Escapes
        {
          c = c_lexer_evaluate_escape(&lexer, &end); // Will push end
        }
        else if (c == '\n') // Uh, oh
        {
          token.type = C_TOKEN_NONE;
          LOG_ERROR("Encountered string literal without closing quotation mark on same line.");
          break;
        }

        array_add(arena, literal, c);
      }

      token.string_literal = literal;
      token.raw = string_substring(lexer.source, lexer.at, end);
    }
    else if (curr_char == '\'') // Character literal
    {
      usize end = lexer.at + 1;

      u8 c = c_lexer_peek_at(lexer, end);

      if (c != '\'') // Should not get an empty ''
      {
        if (c == '\\') // Shit, escape... figure out which it is
        {
          end += 1;

          c = c_lexer_peek_at(lexer, end);

          u8 byte_base = 0;
          if (c == 'x') // Hex byte escape
          {
            byte_base = 16;
            end += 1; // Skip over the x
          }
          else if (char_is_digit_base(c, 8)) // Octal byte escape
          {
            byte_base = 8;
          }

          if (byte_base) // We have a raw byte escape sequence, grab them digits
          {
            while (c_lexer_in_bounds(lexer, end) &&
                   char_is_digit_base(lexer.source.v[end], byte_base))
            {
              u8 digit = char_to_digit_base(lexer.source.v[end], byte_base);
              token.char_literal = (byte_base * token.char_literal) + digit;
              end += 1;
            }
          }
          else if (c < STATIC_COUNT(c_char_escape_table)) // Just a normal escape
          {
            token.char_literal = c_char_escape_table[c];
            end += 1;
          }
        }
        else // No escapes, just the character
        {
          token.char_literal = c;
          end += 1;
        }

        c = c_lexer_peek_at(lexer, end);
        if (c == '\'') // Should end with a '
        {
          end += 1;
        }
        else
        {
          token.type = C_TOKEN_NONE;
          LOG_ERROR("Encountered unterminated character literal.");
        }

        token.type = C_TOKEN_LITERAL_CHAR;
        token.raw  = string_substring(lexer.source, lexer.at, end);
      }
      else
      {
        LOG_ERROR("Encountered empty char literal.");
      }
    }
    else if (char_is_digit(curr_char)) // Number literal
    {
      // By default
      token.type = C_TOKEN_LITERAL_INT;
      token.int_literal.base = 10;

      usize end = lexer.at;

      if (curr_char == '0')
      {
        u8 c = c_lexer_peek_at(lexer, end + 1);
        if (c == 'x' || c == 'X')
        {
          token.int_literal.base = 16;
          end += 2;
        }
        else if (c == 'b' || c == 'B')
        {
          token.int_literal.base = 2;
          end += 2;
        }
      }

      b32 had_digit = false;

      while (c_lexer_in_bounds(lexer, end) && char_is_digit_base(lexer.source.v[end], token.int_literal.base))
      {
        u64 digit = char_to_digit_base(lexer.source.v[end], token.int_literal.base);
        u64 base  = token.int_literal.base;
        token.int_literal.v = base * token.int_literal.v + digit;
        end += 1;

        had_digit = true;
      }

      // Different base literals must have a digit following the base signifier,
      // e.g. 0x1 valid but 0x not valid
      if (!had_digit && token.int_literal.base != 10)
      {
        token.type = C_TOKEN_NONE;
        String base_string = token.int_literal.base == 2 ? STR("Hexadecimal") : STR("Binary");
        LOG_ERROR("%.*s integer literals must include a digit following the x", STRF(base_string));
      }

      // Collect decimals if present and haven't changed base
      if (token.int_literal.base == 10 &&
          c_lexer_peek_at(lexer, end) == '.')
      {
        token.type = C_TOKEN_LITERAL_DOUBLE;
        end += 1;

        token.float_literal = (f64)token.int_literal.v;

        f64 factor = 1.0 / 10.0;
        while (c_lexer_in_bounds(lexer, end) && char_is_digit(lexer.source.v[end]))
        {
          u64 digit = lexer.source.v[end] - '0';
          token.float_literal = (f64)token.float_literal + (factor * (f64)digit);
          factor *= 1.0 / 10.0;

          end += 1;
        }
      }

      // A floating point literal can have an Exponent part without also having a decimal,
      // so check also if we have an integer base 10 so far
      b32 maybe_exponent = token.type == C_TOKEN_LITERAL_DOUBLE || token.int_literal.base == 10;

      if (maybe_exponent &&
          (c_lexer_peek_at(lexer, end) == 'e' || c_lexer_peek_at(lexer, end) == 'E'))
      {
        // If we haven't gotten a decimal yet, we need to do conversion to float here
        if (token.type == C_TOKEN_LITERAL_INT)
        {
          token.float_literal = (f64)token.int_literal.v;
        }

        token.type = C_TOKEN_LITERAL_DOUBLE;
        end += 1;

        f64 sign = 1;
        if (c_lexer_peek_at(lexer, end) == '-')
        {
          sign = -1;
          end += 1;
        }

        b32 valid_exponent = false; // Needs to have a digit after E

        f64 exponent = 0.0;
        while (c_lexer_in_bounds(lexer, end) && char_is_digit(lexer.source.v[end]))
        {
          u64 digit = char_to_digit(lexer.source.v[end]);
          exponent = 10 * exponent + digit;
          end += 1;

          valid_exponent = true;
        }

        if (valid_exponent)
        {
          token.float_literal *= pow(10.0, sign * exponent);
        }
        else
        {
          token.type = C_TOKEN_NONE; // Hmm should we discard this token?
          LOG_ERROR("Float literal with exponent must include digits following the E");
        }
      }

      if (token.type != C_TOKEN_NONE) // Chech for f, ul, ll, LL, etc
      {
        u8 c = c_lexer_peek_at(lexer, end);

        if (token.type == C_TOKEN_LITERAL_DOUBLE)
        {
          if (c == 'f' || c == 'F')
          {
            token.flags |= C_TOKEN_FLAG_LITERAL_FLOAT; // Float literal
            end += 1;
          }
          // NOTE: Apparently there are long doubles? Not going to bother with with actually trying to grab the full 80 (128?) bits
          else if (c == 'l' || c == 'L')
          {
            token.flags |= C_TOKEN_FLAG_LITERAL_LONG;
            end += 1;
          }
        }
        else if (token.type == C_TOKEN_LITERAL_INT)
        {
          if (c == 'u' || c == 'U')
          {
            token.flags |= C_TOKEN_FLAG_LITERAL_UNSIGNED;
            end += 1;
            LOG_INFO("Here: %lu", token.flags);
          }

          // TODO: I actually quite like this... make usage code less branchy, replace most cases of c_lexer_in_bounds() with this
          u8 peek = c_lexer_peek_at(lexer, end);

          // Check for long modifier... need to again check
          if (peek == 'l' || peek == 'L')
          {
            token.flags |= C_TOKEN_FLAG_LITERAL_LONG;
            end += 1;

            peek = c_lexer_peek_at(lexer, end);

            // TODO: Inelegant
            if (peek == 'l' || peek == 'L')
            {
              token.flags |= C_TOKEN_FLAG_LITERAL_2ND_LONG;
              end += 1;
            }
          }
        }
      }

      token.raw = string_substring(lexer.source, lexer.at, end);
    }

    usize advance = MAX(1, token.raw.count); // Always advance by at least 1
    if (token.type != C_TOKEN_NONE)
    {
      c_lexer_push_token(arena, &chunks, token);
    }
    else
    {
      LOG_ERROR("Unkown token encountered at line: %lu, column: %lu", token.line, token.column);
    }

    c_lexer_advance(&lexer, advance);
  }

  // Put into contiguous array for nice interface in parsing
  C_Token_Array result = {0};
  for (C_Token_Chunk *chunk = chunks.first; chunk; chunk = chunk->link_next)
  {
    for (usize i = 0; i < chunk->count; i++)
    {
      array_add(arena, result, chunk->values[i]);
    }
  }

  return result;
}
