#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "testing.h"
#include "testing.c"

int main(int argc, char **argv)
{
  TEST_BLOCK(STR("CLAMP"))
  {

    {
      i32 min = 1;
      i32 max = 11;
      i32 a = 10;

      TEST_EVAL(CLAMP(a, min, max) == a);
    }

    {
      i32 min = 1;
      i32 max = 9;
      i32 a = 10;

      TEST_EVAL(CLAMP(a, min, max) == max);
    }

    {
      i32 min = 11;
      i32 max = 15;
      i32 a = 10;

      TEST_EVAL(CLAMP(a, min, max) == min);
    }
  }

  TEST_BLOCK(STR("MAX"))
  {
    i32 a = 4;
    i32 b = 5;

    TEST_EVAL(MAX(a, b) == b);
    TEST_EVAL(MAX(a, 10) != a);

    f32 fa = 4;
    f32 fb = 5;

    TEST_EVAL(MAX(fa, fb) == fb);
    TEST_EVAL(MAX(fa, 10.0) != fa);
  }

  TEST_BLOCK(STR("MIN"))
  {
    i32 a = 4;
    i32 b = 5;

    TEST_EVAL(MIN(a, b) != b);
    TEST_EVAL(MIN(a, 10) == a);

    f32 fa = 4;
    f32 fb = 5;

    TEST_EVAL(MIN(fa, fb) != fb);
    TEST_EVAL(MIN(fa, 10.0) == fa);
  }

  TEST_BLOCK(STR("SWAP"))
  {
    i32 a = 4;
    i32 b = 5;

    SWAP(a, b, i32);

    TEST_EVAL(a == 5);
    TEST_EVAL(b == 4);
  }

  TEST_BLOCK(STR("STATIC_COUNT"))
  {
    i32 array[16] = {0};
    TEST_EVAL(STATIC_COUNT(array) == 16);
  }

  TEST_BLOCK(STR("arena_alloc / arena_make"))
  {
    Arena arena = arena_make();
    u8 *mem1 = arena_alloc(&arena, 16, 8);
    u8 *mem2 = arena_alloc(&arena, 32, 16);

    TEST_EVAL(mem1 != NULL);
    TEST_EVAL(mem2 != NULL);
    TEST_EVAL(mem2 > mem1);

    arena_clear(&arena);
    u8 *mem3 = arena_alloc(&arena, 8, 4);
    TEST_EVAL(mem3 == arena.base);

    arena_free(&arena);
  }

  TEST_BLOCK(STR("arena_calloc"))
  {
    Arena arena = arena_make();
    u32 *arr = arena_calloc(&arena, 4, u32);
    TEST_EVAL(arr != NULL);
    TEST_EVAL(arr[0] == 0 && arr[1] == 0 && arr[2] == 0 && arr[3] == 0);

    arena_free(&arena);
  }

  TEST_BLOCK(STR("arena_array"))
  {
    Arena arena = arena_make();
    i32_Array array = arena_array(&arena, 4, i32);

    array.v[0] = 10;
    array.v[1] = 20;
    array.v[2] = 30;
    array.v[3] = 40;

    TEST_EVAL(array.count == 4);
    TEST_EVAL(array.v[0] == 10 && array.v[1] == 20 && array.v[2] == 30 && array.v[3] == 40);

    arena_free(&arena);
  }

  TEST_BLOCK(STR("array_add"))
  {
    Arena arena = arena_make();
    i32_Array array = {0};

    i32 val1 = 42;
    i32 val2 = 17;
    int *added1 = array_add(&arena, array, val1);
    int *added2 = array_add(&arena, array, val2);

    TEST_EVAL(added1 != NULL && added2 != NULL);
    TEST_EVAL(array.count == 2);
    TEST_EVAL(array.v[0] == 42 && array.v[1] == 17);
    TEST_EVAL(*added1 == 42 && *added2 == 17);

    arena_free(&arena);
  }

  TEST_BLOCK(STR("SLL_push_first"))
  {

    i32_List list = {0};

    i32_Node *ptr = NULL;

    i32_Node val1 = {NULL, 42};
    i32_Node val2 = {NULL, 17};
    i32_Node val3 = {NULL, 10};

    ptr = SLL_push_first(list.first, list.last, &val1, link_next);
    TEST_EVAL(list.first->value == 42);
    TEST_EVAL(list.last->value == 42);
    TEST_EVAL(ptr->value == 42);

    ptr = SLL_push_first(list.first, list.last, &val2, link_next);
    TEST_EVAL(list.first->value == 17);
    TEST_EVAL(list.last->value == 42);
    TEST_EVAL(ptr->value == 17);

    ptr = SLL_push_first(list.first, list.last, &val3, link_next);
    TEST_EVAL(list.first->value == 10);
    TEST_EVAL(list.last->value == 42);
    TEST_EVAL(ptr->value == 10);
  }

  TEST_BLOCK(STR("SLL_push_last"))
  {

    i32_List list = {0};

    i32_Node *ptr = NULL;

    i32_Node val1 = {NULL, 42};
    i32_Node val2 = {NULL, 17};
    i32_Node val3 = {NULL, 10};

    ptr = SLL_push_last(list.first, list.last, &val1, link_next);
    TEST_EVAL(list.first->value == 42);
    TEST_EVAL(list.last->value == 42);
    TEST_EVAL(ptr->value == 42);

    ptr = SLL_push_last(list.first, list.last, &val2, link_next);
    TEST_EVAL(list.first->value == 42);
    TEST_EVAL(list.last->value == 17);
    TEST_EVAL(ptr->value == 17);

    ptr = SLL_push_last(list.first, list.last, &val3, link_next);
    TEST_EVAL(list.first->value == 42);
    TEST_EVAL(list.last->value == 10);
    TEST_EVAL(ptr->value == 10);
  }

  TEST_BLOCK(STR("list_push_first"))
  {

    i32_List list = {0};

    i32_Node *ptr = NULL;

    i32_Node val1 = {NULL, 42};
    i32_Node val2 = {NULL, 17};
    i32_Node val3 = {NULL, 10};

    ptr = list_push_first(&list, &val1);
    TEST_EVAL(list.first->value == 42);
    TEST_EVAL(list.last->value == 42);
    TEST_EVAL(ptr->value == 42);
    TEST_EVAL(list.count == 1);

    ptr = list_push_first(&list, &val2);
    TEST_EVAL(list.first->value == 17);
    TEST_EVAL(list.last->value == 42);
    TEST_EVAL(ptr->value == 17);
    TEST_EVAL(list.count == 2);

    ptr = list_push_first(&list, &val3);
    TEST_EVAL(list.first->value == 10);
    TEST_EVAL(list.last->value == 42);
    TEST_EVAL(ptr->value == 10);
    TEST_EVAL(list.count == 3);
  }

  TEST_BLOCK(STR("list_push_last"))
  {

    i32_List list = {0};

    i32_Node *ptr = NULL;

    i32_Node val1 = {NULL, 42};
    i32_Node val2 = {NULL, 17};
    i32_Node val3 = {NULL, 10};

    ptr = list_push_last(&list, &val1);
    TEST_EVAL(list.first->value == 42);
    TEST_EVAL(list.last->value == 42);
    TEST_EVAL(ptr->value == 42);
    TEST_EVAL(list.count == 1);

    ptr = list_push_last(&list, &val2);
    TEST_EVAL(list.first->value == 42);
    TEST_EVAL(list.last->value == 17);
    TEST_EVAL(ptr->value == 17);
    TEST_EVAL(list.count == 2);

    ptr = list_push_last(&list, &val3);
    TEST_EVAL(list.first->value == 42);
    TEST_EVAL(list.last->value == 10);
    TEST_EVAL(ptr->value == 10);
    TEST_EVAL(list.count == 3);
  }

  TEST_BLOCK(STR("arena_pop / arena_pop_to"))
  {
    Arena arena = arena_make();
    u32 *mem = arena_calloc(&arena, 3, u32);
    mem[0] = 1;
    mem[1] = 2;
    mem[2] = 3;

    usize saved_offset = arena.next_offset;
    arena_pop(&arena, sizeof(u32));
    TEST_EVAL(arena.next_offset == saved_offset - sizeof(u32));

    arena_pop_to(&arena, 0);
    TEST_EVAL(arena.next_offset == 0);

    arena_free(&arena);
  }

  TEST_BLOCK(STR("scratch_begin / scratch_end"))
  {
    Arena arena = arena_make();
    u32 *mem = arena_calloc(&arena, 5, u32);
    mem[0] = 1;
    Scratch scratch = scratch_begin(&arena);
    u32 *temp = arena_calloc(&arena, 10, u32);
    temp[1] = 1;
    scratch_close(&scratch);
    TEST_EVAL(arena.next_offset == 5 * sizeof(u32));

    arena_free(&arena);
  }

  Arena arena = arena_make();
  TEST_BLOCK(STR("char_is_whitespace"))
  {
    TEST_EVAL(char_is_whitespace(' '));
    TEST_EVAL(char_is_whitespace('\n'));
    TEST_EVAL(char_is_whitespace('\t'));
    TEST_EVAL(!char_is_whitespace('A'));
    TEST_EVAL(!char_is_whitespace('1'));
  }

  TEST_BLOCK(STR("char_is_digit"))
  {
    TEST_EVAL(char_is_digit('0'));
    TEST_EVAL(char_is_digit('5'));
    TEST_EVAL(char_is_digit('9'));
    TEST_EVAL(!char_is_digit('a'));
    TEST_EVAL(!char_is_digit(' '));
  }

  TEST_BLOCK(STR("char_is_digit_base"))
  {
    TEST_EVAL(char_is_digit_base('0', 2));
    TEST_EVAL(char_is_digit_base('1', 2));
    TEST_EVAL(!char_is_digit_base('5', 2));
    TEST_EVAL(char_is_digit_base('9', 10));
    TEST_EVAL(!char_is_digit_base('A', 10));
    TEST_EVAL(char_is_digit_base('0', 16));
    TEST_EVAL(char_is_digit_base('a', 16));
    TEST_EVAL(char_is_digit_base('A', 16));
    TEST_EVAL(char_is_digit_base('f', 16));
    TEST_EVAL(char_is_digit_base('F', 16));
    TEST_EVAL(!char_is_digit_base('g', 10));
  }

  TEST_BLOCK(STR("string_hash_u32"))
  {
    String string1 = String("Hello");
    String string2 = String("Hello");
    String string3 = String("World");
    TEST_EVAL(string_hash_u32(string1) == string_hash_u32(string2));
    TEST_EVAL(string_hash_u32(string1) != string_hash_u32(string3));
  }

  TEST_BLOCK(STR("string_match"))
  {
    String string = String("Foo");
    TEST_EVAL(string_match(string, String("Foo")));
    TEST_EVAL(!string_match(string, String("Bar")));
    TEST_EVAL(!string_match(string, String("Foo\n")));
    TEST_EVAL(!string_match(string, String(" Foo ")));
  }

  TEST_BLOCK(STR("string_starts_with"))
  {
    String string = String("Arkham");
    TEST_EVAL(string_starts_with(string, String("A")));
    TEST_EVAL(string_starts_with(string, String("Ark")));
    TEST_EVAL(!string_starts_with(string, String("ham")));
  }

  TEST_BLOCK(STR("string_from_c_string"))
  {
    char *c_string = "I'm null-terminated";
    String string = string_from_c_string(c_string);
    TEST_EVAL(string.count == strlen(c_string));
    TEST_EVAL(MEM_MATCH(c_string, string.v, string.count));
  }

  TEST_BLOCK(STR("string_to_c_string"))
  {
    String string = String("I'm NOT null-terminated");
    char *c_string = string_to_c_string(&arena, string);
    TEST_EVAL(string.count == strlen(c_string));
    TEST_EVAL(MEM_MATCH(c_string, string.v, string.count));
  }

  TEST_BLOCK(STR("string_skip"))
  {
    String string = String("Skipper");
    TEST_EVAL(string_match(string_skip(string, 1), String("kipper")));
    TEST_EVAL(string_match(string_skip(string, 2), String("ipper")));
    TEST_EVAL(string_match(string_skip(string, string.count), String("")));
    TEST_EVAL(string_match(string_skip(string, 8), String("")));
  }

  TEST_BLOCK(STR("string_chop"))
  {
    String string = String("Chopper");
    TEST_EVAL(string_match(string_chop(string, 1), String("Choppe")));
    TEST_EVAL(string_match(string_chop(string, 2), String("Chopp")));
    TEST_EVAL(string_match(string_chop(string, string.count), String("")));
    TEST_EVAL(string_match(string_chop(string, 10), String("")));
  }

  TEST_BLOCK(STR("string_trim_white_space"))
  {
    TEST_EVAL(string_match(string_trim_whitespace(String("  Foo")), String("Foo")));
    TEST_EVAL(string_match(string_trim_whitespace(String("Foo  ")), String("Foo")));
    TEST_EVAL(string_match(string_trim_whitespace(String("Foo\n")), String("Foo")));
  }

  TEST_BLOCK(STR("string_substring"))
  {
    String string = String("SubstringTest");
    TEST_EVAL(string_match(string_substring(string, 0, 6), String("Substr")));
    TEST_EVAL(string_match(string_substring(string, 3, 9), String("string")));
    TEST_EVAL(string_match(string_substring(string, 0, string.count), String("SubstringTest")));
    TEST_EVAL(string_match(string_substring(string, 5, 20), String("ringTest")));
    TEST_EVAL(string_match(string_substring(string, string.count, string.count), String("")));
  }

  TEST_BLOCK(STR("string_find_substring"))
  {
    String string = String("Find the needle in the haystack.");
    TEST_EVAL(string_find_substring(string, 0, String("needle")) == 9);
    TEST_EVAL(string_find_substring(string, 10, String("needle")) == string.count);
    TEST_EVAL(string_find_substring(string, 0, String("haystack")) == 23);
    TEST_EVAL(string_find_substring(string, 0, String("missing")) == string.count);
  }

  TEST_BLOCK(STR("string_split"))
  {
    String commas = String("Foo,bar,baz");
    String_Array commas_split = string_split(&arena, commas, String(","));
    TEST_EVAL(commas_split.count == 3);
    TEST_EVAL(string_match(commas_split.v[0], String("Foo")));
    TEST_EVAL(string_match(commas_split.v[1], String("bar")));
    TEST_EVAL(string_match(commas_split.v[2], String("baz")));
  }

  TEST_BLOCK(STR("string_split"))
  {
    String spaces = String("Foo bar baz");
    String_Array space_split = string_split(&arena, spaces, String(" "));
    TEST_EVAL(space_split.count == 3);
    TEST_EVAL(string_match(space_split.v[0], String("Foo")));
    TEST_EVAL(string_match(space_split.v[1], String("bar")));
    TEST_EVAL(string_match(space_split.v[2], String("baz")));
  }

  TEST_BLOCK(STR("string_split"))
  {
    String lines = String("\nFoo\nbar\nbaz");
    String_Array line_split = string_split(&arena, lines, String("\n"));
    TEST_EVAL(line_split.count == 4);
    TEST_EVAL(string_match(line_split.v[1], String("Foo")));
    TEST_EVAL(string_match(line_split.v[2], String("bar")));
    TEST_EVAL(string_match(line_split.v[3], String("baz")));
  }

  TEST_BLOCK(STR("string_split_whitespace"))
  {
    String ws = String("\n Foo  \n\nbar \r baz  \n");
    String_Array ws_split = string_split_whitepace(&arena, ws);
    TEST_EVAL(ws_split.count == 3);
    TEST_EVAL(string_match(ws_split.v[0], String("Foo")));
    TEST_EVAL(string_match(ws_split.v[1], String("bar")));
    TEST_EVAL(string_match(ws_split.v[2], String("baz")));
  }

  TEST_BLOCK(STR("string_formatted"))
  {
    String test = STR("Hello, sailor");
    String form = string_formatted(&arena, "%s", "Hello, sailor");
    printf("Test: %.*s\nForm: %.*s\n", STRF(test), STRF(form));
    TEST_EVAL(test.count == form.count);
    TEST_EVAL(string_match(test, form));
  }

  tester_summarize();

  arena_free(&arena);
}
