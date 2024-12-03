#define STDR_IMPLEMENTATION
#include "stdr.h"

int cmp(const void* a, const void* b) { return (int)(*(i64*)a - *(i64*)b); }

i64 part_1(str_t input) {
  stdr_array(i64) xs = stdr_array_new(i64, &STDR_ALLOCATOR_LIBC);
  stdr_array(i64) ys = stdr_array_new(i64, &STDR_ALLOCATOR_LIBC);

  str_t row;
  str_t rest = input;
  while (stdr_str_split_once(rest, S("\n"), &row, &rest)) {
    str_t s1, s2;
    bool ok = stdr_str_split_once(row, S("   "), &s1, &s2);
    assert(ok);
    i64 v1 = stdr_str_parse_i64(s1);
    i64 v2 = stdr_str_parse_i64(s2);

    stdr_array_append(&xs, v1);
    stdr_array_append(&ys, v2);
  }

  qsort(xs, stdr_array_count(xs), sizeof(*xs), cmp);
  qsort(ys, stdr_array_count(ys), sizeof(*ys), cmp);

  i64 sum = 0;
  for (usize i = 0; i < stdr_array_count(xs); i++) {
    i64 d = STDR_ABS(xs[i] - ys[i]);
    sum += d;
  }

  stdr_array_delete(&ys);
  stdr_array_delete(&xs);

  return sum;
}

i64 part_2(str_t input) {
  stdr_array(i64) xs = stdr_array_new(i64, &STDR_ALLOCATOR_LIBC);
  stdr_array(i64) ys = stdr_array_new(i64, &STDR_ALLOCATOR_LIBC);

  str_t row;
  str_t rest = input;
  while (stdr_str_split_once(rest, S("\n"), &row, &rest)) {
    str_t s1, s2;
    assert(stdr_str_split_once(row, S("   "), &s1, &s2));
    stdr_array_append(&xs, stdr_str_parse_i64(s1));
    stdr_array_append(&ys, stdr_str_parse_i64(s2));
  }

  i64 sum = 0;
  for (usize i = 0; i < stdr_array_count(xs); i++) {
    i64 count = 0;
    for (usize j = 0; j < stdr_array_count(ys); j++) {
      if (xs[i] == ys[j]) {
        count++;
      }
    }
    sum += xs[i] * count;
  }

  stdr_array_delete(&ys);
  stdr_array_delete(&xs);

  return sum;
}

#ifndef DAYDIR
#define DAYDIR ""
#endif

i32 main(void) {
  stdr_arena_ctx_init(1024 * 32, &STDR_ALLOCATOR_LIBC);
  str_t example = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "example.txt");
  str_t input = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "input.txt");

  STDR_ASSERT(part_1(example) == 11);
  STDR_ASSERT(part_1(input) == 1151792);
  STDR_ASSERT(part_2(example) == 31);
  STDR_ASSERT(part_2(input) == 21790168);

  return 0;
}
