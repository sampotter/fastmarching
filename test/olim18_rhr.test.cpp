#include "common.defs.hpp"
#include "olim.hpp"
#include "olim3d.hpp"
#include "olim.test.common.hpp"

#define MARCHER olim18_rhr

using olim_t = olim8_rhr;
using olim3d_t = MARCHER;

TEST (MARCHER, quadrants_are_correct) {
  ASSERT_TRUE(quadrants_are_correct<olim3d_t>(sqrt2));
}

TEST (MARCHER, octants_are_correct) {
  ASSERT_TRUE(octants_are_correct<olim3d_t>(sqrt2, sqrt2 + 1.0/sqrt3));
}

TEST (MARCHER, planes_are_correct) {
  auto res = planes_are_correct<olim_t, olim3d_t>(speed_funcs[0], speed_funcs_3d[0]);
  ASSERT_TRUE(res);

  res = planes_are_correct<olim_t, olim3d_t>(speed_funcs[1], speed_funcs_3d[1], 9);
  ASSERT_TRUE(res);
}

TEST (MARCHER, result_is_symmetric) {
  ASSERT_TRUE(result_is_symmetric<olim3d_t>(speed_funcs_3d[0]));
  ASSERT_TRUE(result_is_symmetric<olim3d_t>(speed_funcs_3d[1]));
}

TEST (MARCHER, two_by_two_by_three_cells_are_correct) {
  ASSERT_TRUE(two_by_two_by_three_cells_are_correct<olim3d_t>());
}

TEST (MARCHER, plane_boundaries_are_correct) {
  ASSERT_TRUE(plane_boundaries_are_correct<olim3d_t>());
}

TEST (MARCHER, solution_is_exact_in_factored_region) {
  ASSERT_TRUE(solution_is_exact_in_factored_square<olim_t>(3));
}
