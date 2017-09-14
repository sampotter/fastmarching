#ifndef __OLIM26_IMPL_HPP__
#define __OLIM26_IMPL_HPP__

#include <algorithm>
#include <cstdio>

#include "common.macros.hpp"
#include "olim26.defs.hpp"

// neighbor order:
// degree 1: U, N, E, S, W, D
// degree 2: UN, UE, US, UW, NE, SE, SW, NW, DN, DE, DS, DW
// degree 3: UNE, USE, USW, UNW, DNE, DSE, DSW, DNW

template <class node, class update_rules>
int olim26<node, update_rules>::di[] = {
  0, 1, 0, -1, 0, 0,
  1, 0, -1, 0, 1, -1, -1, 1, 1, 0, -1, 0,
  1, -1, -1, 1, 1, -1, -1, 1
};

template <class node, class update_rules>
int olim26<node, update_rules>::dj[] = {
  0, 0, 1, 0, -1, 0,
  0, 1, 0, -1, 1, 1, -1, -1, 0, 1, 0, -1,
  1, 1, -1, -1, 1, 1, -1, -1
};

template <class node, class update_rules>
int olim26<node, update_rules>::dk[] = {
  1, 0, 0, 0, 0, -1,
  1, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1,
  1, 1, 1, 1, -1, -1, -1, -1
};

template <class node, class update_rules>
void olim26<node, update_rules>::get_valid_neighbors(int i, int j, int k,
                                                abstract_node ** nb) {
  int a, b, c;
  for (int l = 0; l < 26; ++l) {
    a = i + di[l], b = j + dj[l], c = k + dk[l];
    if (this->is_valid(a, b, c)) {
      nb[l] = &this->operator()(a, b, c);
    }
  }
}

template <class node, class update_rules>
void olim26<node, update_rules>::stage_neighbors_impl(abstract_node * n) {
  int i = static_cast<node *>(n)->get_i();
  int j = static_cast<node *>(n)->get_j();
  int k = static_cast<node *>(n)->get_k();

  for (int l = 0; l < 26; ++l) {
    this->stage(i + di[l], j + dj[l], k + dk[l]);
  }

  int a, b, c;
  for (int l = 0; l < 26; ++l) {
    a = i + di[l], b = j + dj[l], c = k + dk[l];
    if (this->in_bounds(a, b, c) && !this->operator()(a, b, c).is_valid()) {
      this->update(a, b, c);
    }
  }
}

namespace olim26_defs {
  int line1tris[6][8] = {
    {UN, UNE, NE, DNE, DN, DNW, NW, UNW}, // N
    {UE, USE, SE, DSE, DE, DNE, NE, UNE}, // E
    {UN, UNE, UE, USE, US, USW, UW, UNW}, // U
    {US, USE, SE, DSE, DS, DSW, SW, USW}, // S
    {UW, USW, SW, DSW, DW, DNW, NW, UNW}, // W
    {DN, DNE, DE, DSE, DS, DSW, DW, DNW}, // D
  };

  int line2tris[12][4] = {
    {U, N, UNE, UNW}, // UN
    {U, E, UNE, USE}, // UE
    {U, S, USE, USW}, // US
    {U, W, UNW, USW}, // UW
    {N, E, UNE, DNE}, // NE
    {S, E, USE, DSE}, // SE
    {S, W, USW, DSW}, // SW
    {N, W, UNW, DNW}, // NW
    {D, N, DNE, DNW}, // DN
    {D, E, DNE, DSE}, // DE
    {D, S, DSE, DSW}, // DS
    {D, W, DNW, DSW}, // DW
  };

  int line3tris[8][6] = {
    {U,  N,  E, UN, UE, NE}, // UNE
    {U,  S,  E, US, UE, SE}, // USE
    {U,  S,  W, US, UW, SW}, // USW
    {U,  N,  W, UN, UW, NW}, // UNW
    {D,  N,  E, DN, DE, NE}, // DNE
    {D,  S,  E, DS, DE, SE}, // DSE
    {D,  S,  W, DS, DW, SW}, // DSW
    {D,  N,  W, DN, DW, NW}, // DNW
  };
}

template <class node, class update_rules>
void olim26<node, update_rules>::update_impl(int i, int j, int k, double & T) {
  using namespace olim26_defs;
  using std::min;

  abstract_node * nb[26];
  memset(nb, 0x0, 26*sizeof(abstract_node *));
  get_valid_neighbors(i, j, k, nb);

  double h = this->get_h(), s = this->speed(i, j, k), Tnew;
  int l, l0, l1, l2, a, b, * is;

  double s_[26];
  for (l = 0; l < 26; ++l) {
    if (nb[l]) {
      s_[l] = this->speed(i + di[l], j + dj[l], k + dk[l]);
    }
  }

  /**
   * Degree 1 line updates
   */
  for (l = 0; l < 6; ++l) {
    is = olim26_defs::line1tris[l];
    if (!nb[l])
      continue;
    if (is[0] || is[1] || is[2] || is[3] || is[4] || is[5] || is[6] || is[7])
      continue;
    T = min(T, this->line1(VAL(l), s, s_[l], h));
  }

  /**
   * Degree 2 line updates
   */
  for (; l < 18; ++l) {
    is = olim26_defs::line2tris[l - 6];
    if (!nb[l])
      continue;
    if (is[0] || is[1] || is[2] || is[3])
      continue;
    T = min(T, this->line2(VAL(l), s, s_[l], h));
  }

  /**
   * Degree 3 line updates
   */
  for (; l < 26; ++l) {
    is = olim26_defs::line3tris[l - 18];
    if (!nb[l])
      continue;
    if (is[0] || is[1] || is[2] || is[3] || is[4] || is[5])
      continue;
    T = min(T, this->line3(VAL(l), s, s_[l], h));
  }

  /**
   * Degree (1, 2) and (1, 3) triangle updates
   */
  for (l0 = 0; l0 < 6; ++l0) {
    is = olim26_defs::line1tris[l0];
    if (nb[l0]) {
      for (a = 0, l1 = *is; a < 8; l1 = is[++a]) {
        if (nb[l1]) {
          if (a % 2 == 0) {
            T = min(T, this->tri12(VAL(l0), VAL(l1), s, s_[l0], s_[l1], h));
          } else {
            T = min(T, this->tri13(VAL(l1), VAL(l0), s, s_[l1], s_[l0], h));
          }
        }
      }
    }
  }

  /**
   * Degree (2, 3) triangle updates
   */
  for (a = 0, l0 = 18; a < 8; ++a, ++l0) {
    is = olim26_defs::line3tris[a];
    if (nb[l0]) {
      for (b = 3, l1 = is[b]; b < 6; l1 = is[++b]) {
        if (nb[l1]) {
          T = min(T, this->tri23(VAL(l0), VAL(l1), s, s_[l0], s_[l1], h));
        }
      }
    }
  }

  /**
   * Degree (1, 2, 3) tetrahedron updates
   */
  for (a = 0, l0 = 18; a < 8; ++a, ++l0) {
    is = olim26_defs::line3tris[a];
    if (nb[l0]) {
      for (b = 0, l1 = *is, l2 = is[(b + 1) % 6]; b < 6;
           l1 = is[++b], l2 = is[(b + 1) % 6]) {
        if (nb[l1] && nb[l2]) {
          if (b % 2 == 0) {
            Tnew = this->tetra123(
              VAL(l1), VAL(l2), VAL(l0), s, s_[l1], s_[l2], s_[l0], h);
            if (ISINF(Tnew)) {
              T = min(T, this->tri12(VAL(l1), VAL(l2), s, s_[l1], s_[l2], h));
              T = min(T, this->tri13(VAL(l1), VAL(l0), s, s_[l1], s_[l0], h));
              T = min(T, this->tri23(VAL(l2), VAL(l0), s, s_[l2], s_[l0], h));
            } else {
              T = min(T, Tnew);
            }
          } else {
            Tnew = this->tetra123(
              VAL(l2), VAL(l1), VAL(l0), s, s_[l2], s_[l1], s_[l0], h);
            if (ISINF(Tnew)) {
              T = min(T, this->tri12(VAL(l2), VAL(l1), s, s_[l2], s_[l1], h));
              T = min(T, this->tri13(VAL(l2), VAL(l0), s, s_[l2], s_[l0], h));
              T = min(T, this->tri23(VAL(l1), VAL(l0), s, s_[l1], s_[l0], h));
            } else {
              T = min(T, Tnew);
            }
          }
        }
      }
    }
  }
}

#endif // __OLIM26_IMPL_HPP__

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: