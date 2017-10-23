#ifndef __UPDATE_RULES_TRI_UPDATES_IMPL_HPP__
#define __UPDATE_RULES_TRI_UPDATES_IMPL_HPP__

#include <src/config.hpp>

#if PRINT_UPDATES
#    include <cstdio>
#endif

#include "common.hpp"
#include "common.defs.hpp"
#include "common.macros.hpp"
#include "olim_util.hpp"
#include "qroots.hpp"

namespace update_rules {
  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri11(
    double u0, double u1, double s, double s0, double s1, double h)
    const
  {
    return tri11_impl(
      u0, u1, s, s0, s1, h, eikonal::bool_t<is_constrained> {});
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri12(
    double u0, double u1, double s, double s0, double s1, double h)
    const
  {
    return tri12_impl(
      u0, u1, s, s0, s1, h, eikonal::bool_t<is_constrained> {});
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri13(
    double u0, double u1, double s, double s0, double s1, double h)
    const
  {
    return tri13_impl(
      u0, u1, s, s0, s1, h, eikonal::bool_t<is_constrained> {});
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri22(
    double u0, double u1, double s, double s0, double s1, double h)
    const
  {
    return tri22_impl(
      u0, u1, s, s0, s1, h, eikonal::bool_t<is_constrained> {});
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri23(
    double u0, double u1, double s, double s0, double s1, double h)
    const
  {
    return tri23_impl(
      u0, u1, s, s0, s1, h, eikonal::bool_t<is_constrained> {});
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri11_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::true_type &&) const
  {
#ifdef EIKONAL_DEBUG
    check_params(u0, u1, s, s0, s1, h);
#endif
    double sh = this->s_hat(s, s0, s1)*h;
    double c = (u0 - u1)/sh;
    double lam = 0.5 + (c > 0 ? 1 : -1)*std::fabs(c)/(2*sqrt(2 - c*c));
#if PRINT_UPDATES
    double tmp = (1 - lam)*u0 + lam*u1 + sh*sqrt(2*lam*(lam - 1) + 1);
    printf("tri11(u0 = %g, u1 = %g, s = %g, s0 = %g, s1 = %g, h = %g, "
           "is_constrained = true) -> %g\n",
           u0, u1, s, s0, s1, h, tmp);
    return tmp;
#endif
    return (1 - lam)*u0 + lam*u1 + s*h*sqrt(2*lam*(lam - 1) + 1);
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri11_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::false_type &&) const
  {
#ifdef EIKONAL_DEBUG
    check_params(u0, u1, s, s0, s1, h);
#endif
    double sh = this->s_hat(s, s0, s1)*h, du = u1 - u0, alpha = fabs(du)/sh,
      alpha_sq = alpha*alpha;
    double T = INF(double);
    if (alpha_sq > 2) {
      return T;
    }
    double sgn = du < 0 ? 1 : -1;
    double lam = 0.5 + sgn*alpha/(2*sqrt(2 - alpha_sq));
    double l = sqrt(2*lam*(lam - 1) + 1);
    if (0 <= lam && lam <= 1) {
      T = (1 - lam)*u0 + lam*u1 + sh*l;
    }
#if PRINT_UPDATES
    printf("tri11(u0 = %g, u1 = %g, s = %g, s0 = %g, s1 = %g, h = %g, "
           "is_constrained = false) -> %g\n", u0, u1, s, s0, s1, h, T);
#endif
    return T;
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri12_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::true_type &&) const
  {
#ifdef EIKONAL_DEBUG
    check_params(u0, u1, s, s0, s1, h);
#endif
    double sh = this->s_hat(s, s0, s1)*h, c = std::fabs(u0 - u1)/sh;
    double sgn = u0 > u1 ? 1 : -1;
    double lam = std::max(0.0, std::min(1.0, sgn*c/sqrt(1 - c*c)));
#if PRINT_UPDATES
    double tmp = (1 - lam)*u0 + lam*u1 + sh*std::sqrt(lam*lam + 1);
    printf("tri12(u0 = %g, u1 = %g, s = %g, s0 = %g, s1 = %g, h = %g, "
           "is_constrained = true) -> %g\n", u0, u1, s, s0, s1, h, tmp);
    return tmp;
#endif
    return (1 - lam)*u0 + lam*u1 + sh*std::sqrt(lam*lam + 1);
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri12_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::false_type &&)
    const
  {
#ifdef EIKONAL_DEBUG
    check_params(u0, u1, s, s0, s1, h);
#endif
    double sh = this->s_hat(s, s0, s1)*h, du = u1 - u0, alpha = fabs(du)/sh,
      alpha_sq = alpha*alpha;
    double T = INF(double);
    if (alpha_sq > 1) {
      return T;
    }
    double lam = alpha/sqrt(1 - alpha_sq), l = sqrt(lam*lam + 1);
    if (0 <= lam && lam <= 1 && fabs(du*l + sh*lam) < 1e-13) {
      T = (1 - lam)*u0 + lam*u1 + sh*l;
    } else if (0 <= -lam && -lam <= 1 && fabs(du*l - sh*lam) < 1e-13) {
      T = (1 + lam)*u0 - lam*u1 + sh*l;
    }
#if PRINT_UPDATES
    printf("tri12(u0 = %g, u1 = %g, s = %g, s0 = %g, s1 = %g, h = %g, "
           "is_constrained = false) -> %g\n", u0, u1, s, s0, s1, h, T);
#endif
    return T;
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri13_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::false_type &&)
    const
  {
#ifdef EIKONAL_DEBUG
    check_params(u0, u1, s, s0, s1, h);
#endif
    double sh = this->s_hat(s, s0, s1)*h, du = u1 - u0, alpha = fabs(du)/sh,
      alpha_sq = alpha*alpha;
    double T = INF(double);
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
    printf("tri13(u0 = %g, u1 = %g, s = %g, s0 = %g, s1 = %g, h = %g, "
           "is_constrained = false) -> %g\n", u0, u1, s, s0, s1, h, T);
#endif
    return T;
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri22_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::false_type &&)
    const
  {
#ifdef EIKONAL_DEBUG
    check_params(u0, u1, s, s0, s1, h);
#endif
    double sh = this->s_hat(s, s0, s1)*h, du = u1 - u0, alpha = fabs(du)/sh,
      alpha_sq = alpha*alpha;
    double T = INF(double);
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
    printf("tri22(u0 = %g, u1 = %g, s = %g, s0 = %g, s1 = %g, h = %g, "
           "is_constrained = false) -> %g\n", u0, u1, s, s0, s1, h, T);
#endif
    return T;
  }

  template <class speed_estimator, bool is_constrained>
  double
  rect_tri_updates<speed_estimator, is_constrained>::tri23_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::false_type &&)
    const
  {
#ifdef EIKONAL_DEBUG
    check_params(u0, u1, s, s0, s1, h);
#endif
    double sh = this->s_hat(s, s0, s1)*h, du = u1 - u0, alpha = fabs(du)/sh,
      alpha_sq = alpha*alpha;
    double T = INF(double);
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
    printf("tri23(u0 = %g, u1 = %g, s = %g, s0 = %g, s1 = %g, h = %g, "
           "is_constrained = false) -> %g\n", u0, u1, s, s0, s1, h, T);
#endif
    return T;
  }

  template <bool is_constrained>
  double mp1_tri_updates<is_constrained>::tri11(
    double u0, double u1, double s, double s0, double s1, double h) const
  {
    return tri11_impl(
      u0, u1, s, s0, s1, h, eikonal::bool_t<is_constrained> {});
  }

  template <bool is_constrained>
  double mp1_tri_updates<is_constrained>::tri12(
    double u0, double u1, double s, double s0, double s1, double h) const
  {
    return tri12_impl(
      u0, u1, s, s0, s1, h, eikonal::bool_t<is_constrained> {});
  }

  template <bool is_constrained>
  double
  mp1_tri_updates<is_constrained>::tri11_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::true_type &&) const
  {
    double T = tri11_impl(u0, u1, s, s0, s1, h, std::false_type {});
    return std::isinf(T) ?
      std::min(u0 + (s + s0)*h/2, u1 + (s + s1)*h/2) : T;
  }

  template <bool is_constrained>
  double
  mp1_tri_updates<is_constrained>::tri11_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::false_type &&) const
  {
#ifdef EIKONAL_DEBUG
    check_params(u0, u1, s, s0, s1, h);
#endif
    double sbar0 = (s + s0)/2, sbar1 = (s + s1)/2;
    if (sbar0 == sbar1) {
      return rhr_adj(u0, u1, sbar0, h);
    }

    double alpha_sq = std::pow((u0 - u1)/h, 2);
    double sbar0_sq = sbar0*sbar0;
    double dsbar = sbar1 - sbar0;
    double dsbar_sq = dsbar*dsbar;
    double a[] = {
      sbar0_sq - alpha_sq - 2*sbar0*dsbar + dsbar_sq,
      -4*sbar0_sq + 2*alpha_sq + 10*sbar0*dsbar - 6*dsbar_sq,
      4*sbar0_sq - 2*alpha_sq - 20*sbar0*dsbar + 17*dsbar_sq,
      16*sbar0*dsbar - 24*dsbar_sq,
      16*dsbar_sq
    };

    double lam, roots[4] = {-1, -1, -1, -1},
      T = std::numeric_limits<double>::infinity(), lhs, rhs;
    qroots(a, roots);

    int i = 0;
    while ((lam = roots[i++]) != -1) {
      lhs = (u0 - u1)*std::sqrt(1 - 2*lam + 2*lam*lam)/h;
      rhs = -sbar0*(4*lam*lam - 5*lam + 2) + sbar1*(4*lam*lam - 3*lam + 1);
      if (fabs(lhs - rhs)/fabs(lhs) < 1e-6) {
        T = std::min(
          T,
          (1 - lam)*u0 + lam*u1 +
            ((1 - lam)*sbar0 + lam*sbar1)*h*std::sqrt(1 - 2*lam + 2*lam*lam + 1));
      }
    }

    return T;
  }

  template <bool is_constrained>
  double
  mp1_tri_updates<is_constrained>::tri12_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::true_type &&) const
  {
    double T = tri12_impl(u0, u1, s, s0, s1, h, std::false_type {});
    return std::isinf(T) ?
      std::min(u0 + (s + s0)*h/2, u1 + (s + s1)*h*sqrt2/2) : T;
  }

  template <bool is_constrained>
  double
  mp1_tri_updates<is_constrained>::tri12_impl(
    double u0, double u1, double s, double s0, double s1, double h,
    std::false_type &&) const
  {
#ifdef EIKONAL_DEBUG
    check_params(u0, u1, s, s0, s1, h);
#endif
    double sbar0 = (s + s0)/2, sbar1 = (s + s1)/2;
    if (sbar0 == sbar1) {
      return rhr_diag(u0, u1, sbar0, h);
    }

    double alpha = std::fabs((u0 - u1)/h);
    double dsbar = sbar1 - sbar0;
    double const a[] = {
      (dsbar - alpha)*(dsbar + alpha),
      2*sbar0*dsbar,
      4*dsbar*dsbar + (sbar0 - alpha)*(sbar0 + alpha),
      4*sbar0*dsbar,
      4*dsbar*dsbar
    };

    double lam, roots[4] = {-1, -1, -1, -1},
      T = std::numeric_limits<double>::infinity(), lhs, rhs;
    qroots(a, roots);

    int i = 0;
    while ((lam = roots[i++]) != -1) {
      lhs = (u0 - u1)*std::sqrt(lam*lam + 1)/h;
      rhs = sbar1 + 2*sbar1*lam*lam - sbar0*(1 - lam + 2*lam*lam);
      if (fabs(lhs - rhs)/fabs(lhs) < 1e-6) {
        T = std::min(
          T,
          (1 - lam)*u0+ lam*u1 +
            ((1 - lam)*sbar0 + lam*sbar1)*h*std::sqrt(lam*lam + 1));
      }
    }

    return T;
  }
}

#endif // __UPDATE_RULES_TRI_UPDATES_IMPL_HPP__

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: