#define STDR_IMPLEMENTATION
#include "stdr.h"
#define STDR_REGEX_IMPLEMENTATION
#include "stdr_regex.h"

#ifndef DAYDIR
#define DAYDIR
#endif

i64 parse_expr(str_t expr) {
  expr = stdr_str_drop_begin(expr, 4);
  str_t s1, s2;
  assert(stdr_str_split_once(expr, S(","), &s1, &s2));
  s2 = stdr_str_drop_end(s2, 1);

  i64 v1 = stdr_str_parse_i64(s1);
  i64 v2 = stdr_str_parse_i64(s2);

  return v1 * v2;
};

i64 part_1(str_t input) {
  regex_t regex = {0};
  regex_state_t s0 = regex_push_state(&regex);  // begin-state
  regex_compile(
      &regex, s0,
      S("mul(\\d+,\\d+)"));  // Note: parenthesis are character literals

  i64 sum = 0;

  for (usize i = 0; i < input.size; i++) {
    str_t tstr = {.ptr = &input.ptr[i], .size = input.size - i};
    str_t match = regex_match_str_start(&regex, s0, tstr, 0, tstr.ptr);
    if (match.ptr == NULL) continue;
    sum += parse_expr(match);
  }

  regex_delete(&regex);
  return sum;
}

i64 part_2(str_t input) {
  regex_t regex_mul = {0};
  regex_state_t s0_mul = regex_push_state(&regex_mul);
  regex_compile(&regex_mul, s0_mul, S("mul(\\d+,\\d+)"));

  i64 sum = 0;
  bool mul_enabled = TRUE;

  for (usize i = 0; i < input.size; i++) {
    str_t tstr = {.ptr = &input.ptr[i], .size = input.size - i};
    if (stdr_str_starts_with(tstr, S("do()"))) mul_enabled = TRUE;
    if (stdr_str_starts_with(tstr, S("don't()"))) mul_enabled = FALSE;

    str_t match = regex_match_str_start(&regex_mul, s0_mul, tstr, 0, tstr.ptr);
    if (match.ptr == NULL) continue;
    if (!mul_enabled) continue;
    // stdr_stdout_fmt(S("%s\n"), match);
    sum += parse_expr(match);
  }

  regex_delete(&regex_mul);
  return sum;
}

i32 main(void) {
  stdr_arena_ctx_init(1024 * 64, &STDR_ALLOCATOR_LIBC);
  str_t example = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "example.txt");
  str_t input = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "input.txt");

  STDR_ASSERT(part_1(example) == 161);
  STDR_ASSERT(part_1(input) == 174561379);
  STDR_ASSERT(part_2(example) == 48);
  STDR_ASSERT(part_2(input) == 106921067);

  stdr_arena_ctx_delete();

  return 0;
}
