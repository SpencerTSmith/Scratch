#ifndef C_PARSE
#define C_PARSE

#include "c_tokenize.h"
#include "common.h"

// FIXME:
// FIXME:
// FIXME:
// FIXME:
// - Will probably need to start having distinct types for statement links, once start actually using the AST
//
// TODO:
// - Functionality:
//   - Type parsing
//     - Struct bit fields
//     - Comma separated identifiers for same declarator i.e. int i, j;
//     - Caching these for typechecking/referencing in symbol table
//   - Symbol Table(s)

#define C_Node_Type(X)           \
  X(C_NODE_NONE)                 \
  X(C_NODE_IDENTIFIER)           \
  X(C_NODE_TYPE_PRIMITIVE)       \
  X(C_NODE_TYPE_STRUCT)          \
  X(C_NODE_TYPE_ENUM)            \
  X(C_NODE_TYPE_POINTER)         \
  X(C_NODE_TYPE_FUNCTION)        \
  X(C_NODE_TYPE_ARRAY)           \
  X(C_NODE_LITERAL)              \
  X(C_NODE_COMPOUND_LITERAL)     \
  X(C_NODE_ROOT)                 \
  X(C_NODE_VARIABLE_DECLARATION) \
  X(C_NODE_STRUCT_DECLARATION)   \
  X(C_NODE_ENUM_DECLARATION)     \
  X(C_NODE_DECLARATOR_LIST)      \
  X(C_NODE_DECLARATOR)           \
  X(C_NODE_FUNCTION_DECLARATION) \
  X(C_NODE_TYPEDEF)              \
  X(C_NODE_UNARY)                \
  X(C_NODE_BINARY)               \
  X(C_NODE_TERNARY)              \
  X(C_NODE_FUNCTION_CALL)        \
  X(C_NODE_BLOCK)                \
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
  X(C_NODE_DEFAULT)              \
  X(C_NODE_GOTO)                 \
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
  C_UNARY_CAST,
  C_UNARY_SIZEOF,

  C_UNARY_COUNT,
} C_Unary;

typedef enum C_Type_Flags
{
  C_DECLARATION_FLAG_NONE     = 0,
  C_DECLARATION_FLAG_CONST    = 1 << 0,
  C_DECLARATION_FLAG_STATIC   = 1 << 1,
  C_DECLARATION_FLAG_EXTERN   = 1 << 2,
  C_DECLARATION_FLAG_VOLATILE = 1 << 3,
  C_DECLARATION_FLAG_RESTRICT = 1 << 4,
} C_Type_Flags;

typedef struct C_Node C_Node;

// Just for acceleration, that is, don't need to traverse the child linked list.
typedef struct C_Statement_Links C_Statement_Links;
struct C_Statement_Links
{
  C_Node *init;
  C_Node *condition;
  C_Node *update;
};

struct C_Node
{
  C_Node_Type type;

  C_Node *parent;

  C_Node *first_child;
  C_Node *last_child;
  usize  child_count;

  C_Node *next_sibling;
  C_Node *prev_sibling;

  C_Statement_Links links;

  C_Type_Flags type_flags;

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
  String source;

  C_Token_Array tokens;
  usize         at;

  i32 loop_nests;
  i32 switch_nests;

  b32 had_error;
};

static
C_Node *parse_c_tokens(Arena *arena, C_Tokenize_Result tokenize_result);

#endif // C_PARSE
