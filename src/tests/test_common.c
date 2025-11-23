#define COMMON_IMPLEMENTATION
#include "../common.h"

int main(int argc, char **argv)
{
  {
    const char *label = "CLAMP";

    {
      i32 min = 1;
      i32 max = 11;
      i32 a = 10;

      PRINT_EVAL(label, CLAMP(a, min, max) == a);
    }

    {
      i32 min = 1;
      i32 max = 9;
      i32 a = 10;

      PRINT_EVAL(label, CLAMP(a, min, max) == max);
    }

    {
      i32 min = 11;
      i32 max = 15;
      i32 a = 10;

      PRINT_EVAL(label, CLAMP(a, min, max) == min);
    }
  }

  {
    const char *label = "MAX";
    i32 a = 4;
    i32 b = 5;

    PRINT_EVAL(label, MAX(a, b) == b);
    PRINT_EVAL(label, MAX(a, 10) != a);

    f32 fa = 4;
    f32 fb = 5;

    PRINT_EVAL(label, MAX(fa, fb) == fb);
    PRINT_EVAL(label, MAX(fa, 10.0) != fa);
  }

  {
    const char *label = "MIN";
    i32 a = 4;
    i32 b = 5;

    PRINT_EVAL(label, MIN(a, b) != b);
    PRINT_EVAL(label, MIN(a, 10) == a);

    f32 fa = 4;
    f32 fb = 5;

    PRINT_EVAL(label, MIN(fa, fb) != fb);
    PRINT_EVAL(label, MIN(fa, 10.0) == fa);
  }

  {
    const char *label = "SWAP";
    i32 a = 4;
    i32 b = 5;

    SWAP(a, b, i32);

    PRINT_EVAL(label, a == 5);
    PRINT_EVAL(label, b == 4);
  }

  {
    const char *label = "STATIC_COUNT";
    i32 array[16] = {0};
    PRINT_EVAL(label, STATIC_COUNT(array) == 16);
  }

  {
    const char *label = "arena_alloc / arena_make";
    Arena arena = arena_make();
    u8 *mem1 = arena_alloc(&arena, 16, 8);
    u8 *mem2 = arena_alloc(&arena, 32, 16);

    PRINT_EVAL(label, mem1 != NULL);
    PRINT_EVAL(label, mem2 != NULL);
    PRINT_EVAL(label, mem2 > mem1);

    arena_clear(&arena);
    u8 *mem3 = arena_alloc(&arena, 8, 4);
    PRINT_EVAL(label, mem3 == arena.base);

    arena_free(&arena);
  }

  {
    const char *label = "arena_calloc";
    Arena arena = arena_make();
    u32 *arr = arena_calloc(&arena, 4, u32);
    PRINT_EVAL(label, arr != NULL);
    PRINT_EVAL(label, arr[0] == 0 && arr[1] == 0 && arr[2] == 0 && arr[3] == 0);

    arena_free(&arena);
  }

  {
    const char *label = "arena_array";
    Arena arena = arena_make();
    i32_Array array = arena_array(&arena, 4, i32);

    array.v[0] = 10;
    array.v[1] = 20;
    array.v[2] = 30;
    array.v[3] = 40;

    PRINT_EVAL(label, array.count == 4);
    PRINT_EVAL(label, array.v[0] == 10 && array.v[1] == 20 && array.v[2] == 30 && array.v[3] == 40);

    arena_free(&arena);
  }

  {
    const char *label = "array_add";
    Arena arena = arena_make();
    i32_Array array = {0};

    i32 val1 = 42;
    i32 val2 = 17;
    int *added1 = array_add(&arena, array, val1);
    int *added2 = array_add(&arena, array, val2);

    PRINT_EVAL(label, added1 != NULL && added2 != NULL);
    PRINT_EVAL(label, array.count == 2);
    PRINT_EVAL(label, array.v[0] == 42 && array.v[1] == 17);
    PRINT_EVAL(label, *added1 == 42 && *added2 == 17);

    arena_free(&arena);
  }

  {
    const char *label = "SLL_push_first";

    i32_List list = {0};

    i32_Node *ptr = NULL;

    i32_Node val1 = {NULL, 42};
    i32_Node val2 = {NULL, 17};
    i32_Node val3 = {NULL, 10};

    ptr = SLL_push_first(list.first, list.last, &val1, link_next);
    PRINT_EVAL(label, list.first->value == 42);
    PRINT_EVAL(label, list.last->value == 42);
    PRINT_EVAL(label, ptr->value == 42);

    ptr = SLL_push_first(list.first, list.last, &val2, link_next);
    PRINT_EVAL(label, list.first->value == 17);
    PRINT_EVAL(label, list.last->value == 42);
    PRINT_EVAL(label, ptr->value == 17);

    ptr = SLL_push_first(list.first, list.last, &val3, link_next);
    PRINT_EVAL(label, list.first->value == 10);
    PRINT_EVAL(label, list.last->value == 42);
    PRINT_EVAL(label, ptr->value == 10);
  }

  {
    const char *label = "SLL_push_last";

    i32_List list = {0};

    i32_Node *ptr = NULL;

    i32_Node val1 = {NULL, 42};
    i32_Node val2 = {NULL, 17};
    i32_Node val3 = {NULL, 10};

    ptr = SLL_push_last(list.first, list.last, &val1, link_next);
    PRINT_EVAL(label, list.first->value == 42);
    PRINT_EVAL(label, list.last->value == 42);
    PRINT_EVAL(label, ptr->value == 42);

    ptr = SLL_push_last(list.first, list.last, &val2, link_next);
    PRINT_EVAL(label, list.first->value == 42);
    PRINT_EVAL(label, list.last->value == 17);
    PRINT_EVAL(label, ptr->value == 17);

    ptr = SLL_push_last(list.first, list.last, &val3, link_next);
    PRINT_EVAL(label, list.first->value == 42);
    PRINT_EVAL(label, list.last->value == 10);
    PRINT_EVAL(label, ptr->value == 10);
  }

  {
    const char *label = "list_push_first";

    i32_List list = {0};

    i32_Node *ptr = NULL;

    i32_Node val1 = {NULL, 42};
    i32_Node val2 = {NULL, 17};
    i32_Node val3 = {NULL, 10};

    ptr = list_push_first(list, &val1);
    PRINT_EVAL(label, list.first->value == 42);
    PRINT_EVAL(label, list.last->value == 42);
    PRINT_EVAL(label, ptr->value == 42);
    PRINT_EVAL(label, list.count == 1);

    ptr = list_push_first(list, &val2);
    PRINT_EVAL(label, list.first->value == 17);
    PRINT_EVAL(label, list.last->value == 42);
    PRINT_EVAL(label, ptr->value == 17);
    PRINT_EVAL(label, list.count == 2);

    ptr = list_push_first(list, &val3);
    PRINT_EVAL(label, list.first->value == 10);
    PRINT_EVAL(label, list.last->value == 42);
    PRINT_EVAL(label, ptr->value == 10);
    PRINT_EVAL(label, list.count == 3);
  }

  {
    const char *label = "list_push_last";

    i32_List list = {0};

    i32_Node *ptr = NULL;

    i32_Node val1 = {NULL, 42};
    i32_Node val2 = {NULL, 17};
    i32_Node val3 = {NULL, 10};

    ptr = list_push_last(list, &val1);
    PRINT_EVAL(label, list.first->value == 42);
    PRINT_EVAL(label, list.last->value == 42);
    PRINT_EVAL(label, ptr->value == 42);
    PRINT_EVAL(label, list.count == 1);

    ptr = list_push_last(list, &val2);
    PRINT_EVAL(label, list.first->value == 42);
    PRINT_EVAL(label, list.last->value == 17);
    PRINT_EVAL(label, ptr->value == 17);
    PRINT_EVAL(label, list.count == 2);

    ptr = list_push_last(list, &val3);
    PRINT_EVAL(label, list.first->value == 42);
    PRINT_EVAL(label, list.last->value == 10);
    PRINT_EVAL(label, ptr->value == 10);
    PRINT_EVAL(label, list.count == 3);
  }

  {
    const char *label = "arena_pop / arena_pop_to";
    Arena arena = arena_make();
    u32 *mem = arena_calloc(&arena, 3, u32);
    mem[0] = 1;
    mem[1] = 2;
    mem[2] = 3;

    usize saved_offset = arena.next_offset;
    arena_pop(&arena, sizeof(u32));
    PRINT_EVAL(label, arena.next_offset == saved_offset - sizeof(u32));

    arena_pop_to(&arena, 0);
    PRINT_EVAL(label, arena.next_offset == 0);

    arena_free(&arena);
  }

  {
    const char *label = "scratch_begin / scratch_end";
    Arena arena = arena_make();
    u32 *mem = arena_calloc(&arena, 5, u32);
    mem[0] = 1;
    Scratch scratch = scratch_begin(&arena);
    u32 *temp = arena_calloc(&arena, 10, u32);
    temp[1] = 1;
    scratch_close(&scratch);
    PRINT_EVAL(label, arena.next_offset == 5 * sizeof(u32));

    arena_free(&arena);
  }

  Arena arena = arena_make();
  {
    const char *label = "char_is_whitespace";
    PRINT_EVAL(label, char_is_whitespace(' '));
    PRINT_EVAL(label, char_is_whitespace('\n'));
    PRINT_EVAL(label, char_is_whitespace('\t'));
    PRINT_EVAL(label, !char_is_whitespace('A'));
    PRINT_EVAL(label, !char_is_whitespace('1'));
  }

  {
    const char *label = "char_is_digit";
    PRINT_EVAL(label, char_is_digit('0'));
    PRINT_EVAL(label, char_is_digit('5'));
    PRINT_EVAL(label, char_is_digit('9'));
    PRINT_EVAL(label, !char_is_digit('a'));
    PRINT_EVAL(label, !char_is_digit(' '));
  }

  {
    const char *label = "char_is_digit_base";
    PRINT_EVAL(label, char_is_digit_base('0', 2));
    PRINT_EVAL(label, char_is_digit_base('1', 2));
    PRINT_EVAL(label, !char_is_digit_base('5', 2));
    PRINT_EVAL(label, char_is_digit_base('9', 10));
    PRINT_EVAL(label, !char_is_digit_base('A', 10));
    PRINT_EVAL(label, char_is_digit_base('0', 16));
    PRINT_EVAL(label, char_is_digit_base('a', 16));
    PRINT_EVAL(label, char_is_digit_base('A', 16));
    PRINT_EVAL(label, char_is_digit_base('f', 16));
    PRINT_EVAL(label, char_is_digit_base('F', 16));
    PRINT_EVAL(label, !char_is_digit_base('g', 10));
  }

  {
    const char *label = "string_hash_u32";
    String string1 = String("Hello");
    String string2 = String("Hello");
    String string3 = String("World");
    PRINT_EVAL(label, string_hash_u32(string1) == string_hash_u32(string2));
    PRINT_EVAL(label, string_hash_u32(string1) != string_hash_u32(string3));
  }

  {
    const char *label = "string_match";
    String string = String("Foo");
    PRINT_EVAL(label, string_match(string, String("Foo")));
    PRINT_EVAL(label, !string_match(string, String("Bar")));
    PRINT_EVAL(label, !string_match(string, String("Foo\n")));
    PRINT_EVAL(label, !string_match(string, String(" Foo ")));
  }

  {
    const char *label = "string_starts_with";
    String string = String("Arkham");
    PRINT_EVAL(label, string_starts_with(string, String("A")));
    PRINT_EVAL(label, string_starts_with(string, String("Ark")));
    PRINT_EVAL(label, !string_starts_with(string, String("ham")));
  }

  {
    const char *label = "string_from_c_string";
    char *c_string = "I'm null-terminated";
    String string = string_from_c_string(c_string);
    PRINT_EVAL(label, string.count == strlen(c_string));
    PRINT_EVAL(label, MEM_MATCH(c_string, string.v, string.count));
  }

  {
    const char *label = "string_to_c_string";
    String string = String("I'm NOT null-terminated");
    char *c_string = string_to_c_string(&arena, string);
    PRINT_EVAL(label, string.count == strlen(c_string));
    PRINT_EVAL(label, MEM_MATCH(c_string, string.v, string.count));
  }

  {
    const char *label = "string_skip";
    String string = String("Skipper");
    PRINT_EVAL(label, string_match(string_skip(string, 1), String("kipper")));
    PRINT_EVAL(label, string_match(string_skip(string, 2), String("ipper")));
    PRINT_EVAL(label, string_match(string_skip(string, string.count), String("")));
    PRINT_EVAL(label, string_match(string_skip(string, 8), String("")));
  }

  {
    const char *label = "string_chop";
    String string = String("Chopper");
    PRINT_EVAL(label, string_match(string_chop(string, 1), String("Choppe")));
    PRINT_EVAL(label, string_match(string_chop(string, 2), String("Chopp")));
    PRINT_EVAL(label, string_match(string_chop(string, string.count), String("")));
    PRINT_EVAL(label, string_match(string_chop(string, 10), String("")));
  }

  {
    const char *label = "string_trim_white_space";
    PRINT_EVAL(label, string_match(string_trim_whitespace(String("  Foo")), String("Foo")));
    PRINT_EVAL(label, string_match(string_trim_whitespace(String("Foo  ")), String("Foo")));
    PRINT_EVAL(label, string_match(string_trim_whitespace(String("Foo\n")), String("Foo")));
  }

  {
    const char *label = "string_substring";
    String string = String("SubstringTest");
    PRINT_EVAL(label, string_match(string_substring(string, 0, 6), String("Substr")));
    PRINT_EVAL(label, string_match(string_substring(string, 3, 9), String("string")));
    PRINT_EVAL(label, string_match(string_substring(string, 0, string.count), String("SubstringTest")));
    PRINT_EVAL(label, string_match(string_substring(string, 5, 20), String("ringTest")));
    PRINT_EVAL(label, string_match(string_substring(string, string.count, string.count), String("")));
  }

  {
    const char *label = "string_find_substring";
    String string = String("Find the needle in the haystack.");
    PRINT_EVAL(label, string_find_substring(string, 0, String("needle")) == 9);
    PRINT_EVAL(label, string_find_substring(string, 10, String("needle")) == string.count);
    PRINT_EVAL(label, string_find_substring(string, 0, String("haystack")) == 23);
    PRINT_EVAL(label, string_find_substring(string, 0, String("missing")) == string.count);
  }

  {
    const char *label = "string_split";
    String commas = String("Foo,bar,baz");
    String_Array commas_split = string_split(&arena, commas, String(","));
    PRINT_EVAL(label, commas_split.count == 3);
    PRINT_EVAL(label, string_match(commas_split.v[0], String("Foo")));
    PRINT_EVAL(label, string_match(commas_split.v[1], String("bar")));
    PRINT_EVAL(label, string_match(commas_split.v[2], String("baz")));
  }

  {
    const char *label = "string_split";
    String spaces = String("Foo bar baz");
    String_Array space_split = string_split(&arena, spaces, String(" "));
    PRINT_EVAL(label, space_split.count == 3);
    PRINT_EVAL(label, string_match(space_split.v[0], String("Foo")));
    PRINT_EVAL(label, string_match(space_split.v[1], String("bar")));
    PRINT_EVAL(label, string_match(space_split.v[2], String("baz")));
  }

  {
    const char *label = "string_split";
    String lines = String("\nFoo\nbar\nbaz");
    String_Array line_split = string_split(&arena, lines, String("\n"));
    PRINT_EVAL(label, line_split.count == 4);
    PRINT_EVAL(label, string_match(line_split.v[1], String("Foo")));
    PRINT_EVAL(label, string_match(line_split.v[2], String("bar")));
    PRINT_EVAL(label, string_match(line_split.v[3], String("baz")));
  }

  {
    const char *label = "string_split_whitespace";
    String ws = String("\n Foo  \n\nbar \r baz  \n");
    String_Array ws_split = string_split_whitepace(&arena, ws);
    PRINT_EVAL(label, ws_split.count == 3);
    PRINT_EVAL(label, string_match(ws_split.v[0], String("Foo")));
    PRINT_EVAL(label, string_match(ws_split.v[1], String("bar")));
    PRINT_EVAL(label, string_match(ws_split.v[2], String("baz")));
  }
}
