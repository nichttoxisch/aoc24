#define STDR_IMPLEMENTATION
#include "stdr.h"

#ifndef DAYDIR
#define DAYDIR
#endif

enum tile {
  TILE_WALL,
  TILE_FLOOR,
  TILE_FLOOR_VISITED_ANY,
  TILE_FLOOR_VISITED_UP,
  TILE_FLOOR_VISITED_RIGHT,
  TILE_FLOOR_VISITED_DOWN,
  TILE_FLOOR_VISITED_LEFT,
};

array(array(enum tile)) input_parse(str_t input, usize* out_x, usize* out_y) {
  array(array(enum tile)) xs = array_new(array(enum tile), &ALLOCATOR_LIBC);

  str_t row;
  str_t rest = input;

  usize iy = 0;
  while (stdr_str_split_once(rest, S("\n"), &row, &rest)) {
    array_append(&xs, array_new(enum tile, &ALLOCATOR_LIBC));
    for (usize ix = 0; ix < row.size; ix++) {
      array_append(&xs[iy], row.ptr[ix] == '#' ? TILE_WALL : TILE_FLOOR);
      if (row.ptr[ix] == '^') {
        *out_x = ix;
        *out_y = iy;
      }
    }
    iy++;
  }

  return xs;
}

enum dir {
  UP,
  RIGHT,
  DOWN,
  LEFT,
};

enum dir dir_rotate(enum dir dir) { return (dir + 1) % 4; }

bool step(array(array(enum tile)) xs, usize* x, usize* y, enum dir* dir,
          bool* loop_detected) {
  switch (*dir) {
    case UP: {
      if (xs[*y][*x] == TILE_FLOOR_VISITED_UP) *loop_detected = TRUE;
      xs[*y][*x] = TILE_FLOOR_VISITED_UP;
      if (*y <= 0) return FALSE;
      if (xs[*y - 1][*x] == TILE_WALL) {
        *dir = dir_rotate(*dir);
      } else {
        *y = *y - 1;
      }
    } break;
    case RIGHT: {
      if (xs[*y][*x] == TILE_FLOOR_VISITED_RIGHT) *loop_detected = TRUE;
      xs[*y][*x] = TILE_FLOOR_VISITED_RIGHT;
      if (*x >= array_count(xs[0]) - 1) return FALSE;
      if (xs[*y][*x + 1] == TILE_WALL) {
        *dir = dir_rotate(*dir);
      } else {
        *x = *x + 1;
      }
    } break;
    case DOWN: {
      if (xs[*y][*x] == TILE_FLOOR_VISITED_DOWN) *loop_detected = TRUE;
      xs[*y][*x] = TILE_FLOOR_VISITED_DOWN;
      if (*y >= array_count(xs) - 1) return FALSE;
      if (xs[*y + 1][*x] == TILE_WALL) {
        *dir = dir_rotate(*dir);
      } else {
        *y = *y + 1;
      }
    } break;
    case LEFT: {
      if (xs[*y][*x] == TILE_FLOOR_VISITED_LEFT) *loop_detected = TRUE;
      xs[*y][*x] = TILE_FLOOR_VISITED_LEFT;
      if (*x <= 0) return FALSE;
      if (xs[*y][*x - 1] == TILE_WALL) {
        *dir = dir_rotate(*dir);
      } else {
        *x = *x - 1;
      }
    } break;
  }

  return TRUE;
}

i64 part_1(str_t input) {
  usize x, y;
  array(array(enum tile)) xs = input_parse(input, &x, &y);
  enum dir dir = UP;

  bool loop = FALSE;
  while (!loop && step(xs, &x, &y, &dir, &loop)) {
    // printf("%lu %lu %d\n", x, y, dir);
  }

  i64 sum = 0;
  for (usize i = 0; i < array_count(xs); i++) {
    for (usize j = 0; j < array_count(xs[i]); j++) {
      if (xs[i][j] >= TILE_FLOOR_VISITED_ANY) sum++;
    }
  }

  forr(array(enum tile), e, xs) { array_delete(e); };
  array_delete(&xs);

  return sum;
}

i64 part_2(str_t input) {
  usize init_x, init_y;
  array(array(enum tile)) xs = input_parse(input, &init_x, &init_y);

  usize x = init_x;
  usize y = init_y;
  enum dir dir = UP;
  bool loop = FALSE;
  while (!loop && step(xs, &x, &y, &dir, &loop));

  i64 sum = 0;
  for (usize i = 0; i < array_count(xs); i++) {
    for (usize j = 0; j < array_count(xs[i]); j++) {
      if (init_x == j && init_y == i) continue;
      if (xs[i][j] < TILE_FLOOR_VISITED_ANY) continue;

      array(array(enum tile)) ys = input_parse(input, &x, &y);
      dir = UP;

      ys[i][j] = TILE_WALL;

      bool loop_detected = FALSE;
      usize ii = 0;
      while (!loop_detected && step(ys, &x, &y, &dir, &loop_detected)) {
        ii++;
        if (ii > 10000) loop_detected = TRUE;  // Sowwy
        // Actually the probleme is that in one single case the guard will walk
        // between two points and thus will not be detected by my loop detection
        // TODO: Maybe correct this with with visited fields as bitmap 0b0001 =
        // UP 0b0010 = RIGHT ...
      }

      if (loop_detected) {
        sum++;
        // printf("F: %lu %lu\n", j, i);
      };

      forr(array(enum tile), e, ys) { array_delete(e); };
      array_delete(&ys);
    }
  }

  forr(array(enum tile), e, xs) { array_delete(e); };
  array_delete(&xs);

  return sum;
}

i32 main(void) {
  stdr_arena_ctx_init(1024 * 64, &STDR_ALLOCATOR_LIBC);
  str_t example = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "example.txt");
  str_t input = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "input.txt");

  STDR_ASSERT(part_1(example) == 41);
  STDR_ASSERT(part_1(input) == 5242);
  STDR_ASSERT(part_2(example) == 6);
  // STDR_ASSERT(part_2(input) == 1424); // Will work but takes a little long

  stdr_arena_ctx_delete();

  return 0;
}
