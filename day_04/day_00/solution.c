#define STDR_IMPLEMENTATION
#include "stdr.h"

#ifndef DAYDIR
#define DAYDIR
#endif

i64 part_1(str_t input) {
  (void)input;
  return 0;
}

i64 part_2(str_t input) {
  (void)input;

  return 0;
}

i32 main(void) {
  stdr_arena_ctx_init(1024 * 32, &STDR_ALLOCATOR_LIBC);
  str_t example = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "example.txt");
  str_t input = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "input.txt");

  STDR_ASSERT(part_1(example) == 0);
  STDR_ASSERT(part_1(input) == 0);
  STDR_ASSERT(part_2(example) == 0);
  STDR_ASSERT(part_2(input) == 0);

  stdr_arena_ctx_delete();

  return 0;
}
