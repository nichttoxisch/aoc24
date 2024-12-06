#define STDR_IMPLEMENTATION
#include "stdr.h"

#ifndef DAYDIR
#define DAYDIR
#endif

i64 part_1(str_t input) {
  stdr_array(str_t) xs = stdr_array_new(str_t, &STDR_ALLOCATOR_LIBC);

  str_t row;
  str_t rest = input;
  while (stdr_str_split_once(rest, S("\n"), &row, &rest)) {
    array_append(&xs, row);
  }

  i64 count = 0;

  // Horizontal
  for (usize i = 0; i < array_count(xs); i++) {
    str_t s = xs[i];
    for (usize j = 0; j < xs[i].size - 3; j++) {
      char buf[4] = {s.ptr[j + 0], s.ptr[j + 1], s.ptr[j + 2], s.ptr[j + 3]};
      if (strncmp(buf, "XMAS", 4) == 0) {
        count++;
      };
    }
  }

  // Horizontal Reversed
  for (usize i = 0; i < array_count(xs); i++) {
    str_t s = xs[i];
    for (usize j = 0; j < s.size - 3; j++) {
      char buf[4] = {s.ptr[j + 3], s.ptr[j + 2], s.ptr[j + 1], s.ptr[j + 0]};
      if (strncmp(buf, "XMAS", 4) == 0) {
        count++;
      };
    }
  }

  // Diagonal Down
  for (usize i = 0; i < array_count(xs) - 3; i++) {
    for (usize j = 0; j < xs[i].size - 3; j++) {
      char buf[4] = {xs[i + 0].ptr[j + 0], xs[i + 1].ptr[j + 1],
                     xs[i + 2].ptr[j + 2], xs[i + 3].ptr[j + 3]};
      if (strncmp(buf, "XMAS", 4) == 0) {
        count++;
      };
    }
  }

  // Diagonal Down Reversed
  for (usize i = 0; i < array_count(xs) - 3; i++) {
    for (usize j = 0; j < xs[i].size - 3; j++) {
      char buf[4] = {xs[i + 3].ptr[j + 3], xs[i + 2].ptr[j + 2],
                     xs[i + 1].ptr[j + 1], xs[i + 0].ptr[j + 0]};
      if (strncmp(buf, "XMAS", 4) == 0) {
        count++;
      };
    }
  }

  // Diagonal Up
  for (usize i = 0; i < array_count(xs) - 3; i++) {
    for (usize j = 0; j < xs[i].size - 3;
         j++) {  // All rows are of the same size
      char buf[4] = {xs[i + 3].ptr[j + 0], xs[i + 2].ptr[j + 1],
                     xs[i + 1].ptr[j + 2], xs[i + 0].ptr[j + 3]};
      if (strncmp(buf, "XMAS", 4) == 0) {
        count++;
      };
    }
  }

  // Diagonal Up Reversed
  for (usize i = 0; i < array_count(xs) - 3; i++) {
    for (usize j = 0; j < xs[i].size - 3;
         j++) {  // All rows are of the same size
      char buf[4] = {xs[i + 0].ptr[j + 3], xs[i + 1].ptr[j + 2],
                     xs[i + 2].ptr[j + 1], xs[i + 3].ptr[j + 0]};
      if (strncmp(buf, "XMAS", 4) == 0) {
        count++;
      };
    }
  }

  // Vertical
  for (usize i = 0; i < array_count(xs) - 3; i++) {
    for (usize j = 0; j < xs[i].size; j++) {
      char buf[4] = {xs[i + 0].ptr[j], xs[i + 1].ptr[j], xs[i + 2].ptr[j],
                     xs[i + 3].ptr[j]};
      if (strncmp(buf, "XMAS", 4) == 0) {
        count++;
      };
    }
  }

  // Vertical Reversed
  for (usize i = 0; i < array_count(xs) - 3; i++) {
    for (usize j = 0; j < xs[i].size; j++) {
      char buf[4] = {xs[i + 3].ptr[j], xs[i + 2].ptr[j], xs[i + 1].ptr[j],
                     xs[i + 0].ptr[j]};
      if (strncmp(buf, "XMAS", 4) == 0) {
        count++;
      };
    }
  }

  array_delete(&xs);

  return count;
}

i64 part_2(str_t input) {
  stdr_array(str_t) xs = stdr_array_new(str_t, &STDR_ALLOCATOR_LIBC);

  str_t row;
  str_t rest = input;
  while (stdr_str_split_once(rest, S("\n"), &row, &rest)) {
    array_append(&xs, row);
  }
  i64 count = 0;

  for (usize i = 1; i < array_count(xs) - 1; i++) {
    for (usize j = 1; j < xs[0].size - 1; j++) {
      if (!(xs[i].ptr[j] == 'A')) continue;
      if (!((xs[i - 1].ptr[j - 1] == 'M' && xs[i + 1].ptr[j + 1] == 'S') ||
            (xs[i - 1].ptr[j - 1] == 'S' && xs[i + 1].ptr[j + 1] == 'M')))
        continue;
      if (!((xs[i - 1].ptr[j + 1] == 'M' && xs[i + 1].ptr[j - 1] == 'S') ||
            (xs[i - 1].ptr[j + 1] == 'S' && xs[i + 1].ptr[j - 1] == 'M')))
        continue;

      count++;
    }
  }

  array_delete(&xs);
  return count;
}

i32 main(void) {
  stdr_arena_ctx_init(1024 * 64, &STDR_ALLOCATOR_LIBC);
  str_t example = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "example.txt");
  str_t input = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "input.txt");

  STDR_ASSERT(part_1(example) == 18);
  STDR_ASSERT(part_1(input) == 2447);
  STDR_ASSERT(part_2(example) == 9);
  STDR_ASSERT(part_2(input) == 1868);

  stdr_arena_ctx_delete();

  return 0;
}
