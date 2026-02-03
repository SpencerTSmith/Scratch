#ifndef C_PARSE
#define C_PARSE

#include "common.h"

#include "c_tokenize.h"

// TODO:
// - Better error reporting, show line, etc. have all the info just need to stitch together.
// - IMPORTANT: Switch over completely to using nil node... no more NULL!
// - Declarations
//   - Struct
//   - Enum
//   - Typedef
// - Statements
//   - switch
//   - labels
// - Expressions
//   - Compound literals
//   - Casts
//
// - Will probably need to start having distinct types for links

#define C_Node_Type(X)           \
  X(C_NODE_NONE)                 \
  X(C_NODE_IDENTIFIER)           \
  X(C_NODE_TYPE)                 \
  X(C_NODE_LITERAL)              \
  X(C_NODE_ROOT)                 \
  X(C_NODE_VARIABLE_DECLARATION) \
  X(C_NODE_FUNCTION_DECLARATION) \
  X(C_NODE_UNARY)                \
  X(C_NODE_BINARY)               \
  X(C_NODE_TERNARY)              \
  X(C_NODE_FUNCTION_CALL)        \
  X(C_NODE_BLOCK)                \
  X(C_NODE_EXPRESSION_STATEMENT) \
  X(C_NODE_IF)                   \
  X(C_NODE_ELSE)                 \
  X(C_NODE_WHILE)                \
  X(C_NODE_RETURN)               \
  X(C_NODE_FOR)                  \
  X(C_NODE_DO_WHILE)             \
  X(C_NODE_BREAK)                \
  X(C_NODE_CONTINUE)             \
  X(C_NODE_SWITCH)               \
  X(C_NODE_CASE)                 \
  X(C_NODE_LABEL)                \
  X(C_NODE_COUNT)

ENUM_TABLE(C_Node_Type);

#define C_MIN_PRECEDENCE INT32_MIN

typedef enum C_Binary
{
  C_BINARY_NONE,

  C_BINARY_ACCESS,
  C_BINARY_POINTER_ACCESS,
  C_BINARY_ARRAY_ACCESS,
  C_BINARY_MULTIPLY,
  C_BINARY_DIVIDE,
  C_BINARY_MODULO,
  C_BINARY_ADD,
  C_BINARY_SUBTRACT,
  C_BINARY_LEFT_SHIFT,
  C_BINARY_RIGHT_SHIFT,
  C_BINARY_LESS_THAN,
  C_BINARY_LESS_THAN_EQUAL,
  C_BINARY_GREATER_THAN,
  C_BINARY_GREATER_THAN_EQUAL,
  C_BINARY_COMPARE_EQUAL,
  C_BINARY_COMPARE_NOT_EQUAL,
  C_BINARY_BITWISE_AND,
  C_BINARY_XOR,
  C_BINARY_BITWISE_OR,
  C_BINARY_LOGICAL_AND,
  C_BINARY_LOGICAL_OR,
  C_BINARY_ASSIGN,
  C_BINARY_ADD_ASSIGN,
  C_BINARY_SUBTRACT_ASSIGN,
  C_BINARY_MULTIPLY_ASSIGN,
  C_BINARY_DIVIDE_ASSIGN,
  C_BINARY_MODULO_ASSIGN,
  C_BINARY_AND_ASSIGN,
  C_BINARY_OR_ASSIGN,
  C_BINARY_XOR_ASSIGN,
  C_BINARY_LEFT_SHIFT_ASSIGN,
  C_BINARY_RIGHT_SHIFT_ASSIGN,
  C_BINARY_COMMA,

  C_BINARY_COUNT,
} C_Binary;

typedef enum C_Unary
{
  C_UNARY_NONE,

  C_UNARY_PRE_INCREMENT,
  C_UNARY_PRE_DECREMENT,
  C_UNARY_POST_INCREMENT,
  C_UNARY_POST_DECREMENT,
  C_UNARY_NEGATE,
  C_UNARY_PLUS,
  C_UNARY_REFERENCE,
  C_UNARY_DEREFERENCE,
  C_UNARY_BITWISE_NOT,
  C_UNARY_LOGICAL_NOT,

  C_UNARY_COUNT,
} C_Unary;

typedef struct C_Node C_Node;
struct C_Node
{
  C_Node_Type type;

  C_Node *parent;

  C_Node *first_child;
  C_Node *last_child;
  usize  child_count;

  C_Node *next_sibling;
  C_Node *prev_sibling;

  union
  {
    String    name;
    C_Literal literal;
    C_Binary  binary;
    C_Unary   unary;
  };
};

typedef struct C_Parser C_Parser;
struct C_Parser
{
  C_Token_Array tokens;
  usize at;

  i32 loop_nests;
  i32 switch_nests;

  b32 had_error;
};

typedef struct C_Token_Node_Map C_Token_Node_Map;
struct C_Token_Node_Map
{
  C_Unary  unary;
  C_Binary binary;
  i32 unary_precedence;
  i32 binary_precedence;
};

// TODO: Could maybe condense this by just setting the enum values to their precedence...
// also could make sure the C_TOKEN_... are low numbers to keep this table small
static
C_Token_Node_Map token_to_node_table[] =
{
  [C_TOKEN_DOT]                = { C_UNARY_NONE,          C_BINARY_ACCESS,             C_MIN_PRECEDENCE, 15 },
  [C_TOKEN_ARROW]              = { C_UNARY_NONE,          C_BINARY_POINTER_ACCESS,     C_MIN_PRECEDENCE, 15 },
  [C_TOKEN_BEGIN_SQUARE_BRACE] = { C_UNARY_NONE,          C_BINARY_ARRAY_ACCESS,       C_MIN_PRECEDENCE, 15 },
  [C_TOKEN_INCREMENT]          = { C_UNARY_PRE_INCREMENT, C_BINARY_NONE,               15,               C_MIN_PRECEDENCE }, // NOTE: Special-case
  [C_TOKEN_DECREMENT]          = { C_UNARY_PRE_DECREMENT, C_BINARY_NONE,               15,               C_MIN_PRECEDENCE }, // NOTE: Special-case
  [C_TOKEN_BITWISE_NOT]        = { C_UNARY_BITWISE_NOT,   C_BINARY_NONE,               14,               C_MIN_PRECEDENCE },
  [C_TOKEN_LOGICAL_NOT]        = { C_UNARY_LOGICAL_NOT,   C_BINARY_NONE,               14,               C_MIN_PRECEDENCE },
  [C_TOKEN_BITWISE_AND]        = { C_UNARY_REFERENCE,     C_BINARY_BITWISE_AND,        14,               8 },
  [C_TOKEN_STAR]               = { C_UNARY_DEREFERENCE,   C_BINARY_MULTIPLY,           14,               13 },
  [C_TOKEN_ADD]                = { C_UNARY_PLUS,          C_BINARY_ADD,                14,               12 },
  [C_TOKEN_MINUS]              = { C_UNARY_NEGATE,        C_BINARY_SUBTRACT,           14,               12 },
  [C_TOKEN_DIVIDE]             = { C_UNARY_NONE,          C_BINARY_DIVIDE,             C_MIN_PRECEDENCE, 12 },
  [C_TOKEN_MODULO]             = { C_UNARY_NONE,          C_BINARY_MODULO,             C_MIN_PRECEDENCE, 12 },
  [C_TOKEN_LEFT_SHIFT]         = { C_UNARY_NONE,          C_BINARY_LEFT_SHIFT,         C_MIN_PRECEDENCE, 11 },
  [C_TOKEN_RIGHT_SHIFT]        = { C_UNARY_NONE,          C_BINARY_RIGHT_SHIFT,        C_MIN_PRECEDENCE, 11 },
  [C_TOKEN_LESS_THAN]          = { C_UNARY_NONE,          C_BINARY_LESS_THAN,          C_MIN_PRECEDENCE, 10 },
  [C_TOKEN_LESS_THAN_EQUAL]    = { C_UNARY_NONE,          C_BINARY_LESS_THAN_EQUAL,    C_MIN_PRECEDENCE, 10 },
  [C_TOKEN_GREATER_THAN]       = { C_UNARY_NONE,          C_BINARY_GREATER_THAN,       C_MIN_PRECEDENCE, 10 },
  [C_TOKEN_GREATER_THAN_EQUAL] = { C_UNARY_NONE,          C_BINARY_GREATER_THAN_EQUAL, C_MIN_PRECEDENCE, 10 },
  [C_TOKEN_COMPARE_EQUAL]      = { C_UNARY_NONE,          C_BINARY_COMPARE_EQUAL,      C_MIN_PRECEDENCE, 9 },
  [C_TOKEN_COMPARE_NOT_EQUAL]  = { C_UNARY_NONE,          C_BINARY_COMPARE_NOT_EQUAL,  C_MIN_PRECEDENCE, 9 },
  [C_TOKEN_XOR]                = { C_UNARY_NONE,          C_BINARY_XOR,                C_MIN_PRECEDENCE, 7 },
  [C_TOKEN_BITWISE_OR]         = { C_UNARY_NONE,          C_BINARY_BITWISE_OR,         C_MIN_PRECEDENCE, 6 },
  [C_TOKEN_LOGICAL_AND]        = { C_UNARY_NONE,          C_BINARY_LOGICAL_AND,        C_MIN_PRECEDENCE, 5 },
  [C_TOKEN_LOGICAL_OR]         = { C_UNARY_NONE,          C_BINARY_LOGICAL_OR,         C_MIN_PRECEDENCE, 4 },
  [C_TOKEN_QUESTION]           = { C_UNARY_NONE,          C_BINARY_NONE,               C_MIN_PRECEDENCE, 3 }, // NOTE: Ternary here for convenience
  [C_TOKEN_ASSIGN]             = { C_UNARY_NONE,          C_BINARY_ASSIGN,             C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_ADD_ASSIGN]         = { C_UNARY_NONE,          C_BINARY_ADD_ASSIGN,         C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_SUBTRACT_ASSIGN]    = { C_UNARY_NONE,          C_BINARY_SUBTRACT_ASSIGN,    C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_MULTIPLY_ASSIGN]    = { C_UNARY_NONE,          C_BINARY_MULTIPLY_ASSIGN,    C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_DIVIDE_ASSIGN]      = { C_UNARY_NONE,          C_BINARY_DIVIDE_ASSIGN,      C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_MODULO_ASSIGN]      = { C_UNARY_NONE,          C_BINARY_MODULO_ASSIGN,      C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_AND_ASSIGN]         = { C_UNARY_NONE,          C_BINARY_AND_ASSIGN,         C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_OR_ASSIGN]          = { C_UNARY_NONE,          C_BINARY_OR_ASSIGN,          C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_XOR_ASSIGN]         = { C_UNARY_NONE,          C_BINARY_XOR_ASSIGN,         C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_LEFT_SHIFT_ASSIGN]  = { C_UNARY_NONE,          C_BINARY_LEFT_SHIFT_ASSIGN,  C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_RIGHT_SHIFT_ASSIGN] = { C_UNARY_NONE,          C_BINARY_RIGHT_SHIFT_ASSIGN, C_MIN_PRECEDENCE, 2 },
  [C_TOKEN_COMMA]              = { C_UNARY_NONE,          C_BINARY_COMMA,              C_MIN_PRECEDENCE, 1 },
};

static
C_Token_Node_Map c_token_to_node_mapping(C_Token_Type type)
{
  C_Token_Node_Map result = {0};

  if (type > 0 && type < STATIC_COUNT(token_to_node_table))
  {
    result = token_to_node_table[type];
  }

  return result;
}

// Could easily convert these to lookups
static
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

static
b32 c_token_is_unary_operator(C_Token token)
{
  b32 result = c_token_to_node_mapping(token.type).unary != C_UNARY_NONE;

  return result;
}

static
b32 c_token_is_binary_operator(C_Token token)
{
  b32 result = c_token_to_node_mapping(token.type).binary != C_BINARY_NONE;

  return result;
}

static
C_Binary c_token_to_binary(C_Token token)
{
  C_Binary result = c_token_to_node_mapping(token.type).binary;

  return result;
}

static
C_Unary c_token_to_unary(C_Token token, b32 is_post)
{
  C_Unary result = c_token_to_node_mapping(token.type).unary;

  // Special case for post increment and decrement
  if (is_post)
  {
    if (result == C_UNARY_PRE_INCREMENT)
    {
      result = C_UNARY_POST_INCREMENT;
    }
    else if (result == C_UNARY_PRE_DECREMENT)
    {
      result = C_UNARY_POST_DECREMENT;
    }
  }

  return result;
}

static
i32 c_operator_precedence(C_Token token, b32 is_unary, b32 is_post)
{
  i32 result = C_MIN_PRECEDENCE;

  C_Token_Node_Map info = c_token_to_node_mapping(token.type);
  if (is_unary)
  {
    result = info.unary_precedence;

    // NOTE: Special case
    if ((info.unary == C_UNARY_PRE_INCREMENT || info.unary == C_UNARY_PRE_DECREMENT) && is_post)
    {
      result += 1;
    }
  }
  else
  {
    result = info.binary_precedence;
  }

  return result;
}

static
b32 c_binary_is_right_associative(C_Binary binary)
{
  b32 result = false;

  if (binary == C_BINARY_ASSIGN             ||
      binary == C_BINARY_ADD_ASSIGN         ||
      binary == C_BINARY_SUBTRACT_ASSIGN    ||
      binary == C_BINARY_MULTIPLY_ASSIGN    ||
      binary == C_BINARY_DIVIDE_ASSIGN      ||
      binary == C_BINARY_MODULO_ASSIGN      ||
      binary == C_BINARY_LEFT_SHIFT_ASSIGN  ||
      binary == C_BINARY_RIGHT_SHIFT_ASSIGN ||
      binary == C_BINARY_AND_ASSIGN         ||
      binary == C_BINARY_XOR_ASSIGN         ||
      binary == C_BINARY_OR_ASSIGN)
  {
    result = true;
  }

  return result;
}

static
C_Token c_parse_peek_token(C_Parser parser, isize offset)
{
  C_Token result = {0};

  if (parser.at + offset < parser.tokens.count && ((isize)parser.at + offset) >= 0)
  {
    result = parser.tokens.v[parser.at + offset];
  }

  return result;
}

static
b32 c_parse_match(C_Parser *parser, C_Token_Type type)
{
  return c_parse_peek_token(*parser, 0).type == type;
}

static
b32 c_parse_eat(C_Parser *parser, C_Token_Type type)
{
  b32 result = false;

  if (c_parse_match(parser, type))
  {
    result = true;
    parser->at += 1;
  }

  return result;
}

static
void c_parse_error(C_Parser *parser, char *message)
{
  LOG_ERROR(message);
  parser->had_error = true;
}

static
C_Node *c_new_node(Arena *arena, C_Node_Type type)
{
  C_Node *result = arena_new(arena, C_Node);
  result->type = type;

  return result;
}

static
void c_node_add_child(C_Node *parent, C_Node *child)
{
  child->parent = parent;

  DLL_push_last(parent->first_child, parent->last_child, child, next_sibling, prev_sibling);
  parent->child_count += 1;
}

static
b32 c_parse_incomplete(C_Parser parser)
{
  return parser.at < parser.tokens.count && !parser.had_error;
}

static
C_Node *c_nil_node()
{
  static C_Node nil = {0};

  static b32 init = false;

  if (!init)
  {
    c_node_add_child(&nil, &nil);
    init = true;
  }

  return &nil;
}

static
C_Node *c_parse_identifier(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  C_Token token = c_parse_peek_token(*parser, 0);

  if (token.type == C_TOKEN_IDENTIFIER)
  {
    result = c_new_node(arena, C_NODE_IDENTIFIER);
    result->name = token.raw;

    c_parse_eat(parser, C_TOKEN_IDENTIFIER);
  }
  else
  {
    c_parse_error(parser, "Tried to parse non-identifier token as variable");
  }

  return result;
}

static
C_Node *c_parse_expression(Arena *arena, C_Parser *parser, i32 min_precedence);

static
C_Node *c_parse_function_call(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  C_Token identifier = c_parse_peek_token(*parser, 0);

  if (identifier.type == C_TOKEN_IDENTIFIER)
  {
    result = c_new_node(arena, C_NODE_FUNCTION_CALL);

    result->name = identifier.raw;

    c_parse_eat(parser, C_TOKEN_IDENTIFIER);

    if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
    {
      // Keep going until we consume a close parenthesis.
      while (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
      {
        // Pass precedence of comma so that we stop parsing at comma.
        C_Node *argument = c_parse_expression(arena, parser, token_to_node_table[C_TOKEN_COMMA].binary_precedence);
        c_node_add_child(result, argument);

        // Skip comma
        c_parse_eat(parser, C_TOKEN_COMMA);
      }
    }
    else
    {
      c_parse_error(parser, "Expected function call to have open parenthesis");
    }
  }
  else
  {
    c_parse_error(parser, "Expected function call to begin with identifier");
  }

  return result;
}

// We parse identifiers, function calls, prefix unary ops, and open parenthesis here.
static
C_Node *c_parse_expression_start(Arena *arena, C_Parser *parser)
{
  C_Token token = c_parse_peek_token(*parser, 0);

  C_Node *result = c_nil_node();

  if (token.type == C_TOKEN_LITERAL)
  {
    result = c_new_node(arena, C_NODE_LITERAL);

    result->literal = token.literal; // Copy over

    c_parse_eat(parser, C_TOKEN_LITERAL);
  }
  else if (token.type == C_TOKEN_IDENTIFIER)
  {
    C_Token peek = c_parse_peek_token(*parser, 1);

    if (peek.type != C_TOKEN_BEGIN_PARENTHESIS)
    {
      result = c_parse_identifier(arena, parser);
    }
    else
    {
      result = c_parse_function_call(arena, parser);
    }
  }
  // As a side effect, handling prefix unaries here ensures right associativity.
  else if (c_token_is_unary_operator(token))
  {
    result = c_new_node(arena, C_NODE_UNARY);

    b32 is_post = false;

    result->unary = c_token_to_unary(token, is_post);
    c_parse_eat(parser, token.type);

    b32 is_unary = true;
    i32 precedence = c_operator_precedence(token, is_unary, is_post);
    C_Node *child = c_parse_expression(arena, parser, precedence);
    c_node_add_child(result, child);
  }
  else if (token.type == C_TOKEN_BEGIN_PARENTHESIS)
  {
    c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS);

    // Reset as if this expression is all by itself by passing the min precedence
    result = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);

    if (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
    {
      c_parse_error(parser, "Expected close parenthesis");
    }
  }

  return result;
}

static
C_Node *c_parse_expression(Arena *arena, C_Parser *parser, i32 min_precedence)
{
  C_Node *left = c_parse_expression_start(arena, parser);

  C_Node *result = left; // Return just the left if we don't meet later checks

  // Idea from https://www.youtube.com/watch?v=fIPO4G42wYE&t=4260s
  while (true)
  {
    C_Token peek = c_parse_peek_token(*parser, 0);

    if (c_token_is_binary_operator(peek))
    {
      C_Binary operator  = c_token_to_binary(peek);
      i32 new_precedence = c_operator_precedence(peek, false, false);

      // Assignments are right associative, everything else is left
      b32 should_right_associate = (new_precedence > min_precedence) ||
                                   (new_precedence == min_precedence && c_binary_is_right_associative(operator));

      if (should_right_associate)
      {
        // We can recurse down and build a deeper tree
        result = c_new_node(arena, C_NODE_BINARY);
        result->binary = operator;

        c_parse_eat(parser, peek.type);

        // We act like the inner expression of array access is surrounded by parenthesis
        if (operator == C_BINARY_ARRAY_ACCESS)
        {
          new_precedence = C_MIN_PRECEDENCE;
        }

        C_Node *right = c_parse_expression(arena, parser, new_precedence);

        // For array access, skip over the subsequent close square brace
        if (operator == C_BINARY_ARRAY_ACCESS)
        {
          if(!c_parse_eat(parser, C_TOKEN_CLOSE_SQUARE_BRACE))
          {
            c_parse_error(parser, "Expected closing square brace for array access");
          }
        }

        c_node_add_child(result, left);
        c_node_add_child(result, right);
      }
      else
      {
        // We need to head back up the tree (i.e. pop some recursive calls)
        // to parse the next operator, need a lower precedence operator
        break;
      }
    }
    // Handle postfix unary operators here
    else if (peek.type == C_TOKEN_INCREMENT || peek.type == C_TOKEN_DECREMENT)
    {
      b32 is_post  = true;
      b32 is_unary = true;

      i32 new_precedence = c_operator_precedence(peek, is_unary, is_post);

      if (new_precedence > min_precedence)
      {
        result = c_new_node(arena, C_NODE_UNARY);

        result->unary = c_token_to_unary(peek, is_post);

        c_parse_eat(parser, peek.type);

        c_node_add_child(result, left);
      }
      else
      {
        // We need to head back up the tree (i.e. pop some recursive calls)
        // to parse the next operator, need a lower precedence operator
        break;
      }
    }
    else
    {
      // No more binary operators, we are done.
      break;
    }

    // Now left hand side becomes this new tree
    left = result;
  }


  // Check for ternary and if we were called with a lower or equal (since ternary is right-associative) precedence than a ternary...
  // (assignment operators, comma), if so grab it, rebuild result tree
  // since we know that actually the assignment wants to be assigned
  // to the evaluation of the ternary... that is, place the ternary
  // on the right side of the tree when our parent is =, +=, -=, etc.
  // This could potentially be put into the main loop, but since this is just such a special case I decided against it.

  C_Token post_peek = c_parse_peek_token(*parser, 0);
  if (post_peek.type == C_TOKEN_QUESTION &&
      c_operator_precedence(post_peek, false, false) >= min_precedence)
  {
    c_parse_eat(parser, C_TOKEN_QUESTION);

    // We know what we have so far is the condition for the ternary.
    C_Node *condition = result;

    result = c_new_node(arena, C_NODE_TERNARY);

    c_node_add_child(result, condition);

    // Middle expr should act as if parenthesis according to docs
    C_Node *true_expression = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
    c_node_add_child(result, true_expression);

    if (!c_parse_eat(parser, C_TOKEN_COLON))
    {
      c_parse_error(parser, "Expected colon after 2nd ternary argument.");
    }

    // This gets the precedence of the ternary so we can parse commas correctly as not part of the false expression...
    // Though, this lead to side effect of things like a ? b : c = e being parsed even though they are invalid.
    i32 ternary_precedence = c_operator_precedence(post_peek, false, false);
    C_Node *false_expression = c_parse_expression(arena, parser, ternary_precedence);
    c_node_add_child(result, false_expression);
  }

  return result;
}

// TODO: Type cache
static
C_Node *c_parse_type(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  C_Token token = c_parse_peek_token(*parser, 0);

  if (c_token_is_type_keyword(token))
  {
    result = c_new_node(arena, C_NODE_TYPE);
    result->name = token.raw; // Hehehe, nice to just do this

    c_parse_eat(parser, token.type);
  }
  else if (token.type == C_TOKEN_IDENTIFIER) // Custom type
  {
  }
  else
  {
    c_parse_error(parser, "Tried to create type node with non-type keyword or identifier token.");
  }

  return result;
}

// Can be used for parsing function parameters, the beginning of variable declarations, struct members...
static
C_Node *c_parse_declarator(Arena *arena, C_Parser *parser, C_Node_Type type)
{
  C_Node *result = c_new_node(arena, type);

  C_Node *type_node = c_parse_type(arena, parser);
  c_node_add_child(result, type_node);

  C_Node *name_node = c_parse_identifier(arena, parser);
  c_node_add_child(result, name_node);

  return result;
}

static
C_Node *c_parse_variable_declaration(Arena *arena, C_Parser *parser)
{
  // TODO: Comma separations for multiple variable decls
  C_Node *result = c_parse_declarator(arena, parser, C_NODE_VARIABLE_DECLARATION);

  // If we see an equal sign, grab initializing expression
  if (c_parse_eat(parser, C_TOKEN_ASSIGN))
  {
    C_Node *expression = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
    c_node_add_child(result, expression); // Third child is initializing expression, if present

    if (expression->type == C_NODE_NONE)
    {
      c_parse_error(parser, "Expected expression to initialize variable declaration.");
    }
  }

  if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
  {
    c_parse_error(parser, "Expected semicolon following top level variable declaration.");
  }

  return result;
}

static
C_Node *c_parse_declaration(Arena *arena, C_Parser *parser, b32 at_top_level);

static
C_Node *c_parse_block(Arena *arena, C_Parser *parser);

static
C_Node *c_parse_statement(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_nil_node();

  C_Token peek = c_parse_peek_token(*parser, 0);
  switch (peek.type)
  {
    // Basically same case.
    case C_TOKEN_KEYWORD_WHILE:
    case C_TOKEN_KEYWORD_IF:
    {
      c_parse_eat(parser, peek.type);
      result = c_new_node(arena, peek.type == C_TOKEN_KEYWORD_IF ? C_NODE_IF : C_NODE_WHILE);

      if (peek.type == C_TOKEN_KEYWORD_WHILE)
      {
        parser->loop_nests += 1;
      }

      if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
      {
        C_Node *condition = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        c_node_add_child(result, condition);

        if (c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
        {
          C_Node *statement = c_parse_statement(arena, parser);
          c_node_add_child(result, statement);

          // Check for else
          if (c_parse_eat(parser, C_TOKEN_KEYWORD_ELSE))
          {
            C_Node *els = c_new_node(arena, C_NODE_ELSE);
            c_node_add_child(result, els);

            C_Node *else_statement = c_parse_statement(arena, parser);
            c_node_add_child(els, else_statement);
          }
        }
        else
        {
          c_parse_error(parser, "Expected close parenthesis following condition expression.");
        }
      }
      else
      {
        // FIXME: var args for this function so can pass if or while string to this message
        c_parse_error(parser, "Expected begin parenthesis following if or while.");
      }

      if (peek.type == C_TOKEN_KEYWORD_WHILE)
      {
        parser->loop_nests -= 1;
      }

    } break;
    case C_TOKEN_KEYWORD_FOR:
    {
      c_parse_eat(parser, C_TOKEN_KEYWORD_FOR);
      result = c_new_node(arena, C_NODE_FOR);

      parser->loop_nests += 1;

      if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
      {
        // First part is decl or expression
        C_Node *init = c_parse_variable_declaration(arena, parser);

        // Try to parse as expression if not a declaration
        if (init == c_nil_node())
        {
          init = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);

          // Grab a semicolon, only for expression, as parse_variable_declaration will grab the semicolon in that case.
          if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
          {
            c_parse_error(parser, "Expected semicolon following first initialization loop clause.");
            // Should continue after failing this?
          }
        }
        c_node_add_child(result, init);


        C_Node *condition = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        // FIXME: Don't like this... probably should just start making actual types to put in the node union
        if (condition != c_nil_node())
        {
          c_node_add_child(result, condition);
        }

        if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
        {
          c_parse_error(parser, "Expected semicolon following second condition loop clause.");
          // Should continue after failing this?
        }

        C_Node *update = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        // FIXME: Don't like this... probably should just start making actual types to put in the node union
        if (update != c_nil_node())
        {
          c_node_add_child(result, update);
        }

        if (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
        {
          c_parse_error(parser, "Expected semicolon following third update loop clause.");
          // Should continue after failing this?
        }

        C_Node *statement = c_parse_statement(arena, parser);
        c_node_add_child(result, statement);
      }
      else
      {
        c_parse_error(parser, "Expected begin parenthesis following for.");
      }

      parser->loop_nests -= 1;

    } break;
    case C_TOKEN_KEYWORD_DO:
    {
      c_parse_eat(parser, C_TOKEN_KEYWORD_DO);

      result = c_new_node(arena, C_NODE_DO_WHILE);

      parser->loop_nests += 1;

      C_Node *statement = c_parse_statement(arena, parser);
      c_node_add_child(result, statement);

      if (c_parse_eat(parser, C_TOKEN_KEYWORD_WHILE))
      {
        C_Node *condition = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        c_node_add_child(result, condition);
      }
      else
      {
        c_parse_error(parser, "Expected while following do.");
      }

      parser->loop_nests -= 1;

    } break;
    case C_TOKEN_KEYWORD_SWITCH:
    {
      c_parse_eat(parser, C_TOKEN_KEYWORD_SWITCH);

      result = c_new_node(arena, C_NODE_SWITCH);

      if (c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
      {
        C_Node *condition = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        c_node_add_child(result, condition);

        if (c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
        {
          parser->switch_nests += 1;

          // Begin parsing case statement(s).
          // Multiple cases must be contained with a block.
          C_Node *statement = c_parse_statement(arena, parser);

          if (statement->type == C_NODE_CASE || statement->type == C_NODE_BLOCK)
          {
            c_node_add_child(result, statement);
          }
          else
          {
            c_parse_error(parser, "Invalid statement type following switch.");
          }

          parser->switch_nests -= 1;
        }
        else
        {
          c_parse_error(parser, "Expected close parenthesis following switch condition.");
        }
      }
      else
      {
        c_parse_error(parser, "Expected an open parenthesis following switch.");
      }

    } break;
    case C_TOKEN_KEYWORD_CASE:
    {
      if (parser->switch_nests > 0)
      {
        c_parse_eat(parser, C_TOKEN_KEYWORD_CASE);
        result = c_new_node(arena, C_NODE_CASE);

        C_Node *match_expression = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
        c_node_add_child(result, match_expression);

        if (c_parse_eat(parser, C_TOKEN_COLON))
        {
          C_Token case_or_close_maybe = c_parse_peek_token(*parser, 0);

          // TODO: This seems not great to be checking for close curly here.
          while (case_or_close_maybe.type != C_TOKEN_KEYWORD_CASE &&
                 case_or_close_maybe.type != C_TOKEN_CLOSE_CURLY_BRACE)
          {
            C_Node *statement = c_parse_statement(arena, parser);
            c_node_add_child(result, statement);

            case_or_close_maybe = c_parse_peek_token(*parser, 0);
          }
        }
        else
        {
          c_parse_error(parser, "Expected colon following case match expression.");
        }
      }
      else
      {
        c_parse_error(parser, "Case statement without enclosing switch.");
      }
    } break;
    case C_TOKEN_KEYWORD_RETURN:
    {
      c_parse_eat(parser, C_TOKEN_KEYWORD_RETURN);
      result = c_new_node(arena, C_NODE_RETURN);

      C_Node *expression = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);
      c_node_add_child(result, expression);

      if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
      {
        // FIXME: var args to report the right string
        c_parse_error(parser, "Expected semicolon following return.");
      }
    } break;
    case C_TOKEN_KEYWORD_GOTO:
    {
    } break;
    case C_TOKEN_KEYWORD_BREAK:
    {
      if (parser->loop_nests > 0 || parser->switch_nests > 0)
      {
        c_parse_eat(parser, C_TOKEN_KEYWORD_BREAK);

        result = c_new_node(arena, C_NODE_BREAK);

        if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
        {
          c_parse_error(parser, "Expected semicolon following break.");
        }
      }
      else
      {
        c_parse_error(parser, "Break statement not in a loop or switch.");
      }
    } break;
    case C_TOKEN_KEYWORD_CONTINUE:
    {
      if (parser->loop_nests > 0)
      {
        c_parse_eat(parser, C_TOKEN_KEYWORD_CONTINUE);

        result = c_new_node(arena, C_NODE_CONTINUE);

        if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
        {
          c_parse_error(parser, "Expected semicolon following continue.");
        }
      }
      else
      {
        c_parse_error(parser, "Break statement not in a loop or switch.");
      }
    } break;
    case C_TOKEN_BEGIN_CURLY_BRACE:
    {
      result = c_parse_block(arena, parser);
    } break;

    // Everything else
    default:
    {
      b32 at_top_level = false;
      result = c_parse_declaration(arena, parser, at_top_level);

      // We tried to parse a declaration and couldn't.
      // So, this is most likely an expression statement or its just not a statement.
      if (result == c_nil_node())
      {
        result = c_parse_expression(arena, parser, C_MIN_PRECEDENCE);

        // If we got a valid expression back, eat a semicolon.
        if (result != c_nil_node())
        {
          if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
          {
            // FIXME: var args to report the right string
            c_parse_error(parser, "Expected semicolon following declaration");
          }
        }
      }

    } break;
  }

  return result;
}

static
C_Node *c_parse_block(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_new_node(arena, C_NODE_BLOCK);

  if (c_parse_eat(parser, C_TOKEN_BEGIN_CURLY_BRACE))
  {
    while (true)
    {
      C_Node *statement = c_parse_statement(arena, parser);
      if (statement == c_nil_node())
      {
        break;
      }

      c_node_add_child(result, statement);
    }

    if (!c_parse_eat(parser, C_TOKEN_CLOSE_CURLY_BRACE))
    {
      c_parse_error(parser, "Expected closing curly brace for block statement.");
    }
  }
  else
  {
    c_parse_error(parser, "Tried to parse block without begin curly brace.");
  }

  return result;
}

static
C_Node *c_parse_function_declaration(Arena *arena, C_Parser *parser)
{
  C_Node *result = c_parse_declarator(arena, parser, C_NODE_FUNCTION_DECLARATION);

  if (!c_parse_eat(parser, C_TOKEN_BEGIN_PARENTHESIS))
  {
    c_parse_error(parser, "Expected open parenthesis for function declaration");
  }

  // 3rd child until last are parameters, don't go into this loop if we immediately see a close parenthesis
  if (!c_parse_match(parser, C_TOKEN_CLOSE_PARENTHESIS))
  {
    // Haha, do while
    do
    {
      C_Node *parameter = c_parse_declarator(arena, parser, C_NODE_VARIABLE_DECLARATION);
      c_node_add_child(result, parameter);
    }
    while (c_parse_eat(parser, C_TOKEN_COMMA));
  }

  if (!c_parse_eat(parser, C_TOKEN_CLOSE_PARENTHESIS))
  {
    c_parse_error(parser, "Expected close parenthesis for function declaration");
  }

  // Parse definition block.
  if (c_parse_match(parser, C_TOKEN_BEGIN_CURLY_BRACE))
  {
    C_Node *definition = c_parse_block(arena, parser);
    c_node_add_child(result, definition);
  }
  // Or a semicolon
  else if (!c_parse_eat(parser, C_TOKEN_SEMICOLON))
  {
    c_parse_error(parser, "Expected semicolon or function definition block.");
  }

  return result;
}

static
C_Node *c_parse_struct_declaration(Arena *arena, C_Parser *parser)
{
  return c_nil_node();
}

static
C_Node *c_parse_enum_declaration(Arena *arena, C_Parser *parser)
{
  return c_nil_node();
}

static
C_Node *c_parse_declaration(Arena *arena, C_Parser *parser, b32 at_top_level)
{
  C_Node *result = c_nil_node();

  C_Token token = c_parse_peek_token(*parser, 0);

  // TODO: Should also check for declaration if it is an identifier that is a type too
  if (c_token_is_type_keyword(token))
  {
    C_Token peek0 = c_parse_peek_token(*parser, 1);
    C_Token peek1 = c_parse_peek_token(*parser, 2);

    // Declaration, probably
    if (peek0.type == C_TOKEN_IDENTIFIER)
    {
      // Function thing
      if (peek1.type == C_TOKEN_BEGIN_PARENTHESIS && at_top_level)
      {
        result = c_parse_function_declaration(arena, parser);
      }
      // Variable thing
      else
      {
        result = c_parse_variable_declaration(arena, parser);
      }
    }
  }
  else if (token.type == C_TOKEN_KEYWORD_STRUCT)
  {
    result = c_parse_struct_declaration(arena, parser);
  }
  else if (token.type == C_TOKEN_KEYWORD_ENUM)
  {
    result = c_parse_enum_declaration(arena, parser);
  }

  return result;
}


// Returns root node
static
C_Node *parse_c_tokens(Arena *arena, C_Token_Array tokens)
{
  C_Node *root = c_new_node(arena, C_NODE_ROOT);

  C_Parser parser =
  {
    .tokens = tokens,
    .at = 0,
  };

  while (c_parse_incomplete(parser))
  {
    b32 at_top_level = true;
    C_Node *declaration = c_parse_declaration(arena, &parser, at_top_level);
    c_node_add_child(root, declaration);
  }

  return root;
}
#endif // C_PARSE
