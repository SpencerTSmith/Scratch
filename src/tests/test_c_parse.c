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
      default: { printf("%s", C_Node_Type_strings[node->type]); } break;

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
        }
      }
      break;
      case C_NODE_TERNARY:
      {
        printf("?:");
      }
      break;
      case C_NODE_IDENTIFIER:
      case C_NODE_TYPE:
      {
        printf("%.*s", STRF(node->name));
      }
      break;
      case C_NODE_FUNCTION_CALL:
      {
        printf("CALL - %.*s", STRF(node->name));
      }
      break;
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
  C_Token_Array tokens = tokenize_c_code(arena, code);
  print_c_ast(parse_c_tokens(arena, tokens), 0, 0);
}

static
C_Node *parse_expression(Arena *arena, String code)
{
  C_Token_Array tokens = tokenize_c_code(arena, code);
  C_Parser parser = { .tokens = tokens, .at = 0 };

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
    TEST_EVAL(string_match(tree->name, STR("foo")));
    TEST_EVAL(tree->child_count == 0);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Function call - with arguments"))
  {
    C_Node *tree = parse_expression(&arena, STR("foo(a, b)"));

    TEST_EVAL(tree->type == C_NODE_FUNCTION_CALL);
    TEST_EVAL(string_match(tree->name, STR("foo")));
    TEST_EVAL(tree->child_count == 2);
    TEST_EVAL(is_identifier(tree->first_child, STR("a")));
    TEST_EVAL(is_identifier(tree->first_child->next_sibling, STR("b")));

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
    C_Token_Array tokens = tokenize_c_code(&arena, code);
    C_Node *root = parse_c_tokens(&arena, tokens);

    TEST_EVAL(root->type == C_NODE_ROOT);
    TEST_EVAL(root->child_count == 1);

    C_Node *decl = root->first_child;
    TEST_EVAL(decl->type == C_NODE_VARIABLE_DECLARATION);
    TEST_EVAL(decl->child_count == 2);

    C_Node *type = decl->first_child;
    TEST_EVAL(type->type == C_NODE_TYPE);
    TEST_EVAL(string_match(type->name, STR("int")));

    C_Node *name = decl->first_child->next_sibling;
    TEST_EVAL(name->type == C_NODE_IDENTIFIER);
    TEST_EVAL(string_match(name->name, STR("x")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Variable declaration - with initializer"))
  {
    String code = STR("int x = 42;");
    C_Token_Array tokens = tokenize_c_code(&arena, code);
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
    C_Token_Array tokens = tokenize_c_code(&arena, code);
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
    C_Token_Array tokens = tokenize_c_code(&arena, code);
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
    C_Token_Array tokens = tokenize_c_code(&arena, code);
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
    C_Token_Array tokens = tokenize_c_code(&arena, code);
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

    // C_Node *params = func->first_child->next_sibling->next_sibling;
    // TEST_EVAL(params->child_count == 0);

    arena_clear(&arena);
  }

  tester_summarize();

  String code = STR(
    "typedef float foo;\n"
    "void test(float boo, int bar)\n"
    "{\n"
    "  int boo = (float)boo + foo;"
    "  int boo = {1, 2+2, 3};"
    "}\n"
  );
  print_code_tree(&arena, code);

  arena_free(&arena);
}
