#include "common.hpp"

#include "eikonal/olim.hpp"
#include "eikonal/olim3d.hpp"

using namespace eikonal;

using olim_t = olim8_rhr<>;
using olim3d_t = olim3d_rhr<>;

TEST (olim3d_rhr, quadrants_are_correct) {
  ASSERT_TRUE(quadrants_are_correct<olim3d_t>(sqrt(2)));
}

TEST (olim3d_rhr, octants_are_correct) {
  ASSERT_TRUE(octants_are_correct<olim3d_t>(sqrt(2), sqrt(3)));
}

TEST (olim3d_rhr, two_by_two_by_three_cells_are_correct) {
  ASSERT_TRUE(two_by_two_by_three_cells_are_correct<olim3d_t>());
}

TEST (olim3d_rhr, plane_boundaries_are_correct) {
  ASSERT_TRUE(plane_boundaries_are_correct<olim3d_t>());
}

TEST (olim3d_rhr, solution_is_exact_in_factored_region) {
  ASSERT_TRUE(solution_is_exact_in_factored_square<olim3d_t>(5));
}
