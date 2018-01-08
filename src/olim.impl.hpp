#ifndef __OLIM_IMPL_HPP__
#define __OLIM_IMPL_HPP__

#include <src/config.hpp>

#if PRINT_UPDATES
#    include <cstdio>
#endif

#include "common.macros.hpp"
#include "update_rules.utils.hpp"

#define P01 1
#define P10 2
#define P11 3

#define LINE(i, d) do {                                             \
    if (nb[i]) {                                                    \
      T = min(T, this->template line<d>(VAL(i), SPEED_ARGS(i), h)); \
    }                                                               \
  } while (0)

#define TRI(i, j, p0, p1) do {                  \
    if (nb[i] && nb[j]) {                       \
      T = min(                                  \
        T,                                      \
        this->tri(                              \
          VAL(i),                               \
          VAL(j),                               \
          SPEED_ARGS(i, j),                     \
          h,                                    \
          ffvec<P ## p0> {},                    \
          ffvec<P ## p1> {}));                  \
    }                                           \
  } while (0)

template <class node, class line_updates, class tri_updates, bool adj_updates,
          bool diag_updates>
void olim<node, line_updates, tri_updates, adj_updates,
          diag_updates>::update_impl(int i, int j, double & T)
{
  using std::min;

#ifdef PRINT_UPDATES
  printf("olim::update_impl(i = %d, j = %d)\n", i, j);
#endif

  abstract_node * nb[num_neighbors];
  memset(nb, 0x0, num_neighbors*sizeof(abstract_node *));
  this->get_valid_neighbors(i, j, nb);

  double h = this->get_h(), s = this->speed(i, j), s_[num_neighbors];
  for (int k = 0; k < num_neighbors; ++k) {
    if (nb[k]) {
      s_[k] = this->speed(
        i + neighborhood_t::di[k],
        j + neighborhood_t::dj[k]);
    }
  }

  for (int i = 0, j = 1; i < 4; j = (++i + 1) % 4) {
    LINE(i, 1);
    TRI(i, j, 01, 10);
  }
  if (diag_updates) {
    for (int i = 4, j = 0, k = 1; i < 8; ++i, k = (++j + 1) % 4) {
      LINE(i, 2);
      TRI(i, j, 11, 01);
      TRI(i, k, 11, 10);
    }
  }

#ifdef PRINT_UPDATES
  printf("olim::update_impl: T <- %g\n", T);
#endif
}

#undef P01
#undef P10
#undef P11
#undef LINE
#undef TRI

#endif // __OLIM_IMPL_HPP__
