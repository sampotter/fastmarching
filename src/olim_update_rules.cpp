#include <src/config.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#if PRINT_UPDATES
#    include <cstdio>
#endif

#include "common.defs.hpp"
#include "common.macros.hpp"
#include "olim_update_rules.hpp"
#include "olim_util.hpp"

double olim_rect_update_rules::line1(double u0, double s, double h)
  const
{
#if PRINT_UPDATES
  double tmp = u0 + s*h;
  printf("line1(u0 = %g, s = %g, h = %g) -> %g\n", u0, s, h, tmp);
  return tmp;
#else
  return u0 + s*h;
#endif
}

double olim_rect_update_rules::line2(double u0, double s, double h)
  const
{
#if PRINT_UPDATES
  double tmp = u0 + s*h*sqrt2;
  printf("line2(u0 = %g, s = %g, h = %g) -> %g\n", u0, s, h, tmp);
  return tmp;
#else
  return u0 + s*h*sqrt2;
#endif
}

double olim_rect_update_rules::line3(double u0, double s, double h)
  const
{
#if PRINT_UPDATES
  double tmp = u0 + s*h*sqrt3;
  printf("line3(u0 = %g, s = %g, h = %g) -> %g\n", u0, s, h, tmp);
  return tmp;
#else
  return u0 + s*h*sqrt3;
#endif
}

// TODO: make this solve the unconstrained problem
double olim_rect_update_rules::tri11(
  double u0, double u1, double s, double h) const
{
#ifdef EIKONAL_DEBUG
  check_params(u0, u1, h, s);
#endif
#if PRINT_UPDATES
  double tmp = rhr_adj(u0, u1, s, h);
  printf("tri11(u0 = %g, u1 = %g, s = %g, h = %g) -> %g\n", u0, u1, s, h, tmp);
  return tmp;
#else
  return rhr_adj(u0, u1, s, h);
#endif
}

// TODO: make this solve the unconstrained problem
double olim_rect_update_rules::tri12(double u0, double u1, double s, double h)
  const
{
#ifdef EIKONAL_DEBUG
  check_params(u0, u1, h, s);
#endif
#if PRINT_UPDATES
  double tmp = rhr_diag(u0, u1, s, h);
  printf("tri12(u0 = %g, u1 = %g, s = %g, h = %g) -> %g\n", u0, u1, s, h, tmp);
  return tmp;
#else
  return rhr_diag(u0, u1, s, h);
#endif
}

double olim_rect_update_rules::tri13(double u0, double u1, double s, double h)
  const
{
#ifdef EIKONAL_DEBUG
  check_params(u0, u1, h, s);
#endif
  double sh = s*h, du = u1 - u0, alpha = fabs(du)/sh, alpha_sq = alpha*alpha;
  double T = std::numeric_limits<double>::infinity();
  if (alpha_sq > 2) {
    return T;
  }
  double lam = alpha/sqrt(2*(2 - alpha_sq)), l = sqrt(1 + 2*lam*lam);
  if (0 <= lam && lam <= 1 && fabs(du*l + sh*lam) < 1e-13) {
    T = (1 - lam)*u0 + lam*u1 + sh*l;
  } else if (0 <= -lam && -lam <= 1 && fabs(du*l - sh*lam) < 1e-13) {
    T = (1 + lam)*u0 - lam*u1 + sh*l;
  }
#if PRINT_UPDATES
  printf("tri13(u0 = %g, u1 = %g, s = %g, h = %g) -> %g\n", u0, u1, s, h, T);
#endif
  return T;
}

double olim_rect_update_rules::tri22(double u0, double u1, double s, double h)
  const
{
#ifdef EIKONAL_DEBUG
  check_params(u0, u1, h, s);
#endif
  double sh = s*h, du = u1 - u0, alpha = fabs(du)/sh, alpha_sq = alpha*alpha;
  double T = std::numeric_limits<double>::infinity();
  if (alpha_sq > 2) {
    return T;
  }
  double rhs = sqrt3*alpha/(2*sqrt(4 - alpha_sq));
  double lam = 0.5 - rhs, l = sqrt(2*(1 - lam*(1 - lam)));
  if (0 <= lam && lam <= 1 && fabs(du*l + sh*lam) < 1e-13) {
    T = (1 - lam)*u0 + lam*u1 + sh*l;
  } else {
    lam = 0.5 + rhs, l = sqrt(2*(1 - lam*(1 - lam)));
    if (0 <= lam && lam <= 1 && fabs(du*l * sh*lam) < 1e-13) {
      T = (1 - lam)*u0 + lam*u1 + sh*l;
    }
  }
#if PRINT_UPDATES
  printf("tri22(u0 = %g, u1 = %g, s = %g, h = %g) -> %g\n", u0, u1, s, h, T);
#endif
  return T;
}

double olim_rect_update_rules::tri23(double u0, double u1, double s, double h)
  const
{
#ifdef EIKONAL_DEBUG
  check_params(u0, u1, h, s);
#endif
  double sh = s*h, du = u1 - u0, alpha = fabs(du)/sh, alpha_sq = alpha*alpha;
  double T = std::numeric_limits<double>::infinity();
  if (alpha_sq > 1) {
    return T;
  }
  double lam = sqrt2*alpha/sqrt(1 - alpha_sq), l = sqrt(2 + lam*lam);
  if (0 <= lam && lam <= 1 && fabs(du*l + sh*lam) < 1e-13) {
    T = (1 - lam)*u0 + lam*u1 + sh*l;
  } else if (0 <= -lam && -lam <= 1 && fabs(du*l - sh*lam) < 1e-13) {
    T = (1 + lam)*u0 - lam*u1 + sh*l;
  }
#if PRINT_UPDATES
  printf("tri23(u0 = %g, u1 = %g, s = %g, h = %g) -> %g\n", u0, u1, s, h, T);
#endif
  return T;
}

/**
 * Compute the exact minimizing step size for the relaxed Newton's
 * method used to solve the minimization problem needed to do the
 * tetrahedral updates.
 */
template <int M11, int M12, int M22, int e1, int e2>
static
double compute_step_size(
  double p1, double p2, double du1, double du2,
  double lam1, double lam2, double sh, double Q, bool & failure)
{
  failure = false;
  if (fabs(p1) <= EPS(double) && fabs(p2) <= EPS(double)) {
    return 0;
  }
  double du_dot_p = du1*p1 + du2*p2;
  double delta = std::pow(2*du_dot_p/sh, 2);
  double pMp = 2*(p1*(M11*p1 + M12*p2) + p2*(M12*p1 + M22*p2));
  double lMp = 2*(p1*(M11*lam1 + M12*lam2) + p2*(M12*lam1 + M22*lam2));
  double edotp = 2*(e1*p1 + e2*p2);
  double tmp1 = 2*pMp - delta, tmp2 = lMp + edotp;
  double a = pMp*tmp1/2;
  double b = tmp1*tmp2;
  double c = tmp2*tmp2 - delta*Q;
  double disc = b*b - 4*a*c;
  if (disc < 0) {
    failure = true;
    return INF(double);
  }
  double lhs = -b/(2*a);
  double rhs = sqrt(disc)/(2*a);
  assert(!(lhs + rhs <= 0) && !(lhs - rhs <= 0));
  return lhs + rhs <= 0 ? lhs + rhs : lhs - rhs;
}

double olim_rect_update_rules::tetra111(
  double u0, double u1, double u2, double s, double h) const
{
  using std::max;

#ifdef EIKONAL_DEBUG
  check_params(u0, u1, u2, h, s);
#endif

  double sh = s*h, du1 = u1 - u0, du2 = u2 - u0, T = INF(double),
    lam1 = 1./3., lam2 = 1./3., p1, p2, relerr, alpha, Q, c,
    H11, H12, H22, G1, G2;

  int niters = 0;
  bool failure = false;
  do {
    Q = pow(1 - lam1 - lam2, 2) + lam1*lam1 + lam2*lam2;
    c = sqrt(Q)/sh;
    H11 = c*(1 + lam1*(3*lam1 - 2));
    H12 = c*(lam1*(3*lam2 - 1) - lam2);
    H22 = c*(1 + lam2*(3*lam2 - 2));
    G1 = du1 + (2*lam1 + lam2 - 1)/c;
    G2 = du2 + (lam1 + 2*lam2 - 1)/c;
    p1 = H11*G1 + H12*G2;
    p2 = H12*G1 + H22*G2;

    alpha = compute_step_size<2, 1, 2, -1, -1>(
      p1, p2, du1, du2, lam1, lam2, sh, Q, failure);
    if (failure) {
      goto coda;
    }
    if (fabs(alpha) <= EPS(double)) {
      break;
    }

    lam1 += alpha*p1 , lam2 += alpha*p2;
    if (lam1 < 0 || lam2 < 0 || 1 - lam1 - lam2 < 0) {
      goto coda;
    }
    relerr = max(fabs(p1), fabs(p2))/max(fabs(lam1), fabs(lam2));

    ++niters;
    if (niters > 10) {
#ifdef EIKONAL_DEBUG
      printf("u0 = %0.16g, u1 = %0.16g, u2 = %0.16g, s = %0.16g, h = %0.16g\n",
             u0, u1, u2, s, h);
      std::abort();
#else
      // printf("p1 = %g, p2 = %g, alpha = %g\n", p1, p2, alpha);
      break;
#endif
    }
  } while (relerr > 1e-15);

  Q = pow(1 - lam1 - lam2, 2) + lam1*lam1 + lam2*lam2;
  T = (1 - lam1 - lam2)*u0 + lam1*u1 + lam2*u2 + sh*sqrt(Q);

  coda:
#if PRINT_UPDATES
  printf("tetra111(u0 = %0.16g, u1 = %0.16g, u2 = %0.16g, s = %0.16g, "
         "h = %0.16g) -> %g\n", u0, u1, u2, s, h, T);
#endif
  return T;
}

double olim_rect_update_rules::tetra122(
  double u0, double u1, double u2, double s, double h) const
{
  using std::min;
  using std::sqrt;

#ifdef EIKONAL_DEBUG
  check_params(u0, u1, u2, s, h);
#endif

  double T = std::numeric_limits<double>::infinity();

  double sh = s*h, du1 = u1 - u0, du2 = u2 - u0;
  double alpha1 = fabs(du1/sh), alpha1_sq = alpha1*alpha1;
  double alpha2 = fabs(du2/sh), alpha2_sq = alpha2*alpha2;

  if (alpha1_sq + alpha2_sq >= 1) {
    return T;
  }

  double denom = sqrt(1 - alpha1_sq - alpha2_sq);
  double lam1 = alpha1/denom;
  double lam2 = alpha2/denom;

  assert(lam1 >= 0);
  assert(lam2 >= 0);

  double lam0 = 1 - lam1 - lam2;
  if (lam0 < 0) {
    return T;
  }

  double l = sqrt(1 + lam1*lam1 + lam2*lam2);
  if (fabs(du1*l + sh*lam1) < 1e-13 && fabs(du2*l + sh*lam2) < 1e-13) {
    T = min(T, (1 - lam1 - lam2)*u0 + lam1*u1 + lam2*u2 + sh*l);
  }

#if PRINT_UPDATES
  printf("tetra122(u0 = %g, u1 = %g, u2 = %g, s = %g, h = %g) -> %g\n",
         u0, u1, u2, s, h, T);
#endif

  return T;
}

double olim_rect_update_rules::tetra123(
  double u0, double u1, double u2, double s, double h) const
{
  using std::max;

#ifdef EIKONAL_DEBUG
  check_params(u0, u1, u2, s, h);
#endif

  double sh = s*h, du1 = u1 - u0, du2 = u2 - u0, T = INF(double),
    lam1 = 0, lam2 = 0, p1, p2, err, alpha, Q, c, H11, H12, H22, G1, G2;

  int niters = 0;
  bool failure = false;
  do {
    Q = lam1*lam1 + 2*lam1*lam2 + 2*lam2*lam2 + 1;
    c = sqrt(Q)/sh;
    H11 = c*(2 + lam1*lam1);
    H12 = c*(lam1*lam2 - 1);
    H22 = c*(1 + lam2*lam2);
    G1 = du1 + (lam1 + lam2)/c;
    G2 = du2 + (lam1 + 2*lam2)/c;
    p1 = H11*G1 + H12*G2;
    p2 = H12*G1 + H22*G2;

    alpha = compute_step_size<1, 1, 2, 0, 0>(
      p1, p2, du1, du2, lam1, lam2, sh, Q, failure);
    if (failure) {
      goto coda;
    }
    if (fabs(alpha) <= EPS(double)) {
      break;
    }

    lam1 += alpha*p1 , lam2 += alpha*p2;
    if (lam1 < 0 || lam2 < 0 || 1 - lam1 - lam2 < 0) {
      goto coda;
    }
    err = max(fabs(p1), fabs(p2))/max(fabs(lam1), fabs(lam2));
    ++niters;
    if (niters > 10) {
#ifdef EIKONAL_DEBUG
      printf("u0 = %0.16g, u1 = %0.16g, u2 = %0.16g, s = %0.16g, h = %0.16g\n",
             u0, u1, u2, s, h);
      std::abort();
#else
      // printf("p1 = %g, p2 = %g, alpha = %g\n", p1, p2, alpha);
      break;
#endif
    }
  } while (err > 1e-15);

  Q = lam1*lam1 + 2*lam1*lam2 + 2*lam2*lam2 + 1;
  T = (1 - lam1 - lam2)*u0 + lam1*u1 + lam2*u2 + sh*sqrt(Q);

  coda:
#if PRINT_UPDATES
  printf("tetra123(u0 = %g, u1 = %g, u2 = %g, s = %g, h = %g) -> %g\n",
         u0, u1, u2, s, h, T);
#endif
  return T;
}

double olim_rect_update_rules::tetra222(
  double u0, double u1, double u2, double s, double h) const
{
  using std::max;

#ifdef EIKONAL_DEBUG
  check_params(u0, u1, u2, s, h);
#endif

  double sh = s*h, du1 = u1 - u0, du2 = u2 - u0, T = INF(double),
    lam1 = 1./3., lam2 = 1./3., p1, p2, relerr, alpha, Q, c,
    H11, H12, H22, G1, G2;

  int niters = 0;
  bool failure = false;
  do {
    Q = (1 - lam1)*(1 - lam1) + (1 - lam2)*(1 - lam2) +
      (lam1 + lam2)*(lam1 + lam2);
    c = sqrt(Q)/sh;
    H11 = c*(3 + lam1*(3*lam1 - 2))/4.0;
    H12 = c*(lam1*(3*lam2 - 1) - lam2 - 1)/4.0;
    H22 = c*(3 + lam2*(3*lam2 - 2))/4.0;
    G1 = du1 + (2*lam1 + lam2 - 1)/c;
    G2 = du2 + (lam1 + 2*lam2 - 1)/c;
    p1 = H11*G1 + H12*G2;
    p2 = H12*G1 + H22*G2;

    alpha = compute_step_size<2, 1, 2, -1, -1>(
      p1, p2, du1, du2, lam1, lam2, sh, Q, failure);
    if (failure) {
      goto coda;
    }
    if (fabs(alpha) <= std::numeric_limits<double>::epsilon()) {
      break;
    }

    lam1 += alpha*p1 , lam2 += alpha*p2;
    if (lam1 < 0 || lam2 < 0 || 1 - lam1 - lam2 < 0) {
      goto coda;
    }
    relerr = max(fabs(p1), fabs(p2))/max(fabs(lam1), fabs(lam2));
    ++niters;
    if (niters > 10) {
#ifdef EIKONAL_DEBUG
      printf("u0 = %0.16g, u1 = %0.16g, u2 = %0.16g, s = %0.16g, h = %0.16g\n",
             u0, u1, u2, s, h);
      std::abort();
#else
      // printf("p1 = %g, p2 = %g, alpha = %g\n", p1, p2, alpha);
      break;
#endif
    }
  } while (relerr > 1e-15);

  Q = (1 - lam1)*(1 - lam1) + (1 - lam2)*(1 - lam2) +
    (lam1 + lam2)*(lam1 + lam2);
  T = (1 - lam1 - lam2)*u0 + lam1*u1 + lam2*u2 + sh*sqrt(Q);

  coda:
#if PRINT_UPDATES
  printf("tetra222(u0 = %g, u1 = %g, u2 = %g, s = %g, h = %g) -> %g\n",
         u0, u1, u2, s, h, T);
#endif
  return T;
}

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End:
