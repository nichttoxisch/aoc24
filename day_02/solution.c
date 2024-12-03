#define STDR_IMPLEMENTATION
#include "stdr.h"

#ifndef DAYDIR
#define DAYDIR
#endif

array(array(i64)) parse_input(str_t input) {
  array(array(i64)) xs = array_new(array(i64), &STDR_ALLOCATOR_LIBC);

  str_t row;
  str_t rest = input;
  usize ii = 0;
  while (stdr_str_split_once(rest, S("\n"), &row, &rest)) {
    array_append(&xs, array_new(i64, &STDR_ALLOCATOR_LIBC));

    str_t col;
    str_t row_rest = row;
    while (stdr_str_split_once(row_rest, S(" "), &col, &row_rest)) {
      i64 v = stdr_str_parse_i64(col);
      array_append(&xs[ii], v);
    }
    i64 v = stdr_str_parse_i64(row_rest);
    array_append(&xs[ii], v);
    ii++;
  }

  return xs;
}

bool entry_is_valid(array(i64) entry) {
  bool valid = TRUE;
  if (entry[0] < array_last(entry)) {
    // All levels should be increasing
    for (usize j = 1; j < array_count(entry); j++) {
      if (!(entry[j - 1] < entry[j])) {
        valid = FALSE;
        break;
      }
      if (entry[j] - entry[j - 1] > 3) {
        valid = FALSE;
        break;
      }
    }
  } else {
    // All levels should be decreasing
    for (usize j = 1; j < array_count(entry); j++) {
      if (!(entry[j - 1] > entry[j])) {
        valid = FALSE;
        break;
      }
      if (entry[j - 1] - entry[j] > 3) {
        valid = FALSE;
        break;
      }
    }
  }
  return valid;
}

i64 part_1(str_t input) {
  array(array(i64)) xs = parse_input(input);

  i64 count_valid = 0;
  for (usize i = 0; i < array_count(xs); i++) {
    if (entry_is_valid(xs[i])) {
      count_valid++;
    }
  }

  forr(array(i64), e, xs) { array_delete(e); }
  array_delete(&xs);

  return count_valid;
}

array(i64) array_skip(array(i64) xs, usize idx) {
  array(i64) ys = array_new(i64, stdr_array_header(xs)->allocator);

  for (usize k = 0; k < array_count(xs); k++) {
    if (k == idx) continue;
    array_append(&ys, xs[k]);
  }

  return ys;
}

i64 part_2(str_t input) {
  array(array(i64)) xs = parse_input(input);

  i64 count_valid = 0;
  for (usize i = 0; i < array_count(xs); i++) {
    for (usize j = 0; j < array_count(xs[i]); j++) {
      array(i64) ys = array_skip(xs[i], j);
      if (entry_is_valid(ys)) {
        count_valid++;
        array_delete(&ys);
        break;
      }
      array_delete(&ys);
    }
  }

  forr(array(i64), e, xs) { array_delete(e); }
  array_delete(&xs);

  return count_valid;
}

i32 main(void) {
  stdr_arena_ctx_init(1024 * 64, &STDR_ALLOCATOR_LIBC);
  str_t example = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "example.txt");
  str_t input = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "input.txt");

  STDR_ASSERT(part_1(example) == 2);
  STDR_ASSERT(part_1(input) == 559);
  STDR_ASSERT(part_2(example) == 4);
  STDR_ASSERT(part_2(input) == 601);

  stdr_arena_ctx_delete();

  return 0;
}
