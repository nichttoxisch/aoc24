
#define STDR_IMPLEMENTATION
#include "stdr.h"

//
#include <math.h>

#ifndef DAYDIR
#define DAYDIR
#endif

typedef struct {
  i64 target;
  array(i64) ns;
} sample_t;

array(sample_t) input_parse(str_t input) {
  array(sample_t) xs = array_new(sample_t, &ALLOCATOR_LIBC);

  str_t row;
  str_t rest = input;

  while (stdr_str_split_once(rest, S("\n"), &row, &rest)) {
    sample_t s;
    s.ns = array_new(i64, &ALLOCATOR_LIBC);

    str_t target_str, numbers_str;
    stdr_str_split_once(row, S(": "), &target_str, &numbers_str);
    s.target = stdr_str_parse_i64(target_str);

    str_t nstr;
    while (stdr_str_split_once(numbers_str, S(" "), &nstr, &numbers_str)) {
      array_append(&s.ns, stdr_str_parse_i64(nstr));
    }
    array_append(&s.ns, stdr_str_parse_i64(numbers_str));

    array_append(&xs, s);
  }

  return xs;
}

typedef struct {
  uptr ptr;
  stdr_usize count;
  stdr_usize item_size;
} stdr_slice_t;

stdr_slice_t stdr_slice_drop_begin(stdr_slice_t slice, stdr_usize n) {
  slice.count -= n;
  slice.ptr += slice.item_size * n;
  return slice;
}

stdr_slice_t stdr_array_as_slice(void* arr) {
  stdr_slice_t slice;
  slice.ptr = (uptr)arr;
  slice.count = stdr_array_header(arr)->count;
  slice.item_size = stdr_array_header(arr)->item_size;
  return slice;
}

#define stdr_slice_get(T, slice, index) \
  (((T*)(slice).ptr)[index]);           \
  STDR_ASSERT(sizeof(T) == (slice).item_size)

bool solve(i64 sum, stdr_slice_t xs, i64 target) {
  if (xs.count == 0) return sum == target;

  i64 x = stdr_slice_get(i64, xs, 0);

  if (solve(sum * x, stdr_slice_drop_begin(xs, 1), target)) return TRUE;
  if (solve(sum + x, stdr_slice_drop_begin(xs, 1), target)) return TRUE;

  return FALSE;
}

i64 part_1(str_t input) {
  array(sample_t) xs = input_parse(input);

  // for (usize i = 0; i < array_count(xs); i++) {
  //   printf("%ld: ", xs[i].target);

  //   for (usize j = 0; j < array_count(xs[i].ns); j++) {
  //     printf("%ld ", xs[i].ns[j]);
  //   }
  //   printf("\n");
  // }

  i64 sum = 0;
  for (size_t i = 0; i < array_count(xs); i++) {
    sample_t s = xs[i];
    stdr_slice_t ns = stdr_array_as_slice(s.ns);
    i64 n = stdr_slice_get(i64, ns, 0);
    ns = stdr_slice_drop_begin(ns, 1);
    if (solve(n, ns, s.target)) {
      sum += s.target;
    }
  }

  forr(sample_t, s, xs) { array_delete(&s->ns); }
  array_delete(&xs);

  return sum;
}

i64 concat(i64 n1, i64 n2) {
  i64 n2_digits = 0;

  if (n2 == 0) {
    n2_digits = 1;
  } else {
    n2_digits = (i64)log10((f64)n2) + 1;
  }

  return n1 * (i64)pow(10.0, (f64)n2_digits) + n2;
}

bool solve2(i64 sum, stdr_slice_t xs, i64 target) {
  if (xs.count == 0) return sum == target;

  i64 x = stdr_slice_get(i64, xs, 0);
  xs = stdr_slice_drop_begin(xs, 1);

  if (solve2(sum + x, xs, target)) return TRUE;
  if (solve2(sum * x, xs, target)) return TRUE;
  if (solve2(concat(sum, x), xs, target)) return TRUE;

  return FALSE;
}

i64 part_2(str_t input) {
  array(sample_t) xs = input_parse(input);

  i64 sum = 0;
  for (size_t i = 0; i < array_count(xs); i++) {
    sample_t s = xs[i];
    stdr_slice_t ns = stdr_array_as_slice(s.ns);
    i64 n = stdr_slice_get(i64, ns, 0);
    ns = stdr_slice_drop_begin(ns, 1);
    if (solve2(n, ns, s.target)) {
      sum += s.target;
    }
  }

  forr(sample_t, s, xs) { array_delete(&s->ns); }
  array_delete(&xs);

  return sum;
}

i32 main(void) {
  stdr_arena_ctx_init(1024 * 64, &STDR_ALLOCATOR_LIBC);
  str_t example = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "example.txt");
  str_t input = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "input.txt");

  STDR_ASSERT(part_1(example) == 3749);
  STDR_ASSERT(part_1(input) == 3598800864292);
  STDR_ASSERT(part_2(example) == 11387);
  STDR_ASSERT(part_2(input) == 340362529351427);

  stdr_arena_ctx_delete();

  return 0;
}
