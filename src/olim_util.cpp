#include "olim_util.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>

#include <gsl/gsl_poly.h>

static void check_params(double u0, double u1, double h) {
  (void) u0;
  (void) u1;
  (void) h;
  assert(h > 0);
  assert(u0 >= 0);
  assert(u1 >= 0);
  assert(!std::isinf(u0));
  assert(!std::isinf(u1));
}

/**
 * Adjacent triangle update with constant quadrature rule (used by
 * olim8_rhr and olim8_mp0c).
 */
double rhr_adj(double u0, double u1, double s_est, double h, double * lam) {
  check_params(u0, u1, h);
  assert(s_est >= 0);

  double c = (u0 - u1)/(s_est*h);
  // TODO: use copysign for next line instead?
  double _lam = 0.5 + (c > 0 ? 1 : -1)*std::fabs(c)/(2*std::sqrt(2 - c*c));
  if (lam != nullptr) {
    *lam = _lam;
  }
  return (1 - _lam)*u0 + _lam*u1 + s_est*h*sqrt(2*_lam*(_lam - 1) + 1);
}

/**
 * Diagonal triangle update with constant quadrature rule (used by
 * olim8_rhr and olim8_mp0c).
 */
double rhr_diag(double u0, double u1, double s_est, double h) {
  check_params(u0, u1, h);
  assert(s_est >= 0);
  
  // TODO: make this one as simple as the adj rule
  double c = std::fabs(u0 - u1)/(s_est*h);
  double sgn = u0 > u1 ? 1 : -1;
  double lam = std::max(0.0, std::min(1.0, sgn*c/std::sqrt(1 - c*c)));
  return (1 - lam)*u0 + lam*u1 + s_est*h*sqrt(lam*lam + 1);
}

double mp0l_adj(double u0, double u1, double s, double s0, double s1, double h) {
  check_params(u0, u1, h);
  assert(s >= 0);
  assert(s0 >= 0);
  assert(s1 >= 0);

  double s0bar = (s + s0)/2;
  double s1bar = (s + s1)/2;
  double ds = s1 - s0;
  double du = u1 - u0;
  double b = s0bar/ds - 0.75, b_sq = b*b;
  double c = (ds - s - s0)/(4*ds) + du/(2*ds*h);
  double lam1 = (-b + std::sqrt(b_sq - 4*c))/2;
  double lam2 = (-b - std::sqrt(b_sq - 4*c))/2;

  double T = std::numeric_limits<double>::infinity(), one_minus_lam, lam_sq,
    dist, slambar;
  if (0 <= lam1 && lam1 <= 1) {
    one_minus_lam = 1 - lam1;
    lam_sq = lam1*lam1;
    slambar = (one_minus_lam*s0bar + lam1*s1bar);
    dist = h*std::sqrt(lam_sq + one_minus_lam*one_minus_lam);
    T = std::min(T, one_minus_lam*u0 + lam1*u1 + slambar*dist);
  }
  if (0 <= lam2 && lam2 <= 1) {
    one_minus_lam = 1 - lam2;
    lam_sq = lam2*lam2;
    slambar = (one_minus_lam*s0bar + lam2*s1bar);
    dist = h*std::sqrt(lam_sq + one_minus_lam*one_minus_lam);
    T = std::min(T, one_minus_lam*u0 + lam2*u1 + slambar*dist);
  }
  return T;
}

double mp0l_diag(double u0, double u1, double s, double s0, double s1, double h) {
  check_params(u0, u1, h);
  assert(s >= 0);
  assert(s0 >= 0);
  assert(s1 >= 0);

  double s0bar = (s + s0)/2;
  double s1bar = (s + s1)/2;
  double ds = s1 - s0;
  double du = u1 - u0;
  double tmp1 = s0bar/ds;
  double tmp2 = -tmp1/2;
  double tmp3 = std::sqrt(tmp1*tmp1 - 2 - 4*du/(h*ds))/2;
  double lam1 = tmp2 + tmp3;
  double lam2 = tmp2 - tmp3;

  double T = std::numeric_limits<double>::infinity(), dist, slambar;
  if (0 <= lam1 && lam1 <= 1) {
    slambar = ((1 - lam1)*s0bar + lam1*s1bar);
    dist = h*std::sqrt(lam1*lam1 + 1);
    T = std::min(T, (1 - lam1)*u0 + lam1*u1 + slambar*dist);
  }
  if (0 <= lam2 && lam2 <= 1) {
    slambar = ((1 - lam2)*s0bar + lam2*s1bar);
    dist = h*std::sqrt(lam2*lam2 + 1);
    T = std::min(T, (1 - lam2)*u0 + lam2*u1 + slambar*dist);
  }
  return T;
}

double mp1_adj(double u0, double u1, double s0, double s1, double h) {
  check_params(u0, u1, h);
  assert(s0 >= 0);
  assert(s1 >= 0);

  double alpha_sq = std::pow((u0 - u1)/h, 2);
  double s0_sq = s0*s0;
  double ds = s1 - s0;
  double ds_sq = ds*ds;
  double a[] = {
    s0_sq - alpha_sq - 2*s0*ds + ds_sq,
    -4*s0_sq + 2*alpha_sq + 10*s0*ds - 6*ds_sq,
    4*s0_sq - 2*alpha_sq - 20*s0*ds + 17*ds_sq,
    16*s0*ds - 24*ds_sq,
    16*ds_sq
  };

  double lam = -1;  double z[8];
  gsl_poly_complex_workspace * w = gsl_poly_complex_workspace_alloc(5);
  gsl_poly_complex_solve(a, 5, w, z);
  gsl_poly_complex_workspace_free(w);

  for (int i = 0; i < 4; ++i) {
    if (z[2*i + 1] != 0 || z[2*i] < 0 || z[2*i] > 1) {
      continue;
    }
    lam = z[2*i];
    double lhs = (u0 - u1)*std::sqrt(1 - 2*lam + 2*lam*lam)/h;
    double rhs = -s0*(4*lam*lam - 5*lam + 2) + s1*(4*lam*lam - 3*lam + 1);
    if (std::fabs(lhs - rhs)/std::fabs(lhs) < 1e-6) {
      break;
    }
  }
  return lam == -1 ?
    std::numeric_limits<double>::infinity() :
    (1 - lam)*u0+ lam*u1 + ((1 - lam)*s0 + lam*s1)*h*std::sqrt(1 - 2*lam + 2*lam*lam + 1);
}

double mp1_diag(double u0, double u1, double s0, double s1, double h) {
  check_params(u0, u1, h);
  assert(s0 >= 0);
  assert(s1 >= 0);

  double alpha = std::fabs((u0 - u1)/h);
  double ds = s1 - s0;
  double a[] = {
    (ds - alpha)*(ds + alpha),
    2*s0*ds,
    4*ds*ds + (s0 - alpha)*(s0 + alpha),
    4*s0*ds,
    4*ds*ds
  };

  double z[8];
  gsl_poly_complex_workspace * w = gsl_poly_complex_workspace_alloc(5);
  gsl_poly_complex_solve(a, 5, w, z);
  gsl_poly_complex_workspace_free(w);

  double lam = -1;
  for (int i = 0; i < 4; ++i) {
    if (z[2*i + 1] != 0 || z[2*i] < 0 || z[2*i] > 1) {
      continue;
    }
    lam = z[2*i];
    double lhs = (u0 - u1)*std::sqrt(lam*lam + 1)/h;
    double rhs = s1 + 2*s1*lam*lam - s0*(1 - lam + 2*lam*lam);
    if (std::fabs(lhs - rhs)/std::fabs(lhs) < 1e-6) {
      break;
    }
  }
  return lam == -1 ?
    std::numeric_limits<double>::infinity() :
    (1 - lam)*u0+ lam*u1 + ((1 - lam)*s0 + lam*s1)*h*std::sqrt(lam*lam + 1);
}

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: