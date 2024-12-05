#ifndef CC
#define CC "gcc"
#endif

#define STDR_IMPLEMENTATION
#include "stdr.h"

#define CFLAGS S("-std=c11"), S("-ggdb"), S("-O0")

#define WFLAGS                                                               \
  S("-Wall"), S("-Wextra"), S("-Wconversion"), S("-Wsign-conversion"),       \
      S("-Wswitch"), S("-Wformat"), S("-Wshadow"), S("-Wstrict-overflow"),   \
      S("-Wnull-dereference"), S("-Wformat-security"), S("-Wuninitialized"), \
      S("-Wunreachable-code"), S("-Wundef"), S("-Wfloat-equal"),             \
      S("-Wunused-parameter"), S("-Wmissing-field-initializers"),            \
      S("-Wimplicit-fallthrough"), S("-Wdeprecated")

#define FFLAGS                                                           \
  S("-fstack-protector-all"), S("-fno-omit-frame-pointer"), S("-lasan"), \
      S("-L/usr/lib/gcc/x86_64-linux-gnu/12/"), S("-ftrapv"),            \
      S("-fstrict-aliasing"), S("-fsanitize=address,undefined")

#define INC S("-I.")

void build_advent(str_t file) {
  stdr_arena_t arena;
  stdr_arena_init(&arena, 1024, &STDR_ALLOCATOR_LIBC);

  str_t binpath = stdr_str_drop_end(file, 2);
  str_t bindir;
  assert(stdr_str_split_once(binpath, S("/"), &bindir, NULL));
  str_t dbindir =
      stdr_str_concat_all(&arena, S("-DDAYDIR=\""), bindir, S("/\""));

  stdr_cmd_t cmd = {0};
  stdr_cmd_append_cc(&cmd);
  stdr_cmd_append_all(&cmd, file, dbindir, S("-o"), binpath, INC);
  stdr_cmd_append_all(&cmd, CFLAGS);
  stdr_cmd_append_all(&cmd, WFLAGS);
  stdr_cmd_append_all(&cmd, FFLAGS);
  stdr_cmd_run_reset(&cmd);

  stdr_cmd_append_all(&cmd, binpath);
  stdr_cmd_run_reset(&cmd);
  stdr_cmd_delete(&cmd);
}

void build_run_test(void) {
  stdr_cmd_t cmd = {0};
  stdr_cmd_append_cc(&cmd);
  stdr_cmd_append_all(&cmd, S("tests/test.c"), S("-o"), S("test"), INC,
                      S("-lcriterion"));
  stdr_cmd_run_reset(&cmd);

  stdr_cmd_append_all(&cmd, S("./test"), S("--always-succeed"), S("--verbose"));
  stdr_cmd_run_reset(&cmd);
  stdr_cmd_delete(&cmd);
}

int main(int argc, char** argv) {
  STDR_BUILD_SELF(argc, argv)

  build_run_test();
  build_advent(S("day_00/solution.c"));
  build_advent(S("day_01/solution.c"));
  build_advent(S("day_02/solution.c"));
  build_advent(S("day_03/solution.c"));
  build_advent(S("day_04/solution.c"));
  build_advent(S("day_05/solution.c"));
}
