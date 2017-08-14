#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

enum marcher_type {
  BASIC,
  OLIM4_MP0,
  OLIM4_RHR,
  OLIM4_RHR_LUT,
  OLIM8_MP0C,
  OLIM8_MP0L,
  OLIM8_MP1,
  OLIM8_RHR,
  SOLIM4_MP0
};

typedef double(* speed_func)(double, double);
typedef double(* speed_func_3d)(double, double, double);

#endif // __TYPEDEFS_H__

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: