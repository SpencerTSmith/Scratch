#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "testing.h"
#include "testing.c"

#include "../c_tokenize.c"
#include "../c_parse.c"

static
void print_c_ast(C_Node *node, isize prev_depth, isize depth)
{
  if (node)
  {
    if (depth != 0)
    {
      for (isize i = 0; i < prev_depth; i++)
      {
        printf(" ");
      }
      printf("L");
    }

    for (isize i = 0; i < depth - 1; i++)
    {
      printf("-");
    }

    if (depth != 0)
    {
      printf("> ");
    }

    switch (node->type)
    {
      default:
      {
        printf("%s", C_Node_Type_strings[node->type]);
        if (node->name.v)
        {
          printf(" -- %.*s", STRF(node->name));
        }
      } break;

      case C_NODE_LITERAL:
      {
        C_Literal literal = node->literal;
        switch (literal.type)
        {
          default: { LOG_ERROR("Invalid literal type"); } break;

          case C_LITERAL_CHARACTER:
          {
            printf("%c", literal.character);
          }
          break;
          case C_LITERAL_STRING:
          {
            printf("%*.s", STRF(literal.string));
          }
          break;
          case C_LITERAL_INTEGER:
          {
            // TODO: account for flags better
            if (literal.flags & C_LITERAL_FLAG_UNSIGNED)
            {
              printf("%lu", literal.integer.v);
            }
            else
            {
              printf("%ld", (i64) literal.integer.v);
            }
          }
          break;
          case C_LITERAL_FLOATING:
          {
            printf("%f", literal.floating);
          }
          break;
        }
      }
      break;
      case C_NODE_BINARY:
      {
        C_Binary binary = node->binary;
        switch (binary)
        {
          default: { LOG_ERROR("Invalid binary type"); } break;

          case C_BINARY_ADD:                { printf("+"); } break;
          case C_BINARY_SUBTRACT:           { printf("-"); } break;
          case C_BINARY_MULTIPLY:           { printf("*"); } break;
          case C_BINARY_DIVIDE:             { printf("/"); } break;
          case C_BINARY_MODULO:             { printf("%%"); } break;
          case C_BINARY_XOR:                { printf("^"); } break;
          case C_BINARY_BITWISE_AND:        { printf("&"); } break;
          case C_BINARY_BITWISE_OR:         { printf("|"); }  break;
          case C_BINARY_ACCESS:             { printf("."); } break;
          case C_BINARY_POINTER_ACCESS:     { printf("->"); } break;
          case C_BINARY_ARRAY_ACCESS:       { printf("[]"); } break;
          case C_BINARY_COMPARE_EQUAL:      { printf("=="); } break;
          case C_BINARY_COMPARE_NOT_EQUAL:  { printf("!="); } break;
          case C_BINARY_LESS_THAN:          { printf("<"); } break;
          case C_BINARY_LESS_THAN_EQUAL:    { printf("<="); } break;
          case C_BINARY_GREATER_THAN:       { printf(">"); } break;
          case C_BINARY_GREATER_THAN_EQUAL: { printf(">="); } break;
          case C_BINARY_LOGICAL_AND:        { printf("&&"); } break;
          case C_BINARY_LOGICAL_OR:         { printf("||"); } break;
          case C_BINARY_LEFT_SHIFT:         { printf("<<"); } break;
          case C_BINARY_RIGHT_SHIFT:        { printf(">>"); } break;
          case C_BINARY_ASSIGN:             { printf("="); } break;
          case C_BINARY_ADD_ASSIGN:         { printf("+="); } break;
          case C_BINARY_SUBTRACT_ASSIGN:    { printf("-="); } break;
          case C_BINARY_MULTIPLY_ASSIGN:    { printf("*="); } break;
          case C_BINARY_DIVIDE_ASSIGN:      { printf("/="); } break;
          case C_BINARY_MODULO_ASSIGN:      { printf("%%="); } break;
          case C_BINARY_AND_ASSIGN:         { printf("&="); } break;
          case C_BINARY_OR_ASSIGN:          { printf("|="); } break;
          case C_BINARY_XOR_ASSIGN:         { printf("^="); } break;
          case C_BINARY_LEFT_SHIFT_ASSIGN:  { printf("<<="); } break;
          case C_BINARY_RIGHT_SHIFT_ASSIGN: { printf(">>="); } break;
          case C_BINARY_COMMA:              { printf(","); } break;
        }
      }
      break;
      case C_NODE_UNARY:
      {
        C_Unary unary = node->unary;
        switch (unary)
        {
          default: { LOG_ERROR("Invalid unary type"); } break;

          case C_UNARY_PRE_INCREMENT:  { printf("pre++"); } break;
          case C_UNARY_PRE_DECREMENT:  { printf("pre--"); } break;
          case C_UNARY_POST_INCREMENT: { printf("post++"); } break;
          case C_UNARY_POST_DECREMENT: { printf("post--"); } break;
          case C_UNARY_NEGATE:         { printf("-"); } break;
          case C_UNARY_REFERENCE:      { printf("&"); } break;
          case C_UNARY_DEREFERENCE:    { printf("*"); } break;
          case C_UNARY_BITWISE_NOT:    { printf("~"); }  break;
          case C_UNARY_LOGICAL_NOT:    { printf("!"); } break;
          case C_UNARY_CAST:           { printf("CAST"); } break;
          case C_UNARY_SIZEOF:         { printf("sizeof"); } break;
        }
      }
      break;
      case C_NODE_TERNARY:
      {
        printf("?:");
      }
      break;
    }

    if (node->declaration_flags)
    {
      printf(" flags:");
      // TODO: Keep this updated
      if (node->declaration_flags & C_DECLARATION_FLAG_CONST)    { printf(" const"); }
      if (node->declaration_flags & C_DECLARATION_FLAG_STATIC)   { printf(" static"); }
      if (node->declaration_flags & C_DECLARATION_FLAG_EXTERN)   { printf(" extern"); }
      if (node->declaration_flags & C_DECLARATION_FLAG_VOLATILE) { printf(" volatile"); }
      if (node->declaration_flags & C_DECLARATION_FLAG_RESTRICT) { printf(" restrict"); }
    }


    printf("\n");

    for (C_Node *child = node->first_child; child && child != c_nil_node(); child = child->next_sibling)
    {
      print_c_ast(child, depth, depth + 1);
    }
  }
}

static
void print_code_tree(Arena *arena, String code)
{
  C_Tokenize_Result tokens = tokenize_c_code(arena, code);
  print_c_ast(parse_c_tokens(arena, tokens), 0, 0);
}

static
C_Node *parse_expression(Arena *arena, String code)
{
  C_Tokenize_Result tokens = tokenize_c_code(arena, code);
  C_Parser parser = { .tokens = tokens.tokens, .at = 0 };

  return c_parse_expression(arena, &parser, C_MIN_PRECEDENCE);
}

static
b32 is_binary(C_Node *node, C_Binary op)
{
  return node && node->type == C_NODE_BINARY && node->binary == op;
}

static
b32 is_unary(C_Node *node, C_Unary op)
{
  return node && node->type == C_NODE_UNARY && node->unary == op;
}

static
b32 is_identifier(C_Node *node, String name)
{
  return node && node->type == C_NODE_IDENTIFIER && string_match(node->name, name);
}

static
b32 is_literal_int(C_Node *node, u64 value)
{
  return node && node->type      == C_NODE_LITERAL &&
         node->literal.type      == C_LITERAL_INTEGER &&
         node->literal.integer.v == value;
}

int main(int argc, char **argv)
{
  Arena arena = arena_make();

  // NOTE: AI-generated most of these tests, had to fix many of them and others are basically useless, checking trivial/unimportant things, but writing tests is not fun, so I decided to outsource.

#if 1
  TEST_BLOCK(STR("Literals"))
  {
    C_Node *tree = parse_expression(&arena, STR("42"));
    TEST_EVAL(tree->type == C_NODE_LITERAL);
    TEST_EVAL(tree->literal.integer.v == 42);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Variables"))
  {
    C_Node *tree = parse_expression(&arena, STR("foo"));
    TEST_EVAL(tree->type == C_NODE_IDENTIFIER);
    TEST_EVAL(string_match(tree->name, STR("foo")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Binary precedence - multiplication before addition"))
  {
    C_Node *tree = parse_expression(&arena, STR("a + b * c"));

    TEST_EVAL(is_binary(tree, C_BINARY_ADD));
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));

    C_Node *right = tree->first_child->next_sibling;
    TEST_EVAL(is_binary(right, C_BINARY_MULTIPLY));
    TEST_EVAL(is_identifier(right->first_child, STR("b")));
    TEST_EVAL(is_identifier(right->first_child->next_sibling, STR("c")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Binary precedence - reversed"))
  {
    C_Node *tree = parse_expression(&arena, STR("a * b + c"));

    TEST_EVAL(is_binary(tree, C_BINARY_ADD));

    C_Node *left = tree->first_child;
    TEST_EVAL(is_binary(left, C_BINARY_MULTIPLY));
    TEST_EVAL(is_identifier(left->first_child, STR("a")));
    TEST_EVAL(is_identifier(left->first_child->next_sibling, STR("b")));

    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("c")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Left associativity - addition"))
  {
    C_Node *tree = parse_expression(&arena, STR("a + b + c"));

    // Should be: (a + b) + c
    TEST_EVAL(is_binary(tree, C_BINARY_ADD));
    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("c")));

    C_Node *left = tree->first_child;
    TEST_EVAL(is_binary(left, C_BINARY_ADD));
    TEST_EVAL(is_identifier(left->first_child, STR("a")));
    TEST_EVAL(is_identifier(left->first_child->next_sibling, STR("b")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Right associativity - assignment"))
  {
    C_Node *tree = parse_expression(&arena, STR("a = b = c"));

    // Should be: a = (b = c)
    TEST_EVAL(is_binary(tree, C_BINARY_ASSIGN));
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));

    C_Node *right = tree->first_child->next_sibling;
    TEST_EVAL(is_binary(right, C_BINARY_ASSIGN));
    TEST_EVAL(is_identifier(right->first_child, STR("b")));
    TEST_EVAL(is_identifier(right->first_child->next_sibling, STR("c")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Parentheses override precedence"))
  {
    C_Node *tree = parse_expression(&arena, STR("(a + b) * c"));

    TEST_EVAL(is_binary(tree, C_BINARY_MULTIPLY));

    C_Node *left = tree->first_child;
    TEST_EVAL(is_binary(left, C_BINARY_ADD));
    TEST_EVAL(is_identifier(left->first_child, STR("a")));
    TEST_EVAL(is_identifier(left->first_child->next_sibling, STR("b")));

    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("c")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Prefix unary operators"))
  {
    C_Node *tree = parse_expression(&arena, STR("-a"));
    TEST_EVAL(is_unary(tree, C_UNARY_NEGATE));
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));

    arena_clear(&arena);

    tree = parse_expression(&arena, STR("!a"));
    TEST_EVAL(is_unary(tree, C_UNARY_LOGICAL_NOT));
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));

    arena_clear(&arena);

    tree = parse_expression(&arena, STR("*ptr"));
    TEST_EVAL(is_unary(tree, C_UNARY_DEREFERENCE));
    TEST_EVAL(is_identifier(tree->first_child, STR("ptr")));

    arena_clear(&arena);

    tree = parse_expression(&arena, STR("&var"));
    TEST_EVAL(is_unary(tree, C_UNARY_REFERENCE));
    TEST_EVAL(is_identifier(tree->first_child, STR("var")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Postfix unary operators"))
  {
    C_Node *tree = parse_expression(&arena, STR("a++"));
    TEST_EVAL(is_unary(tree, C_UNARY_POST_INCREMENT));
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));

    arena_clear(&arena);

    tree = parse_expression(&arena, STR("a--"));
    TEST_EVAL(is_unary(tree, C_UNARY_POST_DECREMENT));
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Unary and binary interaction"))
  {
    C_Node *tree = parse_expression(&arena, STR("-a * b"));

    TEST_EVAL(is_binary(tree, C_BINARY_MULTIPLY));

    C_Node *left = tree->first_child;
    TEST_EVAL(is_unary(left, C_UNARY_NEGATE));
    TEST_EVAL(is_identifier(left->first_child, STR("a")));

    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("b")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Array access"))
  {
    C_Node *tree = parse_expression(&arena, STR("a[0]"));

    TEST_EVAL(is_binary(tree, C_BINARY_ARRAY_ACCESS));
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));
    TEST_EVAL(is_literal_int(tree->first_child->next_sibling, 0));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Member access"))
  {
    C_Node *tree = parse_expression(&arena, STR("obj.field"));

    TEST_EVAL(is_binary(tree, C_BINARY_ACCESS));
    TEST_EVAL(is_identifier(tree->first_child, STR("obj")));
    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("field")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Pointer member access"))
  {
    C_Node *tree = parse_expression(&arena, STR("ptr->field"));

    TEST_EVAL(is_binary(tree, C_BINARY_POINTER_ACCESS));
    TEST_EVAL(is_identifier(tree->first_child, STR("ptr")));
    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("field")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Function call - no arguments"))
  {
    C_Node *tree = parse_expression(&arena, STR("foo()"));

    TEST_EVAL(tree->type == C_NODE_FUNCTION_CALL);
    TEST_EVAL(string_match(tree->first_child->name, STR("foo")));
    TEST_EVAL(tree->child_count == 1);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Function call - with arguments"))
  {
    C_Node *tree = parse_expression(&arena, STR("foo(a, b)"));

    TEST_EVAL(tree->type == C_NODE_FUNCTION_CALL);
    TEST_EVAL(string_match(tree->first_child->name, STR("foo")));
    TEST_EVAL(tree->child_count == 3);
    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("a")));
    TEST_EVAL(is_identifier(tree->first_child->next_sibling->next_sibling, STR("b")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Ternary operator - basic"))
  {
    C_Node *tree = parse_expression(&arena, STR("a ? b : c"));

    TEST_EVAL(tree->type == C_NODE_TERNARY);
    TEST_EVAL(tree->child_count == 3);
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));
    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("b")));
    TEST_EVAL(is_identifier(tree->first_child->next_sibling->next_sibling, STR("c")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Ternary operator - nested (right associative)"))
  {
    C_Node *tree = parse_expression(&arena, STR("a ? b : c ? d : e"));

    // Should be: a ? b : (c ? d : e)
    TEST_EVAL(tree->type == C_NODE_TERNARY);
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));
    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("b")));

    C_Node *false_branch = tree->first_child->next_sibling->next_sibling;
    TEST_EVAL(false_branch->type == C_NODE_TERNARY);
    TEST_EVAL(is_identifier(false_branch->first_child, STR("c")));
    TEST_EVAL(is_identifier(false_branch->first_child->next_sibling, STR("d")));
    TEST_EVAL(is_identifier(false_branch->first_child->next_sibling->next_sibling, STR("e")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Ternary with assignment - precedence"))
  {
    C_Node *tree = parse_expression(&arena, STR("a = b ? c : d"));

    // Should be: a = (b ? c : d)
    TEST_EVAL(is_binary(tree, C_BINARY_ASSIGN));
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));

    C_Node *right = tree->first_child->next_sibling;
    TEST_EVAL(right->type == C_NODE_TERNARY);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Complex expression"))
  {
    C_Node *tree = parse_expression(&arena, STR("a + b * c - d"));

    // Should be: (a + (b * c)) - d
    TEST_EVAL(is_binary(tree, C_BINARY_SUBTRACT));

    C_Node *left = tree->first_child;
    TEST_EVAL(is_binary(left, C_BINARY_ADD));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Comparison operators"))
  {
    C_Node *tree = parse_expression(&arena, STR("a < b"));
    TEST_EVAL(is_binary(tree, C_BINARY_LESS_THAN));

    arena_clear(&arena);

    tree = parse_expression(&arena, STR("a <= b"));
    TEST_EVAL(is_binary(tree, C_BINARY_LESS_THAN_EQUAL));

    arena_clear(&arena);

    tree = parse_expression(&arena, STR("a == b"));
    TEST_EVAL(is_binary(tree, C_BINARY_COMPARE_EQUAL));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Logical operators"))
  {
    C_Node *tree = parse_expression(&arena, STR("a && b"));
    TEST_EVAL(is_binary(tree, C_BINARY_LOGICAL_AND));

    arena_clear(&arena);

    tree = parse_expression(&arena, STR("a || b"));
    TEST_EVAL(is_binary(tree, C_BINARY_LOGICAL_OR));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Logical operator precedence"))
  {
    C_Node *tree = parse_expression(&arena, STR("a || b && c"));

    // Should be: a || (b && c)
    TEST_EVAL(is_binary(tree, C_BINARY_LOGICAL_OR));
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));

    C_Node *right = tree->first_child->next_sibling;
    TEST_EVAL(is_binary(right, C_BINARY_LOGICAL_AND));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Variable declaration"))
  {
    String code = STR("int x;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);

    TEST_EVAL(root->type == C_NODE_ROOT);
    TEST_EVAL(root->child_count == 1);

    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_VARIABLE_DECLARATION);
    TEST_EVAL(decl->child_count == 1);

    C_Node *type = decl->first_child;
    TEST_EVAL(type->type == C_NODE_DECLARATOR);
    TEST_EVAL(string_match(type->name, STR("int")));

    C_Node *name = decl->first_child->next_sibling;
    TEST_EVAL(name->type == C_NODE_IDENTIFIER);
    TEST_EVAL(string_match(name->name, STR("x")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Variable declaration - with initializer"))
  {
    String code = STR("int x = 42;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);

    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_VARIABLE_DECLARATION);
    TEST_EVAL(decl->child_count == 3);

    C_Node *type = decl->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE);
    TEST_EVAL(string_match(type->name, STR("int")));

    C_Node *name = decl->first_child->next_sibling;
    TEST_EVAL(name->type == C_NODE_IDENTIFIER);
    TEST_EVAL(string_match(name->name, STR("x")));

    C_Node *init = decl->first_child->next_sibling->next_sibling;
    TEST_EVAL(init->type == C_NODE_LITERAL);
    TEST_EVAL(init->literal.integer.v == 42);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Variable declaration - expression initializer"))
  {
    String code = STR("int result = a + b * 2;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);

    C_Node *decl = root->first_child;
    TEST_EVAL(decl->child_count == 3);

    C_Node *init = decl->first_child->next_sibling->next_sibling;
    TEST_EVAL(is_binary(init, C_BINARY_ADD));

    // Check that b * 2 is properly nested
    C_Node *right = init->first_child->next_sibling;
    TEST_EVAL(is_binary(right, C_BINARY_MULTIPLY));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Multiple variable declarations"))
  {
    String code = STR("int x; float y; char z;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);

    TEST_EVAL(root->child_count == 3);

    C_Node *decl1 = root->first_child;
    TEST_EVAL(decl1->type == C_NODE_VARIABLE_DECLARATION);
    TEST_EVAL(string_match(decl1->first_child->name, STR("int")));
    TEST_EVAL(string_match(decl1->first_child->next_sibling->name, STR("x")));

    C_Node *decl2 = decl1->next_sibling;
    TEST_EVAL(decl2->type == C_NODE_VARIABLE_DECLARATION);
    TEST_EVAL(string_match(decl2->first_child->name, STR("float")));
    TEST_EVAL(string_match(decl2->first_child->next_sibling->name, STR("y")));

    C_Node *decl3 = decl2->next_sibling;
    TEST_EVAL(decl3->type == C_NODE_VARIABLE_DECLARATION);
    TEST_EVAL(string_match(decl3->first_child->name, STR("char")));
    TEST_EVAL(string_match(decl3->first_child->next_sibling->name, STR("z")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Different type keywords"))
  {
    String code = STR("void v; char c; short s; long l; double d;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);

    TEST_EVAL(root->child_count == 5);

    C_Node *curr = root->first_child;
    TEST_EVAL(string_match(curr->first_child->name, STR("void")));

    curr = curr->next_sibling;
    TEST_EVAL(string_match(curr->first_child->name, STR("char")));

    curr = curr->next_sibling;
    TEST_EVAL(string_match(curr->first_child->name, STR("short")));

    curr = curr->next_sibling;
    TEST_EVAL(string_match(curr->first_child->name, STR("long")));

    curr = curr->next_sibling;
    TEST_EVAL(string_match(curr->first_child->name, STR("double")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Function declaration - no parameters"))
  {
    String code = STR("int foo();");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);

    TEST_EVAL(root->child_count == 1);

    C_Node *func = root->first_child;
    TEST_EVAL(func->type == C_NODE_FUNCTION_DECLARATION);
    TEST_EVAL(func->child_count == 2);

    C_Node *return_type = func->first_child;
    TEST_EVAL(return_type->type == C_NODE_TYPE);
    TEST_EVAL(string_match(return_type->name, STR("int")));

    C_Node *name = func->first_child->next_sibling;
    TEST_EVAL(name->type == C_NODE_IDENTIFIER);
    TEST_EVAL(string_match(name->name, STR("foo")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Pointer declaration"))
  {
    String code = STR("int *p;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_VARIABLE_DECLARATION);
    C_Node *type = decl->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE_POINTER);
    TEST_EVAL(type->first_child->type == C_NODE_TYPE);
    TEST_EVAL(string_match(type->first_child->name, STR("int")));
    C_Node *name = decl->first_child->next_sibling;
    TEST_EVAL(string_match(name->name, STR("p")));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Double pointer declaration"))
  {
    String code = STR("int **pp;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    C_Node *outer_ptr = decl->first_child;
    TEST_EVAL(outer_ptr->type == C_NODE_TYPE_POINTER);
    C_Node *inner_ptr = outer_ptr->first_child;
    TEST_EVAL(inner_ptr->type == C_NODE_TYPE_POINTER);
    TEST_EVAL(inner_ptr->first_child->type == C_NODE_TYPE);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Const pointer to int"))
  {
    String code = STR("int * const p;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    C_Node *ptr = decl->first_child;
    TEST_EVAL(ptr->type == C_NODE_TYPE_POINTER);
    TEST_EVAL(ptr->declaration_flags & C_DECLARATION_FLAG_CONST);
    TEST_EVAL(!(ptr->first_child->declaration_flags & C_DECLARATION_FLAG_CONST));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Pointer to const int"))
  {
    String code = STR("const int *p;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    C_Node *ptr = decl->first_child;
    TEST_EVAL(ptr->type == C_NODE_TYPE_POINTER);
    TEST_EVAL(!(ptr->declaration_flags & C_DECLARATION_FLAG_CONST));
    TEST_EVAL(ptr->first_child->declaration_flags & C_DECLARATION_FLAG_CONST);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Const pointer to const int"))
  {
    String code = STR("const int * const p;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    C_Node *ptr = decl->first_child;
    TEST_EVAL(ptr->type == C_NODE_TYPE_POINTER);
    TEST_EVAL(ptr->declaration_flags & C_DECLARATION_FLAG_CONST);
    TEST_EVAL(ptr->first_child->declaration_flags & C_DECLARATION_FLAG_CONST);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Array declaration"))
  {
    String code = STR("int arr[10];");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_VARIABLE_DECLARATION);
    C_Node *type = decl->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE_ARRAY);
    C_Node *base = type->first_child;
    TEST_EVAL(base->type == C_NODE_TYPE);
    TEST_EVAL(string_match(base->name, STR("int")));
    C_Node *count = type->first_child->next_sibling;
    TEST_EVAL(is_literal_int(count, 10));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Array of pointers"))
  {
    String code = STR("int *arr[10];");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    // Top of type tree should be array
    C_Node *type = decl->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE_ARRAY);
    // Underneath array should be pointer
    TEST_EVAL(type->first_child->type == C_NODE_TYPE_POINTER);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("2D array"))
  {
    String code = STR("int arr[10][20];");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    C_Node *outer = decl->first_child;
    TEST_EVAL(outer->type == C_NODE_TYPE_ARRAY);
    C_Node *outer_count = outer->first_child->next_sibling;
    TEST_EVAL(is_literal_int(outer_count, 20));
    C_Node *inner = outer->first_child;
    TEST_EVAL(inner->type == C_NODE_TYPE_ARRAY);
    C_Node *inner_count = inner->first_child->next_sibling;
    TEST_EVAL(is_literal_int(inner_count, 10));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Struct declaration - forward declaration"))
  {
    String code = STR("struct Foo;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_STRUCT_DECLARATION);
    TEST_EVAL(string_match(decl->first_child->name, STR("Foo")));
    TEST_EVAL(decl->child_count == 1);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Struct declaration - with body"))
  {
    String code = STR("struct Foo { int x; float y; };");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_STRUCT_DECLARATION);
    TEST_EVAL(string_match(decl->first_child->name, STR("Foo")));
    TEST_EVAL(decl->child_count == 3);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Struct declaration - anonymous"))
  {
    String code = STR("struct { int x; };");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_STRUCT_DECLARATION);
    TEST_EVAL(decl->name.count == 0);
    arena_clear(&arena);
  }

  // ENUM DECLARATIONS
  TEST_BLOCK(STR("Enum declaration"))
  {
    String code = STR("enum Color { RED, GREEN, BLUE, };");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_ENUM_DECLARATION);
    TEST_EVAL(string_match(decl->first_child->name, STR("Color")));
    TEST_EVAL(decl->child_count == 4);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Enum declaration - with values"))
  {
    String code = STR("enum Foo { A = 1, B = 2, };");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_ENUM_DECLARATION);
    C_Node *a = decl->first_child->next_sibling;
    TEST_EVAL(string_match(a->name, STR("A")));
    TEST_EVAL(is_literal_int(a->first_child, 1));
    C_Node *b = a->next_sibling;
    TEST_EVAL(string_match(b->name, STR("B")));
    TEST_EVAL(is_literal_int(b->first_child, 2));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Function declaration - with parameters"))
  {
    String code = STR("int add(int a, int b);");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *func = root->first_child;
    TEST_EVAL(func->type == C_NODE_FUNCTION_DECLARATION);
    // return type + name + 2 params = 4
    TEST_EVAL(func->child_count == 4);
    C_Node *param1 = func->first_child->next_sibling->next_sibling;
    TEST_EVAL(param1->type == C_NODE_VARIABLE_DECLARATION);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Function declaration - pointer return type"))
  {
    String code = STR("int *alloc(int size);");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *func = root->first_child;
    TEST_EVAL(func->type == C_NODE_FUNCTION_DECLARATION);
    C_Node *return_type = func->first_child;
    TEST_EVAL(return_type->type == C_NODE_TYPE_POINTER);
    TEST_EVAL(return_type->first_child->type == C_NODE_TYPE);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Function definition"))
  {
    String code = STR("int add(int a, int b) { return a + b; }");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *func = root->first_child;
    TEST_EVAL(func->type == C_NODE_FUNCTION_DECLARATION);
    C_Node *body = func->last_child;
    TEST_EVAL(body->type == C_NODE_BLOCK);
    C_Node *ret = body->first_child;
    TEST_EVAL(ret->type == C_NODE_RETURN);
    TEST_EVAL(is_binary(ret->first_child, C_BINARY_ADD));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("If statement"))
  {
    String code = STR("void f() { if (x) { y = 1; } }");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *iff = body->first_child;
    TEST_EVAL(iff->type == C_NODE_IF);
    TEST_EVAL(is_identifier(iff->links.condition, STR("x")));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("If-else statement"))
  {
    String code = STR("void f() { if (x) { a = 1; } else { b = 2; } }");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *iff = body->first_child;
    TEST_EVAL(iff->type == C_NODE_IF);
    TEST_EVAL(iff->child_count == 3); // condition block + else
    C_Node *els = iff->last_child;
    TEST_EVAL(els->type == C_NODE_ELSE);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("While loop"))
  {
    String code = STR("void f() { while (i < 10) { i++; } }");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *loop = body->first_child;
    TEST_EVAL(loop->type == C_NODE_WHILE);
    TEST_EVAL(is_binary(loop->links.condition, C_BINARY_LESS_THAN));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("For loop"))
  {
    String code = STR("void f() { for (int i = 0; i < 10; i++) { } }");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *loop = body->first_child;
    TEST_EVAL(loop->type == C_NODE_FOR);
    TEST_EVAL(loop->links.init->type == C_NODE_VARIABLE_DECLARATION);
    TEST_EVAL(is_binary(loop->links.condition, C_BINARY_LESS_THAN));
    TEST_EVAL(is_unary(loop->links.update, C_UNARY_POST_INCREMENT));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Do-while loop"))
  {
    String code = STR("void f() { do { x++; } while (x < 10); }");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *loop = body->first_child;
    TEST_EVAL(loop->type == C_NODE_DO_WHILE);
    TEST_EVAL(is_binary(loop->links.condition, C_BINARY_LESS_THAN));
    arena_clear(&arena);
  }

  // // TEST_BLOCK(STR("Break and continue"))
  // // {
  // //   String code = STR("void f() { while (1) { if (x) break; else continue; } }");
  // //   C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
  // //   C_Node *root = parse_c_tokens(&arena, tokens);
  // //   TEST_EVAL(!root->parent->had_error); // no parse errors
  // //   arena_clear(&arena);
  // // }
  //
  TEST_BLOCK(STR("Switch statement"))
  {
    String code = STR("void f() { switch (x) { case 1: break; case 2: break; } }");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *sw = body->first_child;
    TEST_EVAL(sw->type == C_NODE_SWITCH);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Switch - basic with cases"))
  {
    String code = STR(
      "void f() {"
      "  switch (x) {"
      "    case 1: y = 1; break;"
      "    case 2: y = 2; break;"
      "  }"
      "}"
    );
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *sw = body->first_child;
    TEST_EVAL(sw->type == C_NODE_SWITCH);
    // First child is condition
    TEST_EVAL(is_identifier(sw->first_child, STR("x")));
    // Second child is the block containing cases
    C_Node *block = sw->first_child->next_sibling;
    TEST_EVAL(block->type == C_NODE_BLOCK);
    C_Node *case1 = block->first_child;
    TEST_EVAL(case1->type == C_NODE_CASE);
    TEST_EVAL(is_literal_int(case1->first_child, 1));
    C_Node *case2 = case1->next_sibling;
    TEST_EVAL(case2->type == C_NODE_CASE);
    TEST_EVAL(is_literal_int(case2->first_child, 2));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Switch - with default"))
  {
    String code = STR(
      "void f() {"
      "  switch (x) {"
      "    case 1: y = 1; break;"
      "    default: y = 0; break;"
      "  }"
      "}"
    );
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *sw = body->first_child;
    TEST_EVAL(sw->type == C_NODE_SWITCH);
    C_Node *block = sw->first_child->next_sibling;
    C_Node *case1 = block->first_child;
    TEST_EVAL(case1->type == C_NODE_CASE);
    C_Node *def = case1->next_sibling;
    TEST_EVAL(def->type == C_NODE_DEFAULT);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Switch - fallthrough"))
  {
    String code = STR(
      "void f() {"
      "  switch (x) {"
      "    case 1:"
      "    case 2: y = 2; break;"
      "    case 3: y = 3; break;"
      "  }"
      "}"
    );
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *sw = body->first_child;
    TEST_EVAL(sw->type == C_NODE_SWITCH);
    C_Node *block = sw->first_child->next_sibling;
    C_Node *case1 = block->first_child;
    TEST_EVAL(case1->type == C_NODE_CASE);
    // Case 1 has no statements (fallthrough), only the match expression
    TEST_EVAL(case1->child_count == 1);
    C_Node *case2 = case1->next_sibling;
    TEST_EVAL(case2->type == C_NODE_CASE);
    TEST_EVAL(is_literal_int(case2->first_child, 2));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Switch - integer expression condition"))
  {
    String code = STR(
      "void f() {"
      "  switch (a + b) {"
      "    case 1: break;"
      "  }"
      "}"
    );
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *sw = body->first_child;
    TEST_EVAL(sw->type == C_NODE_SWITCH);
    TEST_EVAL(is_binary(sw->first_child, C_BINARY_ADD));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Goto and label"))
  {
    String code = STR("void f() { goto end; end: return; }");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *body = root->first_child->last_child;
    C_Node *gt = body->first_child;
    TEST_EVAL(gt->type == C_NODE_GOTO);
    TEST_EVAL(string_match(gt->first_child->name, STR("end")));
    C_Node *label = gt->next_sibling;
    TEST_EVAL(label->type == C_NODE_LABEL);
    arena_clear(&arena);
  }

  // COMPOUND LITERALS / INITIALIZERS
  TEST_BLOCK(STR("Compound literal - positional"))
  {
    C_Node *tree = parse_expression(&arena, STR("{1, 2, 3}"));
    TEST_EVAL(tree->type == C_NODE_COMPOUND_LITERAL);
    TEST_EVAL(tree->child_count == 3);
    TEST_EVAL(is_literal_int(tree->first_child, 1));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Compound literal - designated"))
  {
    C_Node *tree = parse_expression(&arena, STR("{.x = 1, .y = 2}"));
    TEST_EVAL(tree->type == C_NODE_COMPOUND_LITERAL);
    TEST_EVAL(tree->child_count == 2);
    C_Node *x = tree->first_child;
    TEST_EVAL(string_match(x->name, STR("x")));
    TEST_EVAL(is_literal_int(x->first_child, 1));
    arena_clear(&arena);
  }

  // STATIC
  TEST_BLOCK(STR("Static variable"))
  {
    String code = STR("static int x;");
    C_Tokenize_Result tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);
    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_VARIABLE_DECLARATION);
    C_Node *type = decl->first_child;
    TEST_EVAL(type->declaration_flags & C_DECLARATION_FLAG_STATIC);
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Cast - basic"))
  {
    C_Node *tree = parse_expression(&arena, STR("(int)x"));
    TEST_EVAL(is_unary(tree, C_UNARY_CAST));
    C_Node *type = tree->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE);
    TEST_EVAL(string_match(type->name, STR("int")));
    C_Node *operand = type->first_child;
    TEST_EVAL(is_identifier(operand, STR("x")));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Cast - to pointer type"))
  {
    C_Node *tree = parse_expression(&arena, STR("(int *)x"));
    TEST_EVAL(is_unary(tree, C_UNARY_CAST));
    C_Node *type = tree->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE_POINTER);
    TEST_EVAL(type->first_child->type == C_NODE_TYPE);
    TEST_EVAL(string_match(type->first_child->name, STR("int")));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Cast - on expression"))
  {
    C_Node *tree = parse_expression(&arena, STR("(float)(a + b)"));
    TEST_EVAL(is_unary(tree, C_UNARY_CAST));
    C_Node *type = tree->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE);
    C_Node *operand = type->first_child;
    TEST_EVAL(is_binary(operand, C_BINARY_ADD));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Cast - precedence with multiply"))
  {
    C_Node *tree = parse_expression(&arena, STR("(int)a * b"));
    // Should be: ((int)a) * b
    TEST_EVAL(is_binary(tree, C_BINARY_MULTIPLY));
    C_Node *left = tree->first_child;
    TEST_EVAL(is_unary(left, C_UNARY_CAST));
    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("b")));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Cast - chained"))
  {
    C_Node *tree = parse_expression(&arena, STR("(int)(float)x"));
    TEST_EVAL(is_unary(tree, C_UNARY_CAST));
    C_Node *inner = tree->first_child->first_child;
    TEST_EVAL(is_unary(inner, C_UNARY_CAST));
    C_Node *type = inner->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE);
    TEST_EVAL(string_match(type->name, STR("float")));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Cast - void pointer"))
  {
    C_Node *tree = parse_expression(&arena, STR("(void *)x"));
    TEST_EVAL(is_unary(tree, C_UNARY_CAST));
    C_Node *type = tree->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE_POINTER);
    TEST_EVAL(string_match(type->first_child->name, STR("void")));
    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Cast - const pointer"))
  {
    C_Node *tree = parse_expression(&arena, STR("(const int *)x"));
    TEST_EVAL(is_unary(tree, C_UNARY_CAST));
    C_Node *ptr = tree->first_child;
    TEST_EVAL(ptr->type == C_NODE_TYPE_POINTER);
    TEST_EVAL(ptr->first_child->declaration_flags & C_DECLARATION_FLAG_CONST);
    arena_clear(&arena);
  }

#endif

  tester_summarize();

  String code = STR(
    // "typedef float foo;\n"
    // "const int * const *a[10][12] = 1;\n"
    // "const int * const *a[10];\n"

    "int *a[10];\n"
    "int (*a)[10];\n"
    //
    // "struct foo;\n"
    // "struct boo\n"
    // "{\n"
    // "  int a;"
    // "  int b;"
    // "} goo;\n"
    // //
    // "enum boo\n"
    // "{\n"
    // "  A = 1,\n"
    // "  B,\n"
    // "}\n"
    //
    // "void test(float boo, int bar)\n"
    // "{\n"
    // "  struct boo bo = {.a = 1, .b = 2+2, 3};\n"
    // "  if (true)\n"
    // "  {\n"
    // "    boo = 0;\n"
    // "  }\n"
    // "}\n"
  );
  print_code_tree(&arena, code);

  arena_free(&arena);
}
