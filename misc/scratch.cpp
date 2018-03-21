#include <olim.hpp>
#include <olim3d.hpp>
#include <src/config.hpp>

int main() {
  int n = 17;
  olim26_mp1 m {n, n, n, 2./(n-1), (speed_func_3d) s4, 1., 1., 1.};
  m.add_boundary_node(n/2, n/2, n/2);
  m.run();
#if COLLECT_STATS
  m.dump_stats();
#endif

  // int n = 2001;
  // olim8_rhr m {n, n, 2./(n-1), (speed_func) default_speed_func, 1., 1.};
  // m.add_boundary_node(n/2, n/2);
  // m.run();
}
