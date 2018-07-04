#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "abstract_node.hpp"

#include <cmath>

struct node: public abstract_node {
  node() {}
  node(int i, int j, double value = 0):
    abstract_node {value, state::valid}, _i {i}, _j {j} {}
  inline int get_i() const { return _i; }
  inline void set_i(int i) { _i = i; }
  inline int get_j() const { return _j; }
  inline void set_j(int j) { _j = j; }
  inline virtual double get_T() const {
    assert(_parent);
    auto * n = static_cast<node *>(_parent);
    return n->get_sh_fac()*std::sqrt(
      std::pow(_i - n->get_i(), 2) +
      std::pow(_j - n->get_j(), 2));
  }
EIKONAL_PRIVATE:
  int _i {-1}, _j {-1};
};

#endif // __NODE_HPP__
