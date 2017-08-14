#ifndef __NEUMANN_MARCHER_3D_HPP__
#define __NEUMANN_MARCHER_3D_HPP__

#include "marcher_3d.hpp"

template <class Node>
struct neumann_marcher_3d: public marcher_3d<Node> {
  using marcher_3d<Node>::marcher_3d;
protected:
  virtual void get_valid_neighbors(int i, int j, int k, abstract_node ** nb);
  static int di[6];
  static int dj[6];
  static int dk[6];
private:
  virtual void stage_neighbors_impl(abstract_node * n);
};

#include "neumann_marcher_3d.impl.hpp"

#endif // __NEUMANN_MARCHER_3D_HPP__

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: