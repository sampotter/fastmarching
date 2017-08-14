#ifndef __OLIM8_MP0C_HPP__
#define __OLIM8_MP0C_HPP__

#include "olim8.hpp"

struct olim8_mp0c_update_rules {
  double adj1pt(double u0, double s, double s0, double h) const;
  double adj2pt(double u0, double u1, double s, double s0, double s1, double h) const;
  double diag1pt(double u0, double s, double s0, double h) const;
  double diag2pt(double u0, double u1, double s, double s0, double s1, double h) const;
};

using olim8_mp0c = olim8<olim8_mp0c_update_rules>;

#endif // __OLIM8_MP0C_HPP__

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: