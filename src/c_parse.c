#ifndef C_PARSE
#define C_PARSE

#include "common.h"

#include "c_tokenize.h"

#define C_Node_Type(X)       \
  X(C_NODE_NONE)             \
  X(C_NODE_VARIABLE)         \
  X(C_NODE_TYPE)             \
  X(C_NODE_CONSTANT)         \
  X(C_NODE_ROOT)             \
  X(C_NODE_COUNT)

ENUM_TABLE(C_Node_Type);


typedef struct C_Node C_Node;
struct C_Node
{
  C_Node_Type type;

  C_Node *parent;

  C_Node *first_child;
  usize  child_count;

  C_Node *next_sibling;
  C_Node *prev_sibling;

};

typedef struct C_Parser C_Parser;
struct C_Parser
{
  C_Token_Array tokens;
  usize at;
};

b32 c_token_is_type_keyword(C_Token *token)
{
  C_Token_Type t = token->type;

  b32 result = token->type == C_TOKEN_KEYWORD_VOID  ||
               token->type == C_TOKEN_KEYWORD_CHAR  ||
               token->type == C_TOKEN_KEYWORD_SHORT ||
               token->type == C_TOKEN_KEYWORD_INT   ||
               token->type == C_TOKEN_KEYWORD_LONG  ||
               token->type == C_TOKEN_KEYWORD_FLOAT ||
               token->type == C_TOKEN_KEYWORD_DOUBLE;

  return result;
}

b32 c_token_is_literal(C_Token *token)
{
  C_Token_Type t = token->type;

  b32 result = token->type == C_TOKEN_LITERAL_CHAR       ||
               token->type == C_TOKEN_LITERAL_INT        ||
               token->type == C_TOKEN_LITERAL_LONG       ||
               token->type == C_TOKEN_LITERAL_LONG_LONG  ||
               token->type == C_TOKEN_LITERAL_UNSIGNED_LONG_LONG  ||
               token->type == C_TOKEN_LITERAL_FLOAT ||
               token->type == C_TOKEN_LITERAL_DOUBLE ||
               token->type == C_TOKEN_LITERAL_STRING;

  return result;
}

C_Node *c_parse_expression(C_Parser parser)
{
}

// Returns root node
static
C_Node *parse_c_tokens(Arena *arena, C_Token_Array tokens)
{
  C_Node *root = arena_new(arena, C_Node);
  root->type = C_NODE_ROOT;

  C_Node *curr_parent = root;
  for (usize token_idx = 0; token_idx < tokens.count;)
  {
    // Saw this somewhere... think it makes good sense for lookaheads
    C_Token token_0 = tokens.v[token_idx];
    C_Token token_1 = token_idx + 1 < tokens.count ? tokens.v[token_idx + 1] : (C_Token) {0};
    C_Token token_2 = token_idx + 2 < tokens.count ? tokens.v[token_idx + 2] : (C_Token) {0};

    if (c_token_is_type_keyword(&token_0))
    {
      C_Node type_node = {0};
      C_Node name_node = {0};
      C_Node init_node = c_parse_expression();
    }
  }

  return root;
}

#endif // C_PARSE
