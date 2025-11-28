#include "common.h"
#include "benchmark/profile.h"

#define JSON_Token_Type(X)         \
  X(JSON_TOKEN_EOF)                \
  X(JSON_TOKEN_OPEN_CURLY_BRACE)   \
  X(JSON_TOKEN_CLOSE_CURLY_BRACE)  \
  X(JSON_TOKEN_OPEN_SQUARE_BRACE)  \
  X(JSON_TOKEN_CLOSE_SQUARE_BRACE) \
  X(JSON_TOKEN_COMMA)              \
  X(JSON_TOKEN_COLON)              \
  X(JSON_TOKEN_STRING)             \
  X(JSON_TOKEN_NUMBER)             \
  X(JSON_TOKEN_TRUE)               \
  X(JSON_TOKEN_FALSE)              \
  X(JSON_TOKEN_NULL)               \
  X(JSON_TOKEN_COUNT)

ENUM_TABLE(JSON_Token_Type);

typedef struct JSON_Token JSON_Token;
struct JSON_Token
{
  JSON_Token_Type type;
  String          value;
};

typedef struct JSON_Object JSON_Object;
struct JSON_Object
{
  String key;   // Not required for arrays, or the outer most object
  String value;

  JSON_Object *first_child;
  JSON_Object *next_sibling;
};

typedef struct JSON_Parser JSON_Parser;
struct JSON_Parser
{
  String source;
  usize  at;
  b32    had_error;
};

static
u8 *parser_at(JSON_Parser *parser)
{
  ASSERT(parser->at < parser->source.count, "Attempted to read past parser source");
  return parser->source.v + parser->at;
}

// TODO: Use this to peek so don't have to always advance one at a time
static
u8 *parser_peek(JSON_Parser *parser, usize advance)
{
  return parser_at(parser) + advance;
}

static
void parser_advance(JSON_Parser *parser, usize advance)
{
  parser->at += advance;
}

static
b8 parser_incomplete(JSON_Parser *parser)
{
  return parser->at < parser->source.count && !parser->had_error;
}

static
b8 parser_token_is_literal(JSON_Parser *parser, String literal_string)
{
  return MEM_MATCH(parser_at(parser), literal_string.v, literal_string.count);
}

static
b32 is_numeric(u8 ch)
{
  return char_is_digit(ch) || ch == '.' || ch == '-';
}

static
JSON_Token get_json_token(JSON_Parser *parser)
{
  JSON_Token token = {0};

  // Eat white spaces
  while (parser_incomplete(parser)  &&
         (*parser_at(parser) == ' '  ||
          *parser_at(parser) == '\n' ||
          *parser_at(parser) == '\r' ||
          *parser_at(parser) == '\t'))
  {
    // TODO: Probably just count all white space and then advance once for all at the end
    parser_advance(parser, 1);
  }

  if (parser_incomplete(parser)) // If we've not reached the end of file
  {
    token.value.v = parser_at(parser);
    switch (*parser_at(parser))
    {
      case '{':
      {
        token.type = JSON_TOKEN_OPEN_CURLY_BRACE;
        token.value.count = 1;
        parser_advance(parser, 1);
      }
      break;
      case '}':
      {
        token.type = JSON_TOKEN_CLOSE_CURLY_BRACE;
        token.value.count = 1;
        parser_advance(parser, 1);
      }
      break;
      case '[':
      {
        token.type = JSON_TOKEN_OPEN_SQUARE_BRACE;
        token.value.count = 1;
        parser_advance(parser, 1);
      }
      break;
      case ']':
      {
        token.type = JSON_TOKEN_CLOSE_SQUARE_BRACE;
        token.value.count = 1;
        parser_advance(parser, 1);
      }
      break;
      case ',':
      {
        token.type = JSON_TOKEN_COMMA;
        token.value.count = 1;
        parser_advance(parser, 1);
      }
      break;
      case ':':
      {
        token.type = JSON_TOKEN_COLON;
        token.value.count = 1;
        parser_advance(parser, 1);
      }
      break;
      case '"':
      {
        token.type = JSON_TOKEN_STRING;

        parser_advance(parser, 1); // For the quotation mark

        token.value.v = parser_at(parser); // Special case, we want the start to ignore the "

        // TODO: account for escaped quotes
        usize string_count = 0;
        while (*parser_at(parser) != '"')
        {
          string_count += 1;
          parser_advance(parser, 1);
        }
        token.value.count = string_count;

        parser_advance(parser, 1); // For the other quotation mark
      }
      break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '.':
      case '-':
      {
        token.type  = JSON_TOKEN_NUMBER;

        usize digit_count = 0;
        while (is_numeric(*parser_at(parser)))
        {
          digit_count += 1;
          parser_advance(parser, 1);
        }
        token.value.count = digit_count;
      }
      break;
      case 't':
      {
        String string = String("true");
        if (parser_token_is_literal(parser, string))
        {
          // No 'value' for this
          token.type = JSON_TOKEN_TRUE;
          parser_advance(parser, string.count);
        }
        else
        {
          LOG_ERROR("Encountered unrecognized literal at byte %lu", parser->at);
          parser->had_error = true;
        }
      }
      break;
      case 'f':
      {
        String string = String("false");
        if (parser_token_is_literal(parser, string))
        {
          // No 'value' for this
          token.type = JSON_TOKEN_FALSE;
          parser_advance(parser, string.count);
        }
        else
        {
          LOG_ERROR("Encountered unrecognized literal at byte %lu", parser->at);
          parser->had_error = true;
        }
      }
      break;
      case 'n':
      {
        String string = String("null");
        if (parser_token_is_literal(parser, string))
        {
          // No 'value' for this
          token.type = JSON_TOKEN_NULL;
          parser_advance(parser, string.count);
        }
        else
        {
          LOG_ERROR("Encountered unrecognized literal at byte %lu", parser->at);
          parser->had_error = true;
        }
      }
      break;
    }
  }
  else
  {
    // EOF, do nothing else
    parser_advance(parser, 1);
  }

  return token;
}

static
b32 json_token_type_is_value_type(JSON_Token_Type type)
{
  b32 is_value_type = (type == JSON_TOKEN_STRING ||
                       type == JSON_TOKEN_TRUE   ||
                       type == JSON_TOKEN_FALSE  ||
                       type == JSON_TOKEN_NULL   ||
                       type == JSON_TOKEN_NUMBER);
  return is_value_type;
}

static
JSON_Object *parse_json_children(Arena *arena, JSON_Parser *parser,
                                 JSON_Token_Type end_token, b32 has_keys);

static
JSON_Object *parse_json_object(Arena *arena, JSON_Parser *parser, String key, JSON_Token token)
{
  profile_begin_func();

  JSON_Object *first_child = NULL;

  if (token.type == JSON_TOKEN_OPEN_CURLY_BRACE)
  {
    // Normal key : value pairs
    b32 has_keys = true;
    first_child = parse_json_children(arena, parser, JSON_TOKEN_CLOSE_CURLY_BRACE, has_keys);
  }
  else if (token.type == JSON_TOKEN_OPEN_SQUARE_BRACE)
  {
    // Array, no key
    b32 has_keys = false;
    first_child = parse_json_children(arena, parser, JSON_TOKEN_CLOSE_SQUARE_BRACE, has_keys);
  }
  // else it should be a leaf node containing a value only, not an array or table
  else if (json_token_type_is_value_type(token.type))
  {
    // Do nothing, no children to parse
  }
  else
  {
    LOG_ERROR("Unexpected token type encountered while parsing json object: %s, (value = %.*s)", JSON_Token_Type_strings[token.type], token.value);
          parser->had_error = true;
  }

  JSON_Object *result  = arena_new(arena, JSON_Object);
  result->key          = key;
  result->first_child  = first_child;
  result->next_sibling = NULL;
  result->value        = token.value;

  profile_close_func();

  return result;
}

static
JSON_Object *parse_json_children(Arena *arena, JSON_Parser *parser,
                                 JSON_Token_Type end_token, b32 has_keys)
{
  profile_begin_func();

  JSON_Object *first_child        = NULL;
  JSON_Object *current_last_child = NULL;

  while (parser_incomplete(parser))
  {
    JSON_Token key_token = {0};
    JSON_Token value_token = {0};

    // Grab in the key, if needed, and it's value
    if (has_keys)
    {
      key_token = get_json_token(parser);

      if (key_token.type == JSON_TOKEN_STRING)
      {
        JSON_Token expect_colon = get_json_token(parser);

        if (expect_colon.type == JSON_TOKEN_COLON)
        {
          value_token = get_json_token(parser);
        }
        else
        {
          LOG_ERROR("Expected colon after key: %*.s", String_Format(key_token.value));
          parser->had_error = true;
        }
      }
      else
      {
        LOG_ERROR("Unexpected key type: %s, (value = %*.s)", JSON_Token_Type_strings[key_token.type], String_Format(key_token.value));
        parser->had_error = true;
      }
    }
    // Its just values and no keys
    else
    {
      value_token = get_json_token(parser);
    }

    if (value_token.type == end_token)
    {
      // We're done
      break;
    }

    JSON_Object *object = parse_json_object(arena, parser, key_token.value, value_token);
    if (object)
    {
      // Create links
      if (!first_child)
      {
        first_child        = object;
        current_last_child = object;
      }
      else
      {
        current_last_child->next_sibling = object;
        current_last_child = object;
      }
    }

    // We either want the end, or a comma
    JSON_Token expect_comma_or_end = get_json_token(parser);
    if (expect_comma_or_end.type == end_token)
    {
      break;
    }
    else if (expect_comma_or_end.type != JSON_TOKEN_COMMA)
    {
      LOG_ERROR("Expected comma, parsed Token :: Type = %s, Value = '%.*s', \n", JSON_Token_Type_strings[expect_comma_or_end.type],
                String_Format(expect_comma_or_end.value), expect_comma_or_end);
        parser->had_error = true;
    }
  }

  profile_close_func();

  return first_child;
}

// Returns the very first object
static
JSON_Object *parse_json(Arena *arena, String source)
{
  profile_begin_func();

  JSON_Parser parser =
  {
    .source = source,
    .at     = 0,
  };

  JSON_Object *outer = parse_json_object(arena, &parser, (String){0}, get_json_token(&parser));

  profile_close_func();

  if (parser.had_error)
  {

  }

  return outer;
}

static
JSON_Object *lookup_json_object(JSON_Object *current, String key)
{
  profile_begin_func();

  JSON_Object *result = NULL;

  if (current)
  {
    for (JSON_Object *cursor = current->first_child; cursor; cursor = cursor->next_sibling)
    {
      if (string_match(key, cursor->key))
      {
        result = cursor;
        break;
      }
    }
  }

  profile_close_func();

  return result;
}

static
f64 json_object_to_f64(JSON_Object *object)
{
  f64 result = 0.0;

  if (object)
  {
    String val = object->value;
    // Get sign.
    usize at = 0;

    f64 sign = 1.0;
    if (val.count > at && val.v[at] == '-')
    {
      sign = -1.0;
      at += 1;
    }

    // Before decimal
    while (at < val.count)
    {
      u8 digit = val.v[at] - (u8)'0';
      if (digit < 10)
      {
        // We go left to right so each previous result is 10 times bigger
        result = 10 * result + (f64)digit;
        at += 1;
      }
      else // Not a digit
      {
        break;
      }
    }

    // After decimal (if there)
    if (at < val.count && val.v[at] == '.')
    {
      at += 1;

      f64 factor = 1.0 / 10.0;
      while (at < val.count)
      {
        u8 digit = val.v[at] - (u8)'0';
        if (digit < 10)
        {
          // We go left to right so each additional digit is 10 times smaller
          result = result + factor * (f64)digit;
          factor *= 1.0 / 10.0;
          at += 1;
        }
        else // Not a digit
        {
          break;
        }
      }
    }

    // Exponent
    if (at < val.count && (val.v[at] == 'e' || val.v[at] == 'E'))
    {
      at += 1;

      f64 e_sign = 1;
      if (at < val.count && val.v[at] == '-')
      {
        e_sign = -1;
      }

      f64 exponent = 0.0;
      while (at < val.count)
      {
        u8 digit = val.v[at] - (u8)'0';
        if (digit < 10)
        {
          // We go left to right so each previous result is 10 times bigger
          exponent = 10 * exponent + (f64)digit;
          at += 1;
        }
        else // Not a digit
        {
          break;
        }
      }

      result *= pow(10.0, e_sign * exponent);
    }

    result *= sign;
  }

  return result;
}
