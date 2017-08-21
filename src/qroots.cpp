#include "qroots.hpp"

#include <cassert>
#include <cmath>
#include <limits>

static double polyval(double * coefs, int ncoefs, double x) {
  double y = 0;
  for (int i = ncoefs - 1; i > 0; --i) {
    y += coefs[i];
    y *= x;
  }
  return y + coefs[0];
}

static char nbits[16] = {
  0, // 0000
  1, // 0001
  1, // 0010
  2, // 0011
  1, // 0100
  2, // 0101
  2, // 0110
  3, // 0111
  1, // 1000
  2, // 1001
  2, // 1010
  3, // 1011
  2, // 1100
  3, // 1101
  3, // 1110
  4  // 1111
};

int sigma(double ** polys, double x) {
  double * a = polys[0];
  double y = a[0] + x*(a[1] + x*(a[2] + x*(a[3] + x*a[4])));
  char signs = y > 0 ? 127 : 0;

  a = polys[1];
  y = a[0] + x*(a[1] + x*(a[2] + x*a[3]));
  signs = (signs << (y != 0)) | (y > 0);

  a = polys[2];
  y = a[0] + x*(a[1] + x*a[2]);
  signs = (signs << (y != 0)) | (y > 0);

  a = polys[3];
  y = a[0] + x*a[1];
  signs = (signs << (y != 0)) | (y > 0);

  a = polys[4];
  y = a[0];
  signs = (signs << (y != 0)) | (y > 0);

  // TODO: compare with modding and adding---that may actually be
  // faster
  return nbits[((signs ^ (signs << 1)) >> 1) & 15];
}

int oldsigma(double ** polys, double x) {
  int nsigns = 0, signs[5], changes = 0;

  double y = polys[0][0] +
    x*(polys[0][1] + x*(polys[0][2] + x*(polys[0][3] + x*polys[0][4])));
  if (y > 0) signs[nsigns++] = 1;
  if (y < 0) signs[nsigns++] = -1;

  y = polys[1][0] + x*(polys[1][1] + x*(polys[1][2] + x*polys[1][3]));
  if (y > 0) signs[nsigns++] = 1;
  if (y < 0) signs[nsigns++] = -1;

  y = polys[2][0] + x*(polys[2][1] + x*polys[2][2]);
  if (y > 0) signs[nsigns++] = 1;
  if (y < 0) signs[nsigns++] = -1;

  y = polys[3][0] + x*polys[3][1];
  if (y > 0) signs[nsigns++] = 1;
  if (y < 0) signs[nsigns++] = -1;

  y = polys[4][0];
  if (y > 0) signs[nsigns++] = 1;
  if (y < 0) signs[nsigns++] = -1;

  for (int i = 1, j = 0; i < nsigns; ++i, ++j) {
    if (signs[i] != signs[j]) {
      ++changes;
    }
  }
  return changes;
}

int sturm(double ** polys, double l, double r) {
  // Sturm's theorem finds roots on (l, r]; the case p(l) = 0 *does*
  // happen and *isn't* handled by our fast sigma---so, perturb l to
  // the right a very small amount to get around this
  double l_plus_eps = l + std::numeric_limits<decltype(l)>::epsilon();
  assert(sigma(polys, l_plus_eps) == oldsigma(polys, l_plus_eps));
  assert(sigma(polys, r) == oldsigma(polys, r));
  return sigma(polys, l_plus_eps) - sigma(polys, r);
}

// TODO: remove use of polyval

static double secant(double * const a, double x0, double x1, double l, double r,
                     bool & found, double tol = 1e-13) {
  double x, f0 = polyval(a, 5, x0), f1 = polyval(a, 5, x1);
  do {
    x = (x1*f0 - x0*f1)/(f0 - f1);
    if (x < l || r < x) {
      found = false;
      return x;
    }
    x1 = x0;
    x0 = x;
    f1 = f0;
  } while (fabs(f0 = polyval(a, 5, x)) > tol);
  found = true;
  return x;
}

static void findroot(double * a, double l, double r, double h, double * x0) {
  bool found = false;
  *x0 = secant(a, l, l + (r - l)*h, l, r, found);
  if (!found) *x0 = secant(a, r, r + (l - r)*h, l, r, found);
  assert(found);
}

struct interval {
  interval() {}
  interval(int nroots, double l, double r): nroots {nroots}, l {l}, r {r} {}
  int nroots {-1};
  double l {-1}, r {-1};
};

void qroots(double * a, double * roots, double l, double r) {
  // Check to see if we're actually dealing with a polynomial of lower
  // degree so we can invoke an appropriate direct solver.

  int degree = 4;
  while (a[degree] < 1e-13) --degree;

  int root = 0;

  assert(degree > 1);

  if (degree == 2) {
    double disc = a[1]*a[1] - 4*a[2]*a[0];
    if (disc == 0) {
      roots[root++] = (-a[1] + std::sqrt(disc))/(2*a[2]);
    } else {
      roots[root++] = (-a[1] + std::sqrt(disc))/(2*a[2]);
      roots[root++] = (-a[1] - std::sqrt(disc))/(2*a[2]);
    }
    return;
  }

  assert(degree != 3);

  // Compute Sturm sequence for applying Sturm's theorem.
  // TODO: are there other, simpler Sturm sequences that we can use instead?
  // TODO: simplify arithmetic (low priority)

  double b[4] = {a[1], 2*a[2], 3*a[3], 4*a[4]}; // 1st der. of a

  double c[3] = { // -rem(a, b)
    -a[0] + a[1]*a[3]/(16*a[4]),
    (-6*a[1] + a[2]*a[3]/a[4])/8,
    -a[2]/2 + 3*a[3]*a[3]/(16*a[4])
  };

  double d[2] = { // -rem(b, c)
    -b[0] + c[0]*(-b[3]*c[1] + b[2]*c[2])/(c[2]*c[2]),
    (-b[3]*(c[1]*c[1] + c[0]*c[2]) + (b[2]*c[1] - b[1]*c[2])*c[2])/(c[2]*c[2])
  };

  double e[1] = { // -rem(c, d)
    -c[0] + d[0]*(-c[2]*d[0] + c[1]*d[1])/(d[1]*d[1])
  };

  double * polys[5] = {a, b, c, d, e};
  
  if (fabs(a[0]) < 1e-14) {
    roots[root++] = 0;
  }

  double mid;
  int nroots = sturm(polys, l, r), i = 0;
  interval ivals[4];

  if (nroots == 1) {
    findroot(polys[0], l, r, 0.1, &roots[root++]);
    return;
  } else if (nroots > 1) {
    ivals[i++] = {nroots, l, r};
  }

  while (i > 0) {
    interval ival = ivals[--i];
    if (ival.nroots == 1) {
      findroot(polys[0], ival.l, ival.r, 0.1, &roots[root++]);
    } else if (ival.nroots > 1) {
      mid = (l + r)/2;
      int sigl = sigma(polys, l), sigm = sigma(polys, mid),
        sigr = sigma(polys, r);
      if (sigl > sigm) {
        assert(0 <= i && i < 4);
        ivals[i++] = {sigl - sigm, l, mid};
      }
      if (sigm > sigr) {
        assert(0 <= i && i < 4);
        ivals[i++] = {sigm - sigr, mid, r};
      }
    }
  }
}

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End:
