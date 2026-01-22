#ifndef C_PARSE
#define C_PARSE

#include "common.h"

#include "c_tokenize.h"

#define C_Leaf_Type(X)      \
  X(C_LEAF_NONE)            \
  X(C_LEAF_VARIABLE)        \
  X(C_LEAF_TYPE)            \
  X(C_LEAF_LITERAL)         \
  X(C_LEAF_ROOT)            \
  X(C_LEAF_DECLARATION)     \
  X(C_LEAF_UNARY)           \
  X(C_LEAF_BINARY)          \
  X(C_LEAF_COUNT)

ENUM_TABLE(C_Leaf_Type);

typedef struct C_Leaf C_Leaf;
struct C_Leaf
{
  C_Leaf_Type type;

  C_Leaf *parent;

  C_Leaf *first_child;
  C_Leaf *last_child;
  usize  child_count;

  C_Leaf *next_sibling;
  C_Leaf *prev_sibling;

  String name;
};

typedef struct C_Parser C_Parser;
struct C_Parser
{
  C_Token_Array tokens;
  usize at;
};

// Could easily convert these to lookups
b32 c_token_is_type_keyword(C_Token token)
{
  C_Token_Type t = token.type;

  b32 result = t == C_TOKEN_KEYWORD_VOID  ||
               t == C_TOKEN_KEYWORD_CHAR  ||
               t == C_TOKEN_KEYWORD_SHORT ||
               t == C_TOKEN_KEYWORD_INT   ||
               t == C_TOKEN_KEYWORD_LONG  ||
               t == C_TOKEN_KEYWORD_FLOAT ||
               t == C_TOKEN_KEYWORD_DOUBLE;

  return result;
}

b32 c_token_is_literal(C_Token token)
{
  C_Token_Type t = token.type;

  b32 result = t == C_TOKEN_LITERAL_CHAR   ||
               t == C_TOKEN_LITERAL_INT    ||
               t == C_TOKEN_LITERAL_DOUBLE ||
               t == C_TOKEN_LITERAL_STRING;

  return result;
}

b32 c_token_is_unary_operator(C_Token token)
{
  C_Token_Type t = token.type;

  // Apparently add is also a unary?
  b32 result = t == C_TOKEN_MINUS       ||
               t == C_TOKEN_STAR        ||
               t == C_TOKEN_INCREMENT   ||
               t == C_TOKEN_DECREMENT   ||
               t == C_TOKEN_ADD         ||
               t == C_TOKEN_LOGICAL_AND ||
               t == C_TOKEN_BITWISE_NOT ||
               t == C_TOKEN_LOGICAL_NOT;

  return result;
}

// TODO: account for comma operator... perhaps a flag if we are within a parenthetical
b32 c_token_is_binary_operator(C_Token token)
{
  C_Token_Type t = token.type;

  b32 result = t == C_TOKEN_STAR ||
               t == C_TOKEN_ADD  ||
               t == C_TOKEN_MINUS ||
               t == C_TOKEN_DIVIDE ||
               t == C_TOKEN_MODULO ||
               t == C_TOKEN_XOR ||
               t == C_TOKEN_BITWISE_AND ||
               t == C_TOKEN_BITWISE_OR ||
               t == C_TOKEN_DOT ||
               t == C_TOKEN_ARROW ||
               t == C_TOKEN_BITWISE_NOT ||
               t == C_TOKEN_COMPARE_EQUAL ||
               t == C_TOKEN_LESS_THAN ||
               t == C_TOKEN_LESS_THAN_EQUAL ||
               t == C_TOKEN_GREATER_THAN ||
               t == C_TOKEN_GREATER_THAN_EQUAL ||
               t == C_TOKEN_COMPARE_NOT_EQUAL ||
               t == C_TOKEN_LOGICAL_AND ||
               t == C_TOKEN_LOGICAL_OR;


  return result;
}

// C_Leaf_Type c_token_to_binop_leaf(C_Token_Type type)
// {
//   C_Leaf_Type leaf = C_LEAF_NONE;
//
//   switch (type)
//   {
//     default:
//     {
//       LOG_ERROR("Unkown token to binop conversion");
//     }
//     case C_TOKEN_DIVIDE:
//     {
//       leaf = C_LEAF_
//     }
//     break;
//     case C_TOKEN_STAR:
//     {
//
//     }
//     break;
//   }
//
//   return leaf;
// }

void c_parse_error(usize line, const char *message)
{
  LOG_ERROR("[Line: %llu] %s", line, message);
}

C_Token c_parse_peek_token(C_Parser parser, isize offset)
{
  C_Token result = {0};

  if (parser.at + offset < parser.tokens.count && parser.at + offset >= 0)
  {
    result = parser.tokens.v[parser.at + offset];
  }

  return result;
}

void c_leaf_add_child(C_Leaf *parent, C_Leaf *child)
{
  child->parent = parent;

  // HACK: Hmm, should nil parent be possible or not?
  if (parent)
  {
    DLL_push_last(parent->first_child, parent->last_child, child, next_sibling, prev_sibling);
    parent->child_count += 1;
  }
}

b32 c_parse_incomplete(C_Parser parser)
{
  return parser.at < parser.tokens.count;
}

C_Leaf *c_parse_expression_factor(Arena *arena, C_Parser *parser, C_Leaf *parent)
{
  C_Leaf *result = arena_new(arena, C_Leaf);

  C_Token token = c_parse_peek_token(*parser, 0);

  if (c_token_is_literal(token))
  {
    result->type = C_LEAF_LITERAL;
    parser->at += 1;
  }
  else if (c_token_is_unary_operator(token))
  {
    result->type = C_LEAF_UNARY;
    parser->at += 1;

    // Grab child
    C_Leaf *unary_child = c_parse_expression_factor(arena, parser, result);
  }
  else if (token.type == C_TOKEN_IDENTIFIER)
  {
    result->type = C_LEAF_VARIABLE;
    parser->at += 1;
  }

  c_leaf_add_child(parent, result);

  return result;
}

C_Leaf *c_parse_expression(Arena *arena, C_Parser *parser, C_Leaf *parent)
{
  // We don't know the parent yet...
  // TODO: Actually think these 2 functions parse_expresssion_factor and parse_expression can become one
  C_Leaf *left = c_parse_expression_factor(arena, parser, 0);

  C_Leaf *result = left; // Return just the left if we don't meet later checks

  // TODO: Maybe move this check to parse factor and return a nil leaf if no further operators
  C_Token peek = c_parse_peek_token(*parser, 0);
  if (c_token_is_binary_operator(peek))
  {
    result = arena_new(arena, C_Leaf);
    result->type = C_LEAF_BINARY;

    parser->at += 1; // now skip the binop token

    // Left should be child of operator
    c_leaf_add_child(result, left);

    C_Leaf *right = c_parse_expression(arena, parser, result);
  }
  // catch post increment, decrement here
  else if (c_token_is_unary_operator(peek))
  {
    C_Leaf *unary_op = arena_new(arena, C_Leaf);
    unary_op->type = C_LEAF_UNARY;

    parser->at += 1;

    // Hmm, should the post increment/decrement be a child of the expression... probably
    c_leaf_add_child(result, unary_op);
  }

  c_leaf_add_child(parent, result);

  return result;
}

C_Leaf *c_parse_type(Arena *arena, C_Parser *parser, C_Leaf *parent)
{
  C_Leaf *result = arena_new(arena, C_Leaf);

  C_Token token = c_parse_peek_token(*parser, 0);

  if (c_token_is_type_keyword(token))
  {
    result->type = C_LEAF_TYPE;
    result->name = token.raw; // Hehehe, nice to just do this

    c_leaf_add_child(parent, result);
  }
  // TODO: probably should build a data structure keeping track of structs, typedefs, etc
  else if (token.type == C_TOKEN_IDENTIFIER) // Custom type
  {
  }

  // Consume
  parser->at += 1;

  return result;
}

C_Leaf *c_parse_variable(Arena *arena, C_Parser *parser, C_Leaf *parent)
{
  C_Leaf *result = arena_new(arena, C_Leaf);

  C_Token token = c_parse_peek_token(*parser, 0);

  if (token.type == C_TOKEN_IDENTIFIER)
  {
    result->type = C_LEAF_VARIABLE;
    result->name = token.raw;
    // TODO: probably need to do other stuff?
    c_leaf_add_child(parent, result);
  }

  // Consume
  parser->at += 1;

  return result;
}

C_Leaf *c_parse_declaration(Arena *arena, C_Parser *parser, C_Leaf *parent)
{
  C_Leaf *result = arena_new(arena, C_Leaf);
  result->type = C_LEAF_DECLARATION;
  c_leaf_add_child(parent, result);

  C_Leaf *type_leaf = c_parse_type(arena, parser, result);
  C_Leaf *name_leaf = c_parse_variable(arena, parser, result);

  C_Token peek = c_parse_peek_token(*parser, 0);

  if (peek.type == C_TOKEN_ASSIGN)
  {
    // Skip euqals sign
    parser->at += 1;

    // TODO: Parse expression
    c_parse_expression(arena, parser, result);
  }

  C_Token wish_semicolon = c_parse_peek_token(*parser, 0);
  if (wish_semicolon.type == C_TOKEN_SEMICOLON)
  {
    parser->at += 1;
  }
  else
  {
    c_parse_error(c_parse_peek_token(*parser, -1).line,
                  "Declaration without subsequent semicolon");
  }

  return result;
}

C_Leaf *c_parse_function(Arena *arena, C_Parser *parser, C_Leaf *parent)
{

}

// Returns root leaf
static
C_Leaf *parse_c_tokens(Arena *arena, C_Token_Array tokens)
{

  C_Leaf *root = arena_new(arena, C_Leaf);
  root->type = C_LEAF_ROOT;

  C_Parser parser =
  {
    .tokens = tokens,
    .at = 0,
  };

  while (c_parse_incomplete(parser))
  {
    usize to_consume = 0;

    C_Token token = c_parse_peek_token(parser, 0);

    // TODO: Should also check for declaration if it is an identifier that is a type too
    if (c_token_is_type_keyword(token))
    {
      C_Token peek0 = c_parse_peek_token(parser, 1);
      C_Token peek1 = c_parse_peek_token(parser, 2);

      // Declaration, probably
      if (peek0.type == C_TOKEN_IDENTIFIER)
      {
        // Function thing
        if (peek1.type == C_TOKEN_BEGIN_PARENTHESIS)
        {
          c_parse_function(arena, &parser, root);
        }
        // Variable variable thing
        else
        {
          c_parse_declaration(arena, &parser, root);
        }
      }
    }
  }

  return root;
}

#endif // C_PARSE
