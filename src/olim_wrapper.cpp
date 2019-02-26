#include "olim_wrapper.h"

#include "fac.hpp"
#include "fmm.hpp"
#include "olim.hpp"
#include "olim3d.hpp"

// TODO: this is pretty awful, but at least it works. Definitely need
// to improve on this somehow. I tried std::variant, but ran into
// problems with it. Not totally sure what the cause of them was.

struct fac_src_wrapper {
  int ndims;
  union {
    fac_src<2> _fac_src_2;
    fac_src<3> _fac_src_3;
  };

  fac_src_wrapper(int ndims, double const * coords, double s): ndims {ndims} {
    if (ndims == 2) {
      new (&_fac_src_2) fac_src<2> {{coords}, s};
    } else if (ndims == 3) {
      new (&_fac_src_3) fac_src<3> {{coords}, s};
    } else {
      assert(false);
    } 
  }
};

status_e fac_src_wrapper_init(fac_src_wrapper **w_ptr, fac_src_wrapper_params *p)
{
  *w_ptr = new fac_src_wrapper {p->ndims, p->coords, p->s};

  return SUCCESS;
}

status_e fac_src_wrapper_deinit(fac_src_wrapper_s **w_ptr)
{
  delete *w_ptr;

  return SUCCESS;
}

struct null_olim {
  void run() {}
  void add_src(int *, double) {}
  void add_bd(int *, double) {}
  double * get_U_ptr() const { return nullptr; }
  double * get_s_ptr() const { return nullptr; }
  char * get_state_ptr() const { return nullptr; }
};

union olim_variant {
  ~olim_variant() {}
  null_olim _null_olim;
  olim4_mp0 _olim4_mp0;
  olim4_mp1 _olim4_mp1;
  olim4_rhr _olim4_rhr;
  olim8_mp0 _olim8_mp0;
  olim8_mp1 _olim8_mp1;
  olim8_rhr _olim8_rhr;
  olim6_mp0 _olim6_mp0;
  olim6_mp1 _olim6_mp1;
  olim6_rhr _olim6_rhr;
  olim18_mp0 _olim18_mp0;
  olim18_mp1 _olim18_mp1;
  olim18_rhr _olim18_rhr;
  olim26_mp0 _olim26_mp0;
  olim26_mp1 _olim26_mp1;
  olim26_rhr _olim26_rhr;
  olim3d_mp0 _olim3d_mp0;
  olim3d_mp1 _olim3d_mp1;
  olim3d_rhr _olim3d_rhr;
  fmm<2> _fmm2;
  fmm<3> _fmm3;
};

struct olim_wrapper
{
  neighborhood nb;
  cost_func F;
  olim_variant _olim;

  template <class olim_t>
  olim_t & olim();
};

template <>
olim4_mp0 & olim_wrapper::olim<olim4_mp0>() {
  return _olim._olim4_mp0;
}

template <>
olim4_mp1 & olim_wrapper::olim<olim4_mp1>() {
  return _olim._olim4_mp1;
}

template <>
olim4_rhr & olim_wrapper::olim<olim4_rhr>() {
  return _olim._olim4_rhr;
}

template <>
olim8_mp0 & olim_wrapper::olim<olim8_mp0>() {
  return _olim._olim8_mp0;
}

template <>
olim8_mp1 & olim_wrapper::olim<olim8_mp1>() {
  return _olim._olim8_mp1;
}

template <>
olim8_rhr & olim_wrapper::olim<olim8_rhr>() {
  return _olim._olim8_rhr;
}

template <>
olim6_mp0 & olim_wrapper::olim<olim6_mp0>() {
  return _olim._olim6_mp0;
}

template <>
olim6_mp1 & olim_wrapper::olim<olim6_mp1>() {
  return _olim._olim6_mp1;
}

template <>
olim6_rhr & olim_wrapper::olim<olim6_rhr>() {
  return _olim._olim6_rhr;
}

template <>
olim18_mp0 & olim_wrapper::olim<olim18_mp0>() {
  return _olim._olim18_mp0;
}

template <>
olim18_mp1 & olim_wrapper::olim<olim18_mp1>() {
  return _olim._olim18_mp1;
}

template <>
olim18_rhr & olim_wrapper::olim<olim18_rhr>() {
  return _olim._olim18_rhr;
}

template <>
olim26_mp0 & olim_wrapper::olim<olim26_mp0>() {
  return _olim._olim26_mp0;
}

template <>
olim26_mp1 & olim_wrapper::olim<olim26_mp1>() {
  return _olim._olim26_mp1;
}

template <>
olim26_rhr & olim_wrapper::olim<olim26_rhr>() {
  return _olim._olim26_rhr;
}

template <>
olim3d_mp0 & olim_wrapper::olim<olim3d_mp0>() {
  return _olim._olim3d_mp0;
}

template <>
olim3d_mp1 & olim_wrapper::olim<olim3d_mp1>() {
  return _olim._olim3d_mp1;
}

template <>
olim3d_rhr & olim_wrapper::olim<olim3d_rhr>() {
  return _olim._olim3d_rhr;
}

template <>
fmm<2> & olim_wrapper::olim<fmm<2>>() {
  return _olim._fmm2;
}

template <>
fmm<3> & olim_wrapper::olim<fmm<3>>() {
  return _olim._fmm3;
}

template <class olim_t>
void construct_olim(olim_wrapper * w, olim_wrapper_params_s * p)
{
  new (&w->olim<olim_t>()) olim_t {{p->dims}, p->h, no_slow_t {}};
}

template <class olim_t>
void destruct_olim(olim_wrapper * w)
{
  w->olim<olim_t>().~olim_t();
}

status_e olim_wrapper_init(olim_wrapper ** w_ptr, olim_wrapper_params_s * p)
{
  olim_wrapper * w = (*w_ptr = new olim_wrapper {p->nb, p->F, {}});

  if (p->nb == OLIM4) {
    if (p->F == MP0) {
      construct_olim<olim4_mp0>(w, p);
    } else if (p->F == MP1) {
      construct_olim<olim4_mp1>(w, p);
    } else if (p->F == RHR) {
      construct_olim<olim4_rhr>(w, p);
    }
  } else if (p->nb == OLIM8) {
    if (p->F == MP0) {
      construct_olim<olim8_mp0>(w, p);
    } else if (p->F == MP1) {
      construct_olim<olim8_mp1>(w, p);
    } else if (p->F == RHR) {
      construct_olim<olim8_rhr>(w, p);
    }
  } else if (p->nb == OLIM6) {
    if (p->F == MP0) {
      construct_olim<olim6_mp0>(w, p);
    } else if (p->F == MP1) {
      construct_olim<olim6_mp1>(w, p);
    } else if (p->F == RHR) {
      construct_olim<olim6_rhr>(w, p);
    }
  } else if (p->nb == OLIM18) {
    if (p->F == MP0) {
      construct_olim<olim18_mp0>(w, p);
    } else if (p->F == MP1) {
      construct_olim<olim18_mp1>(w, p);
    } else if (p->F == RHR) {
      construct_olim<olim18_rhr>(w, p);
    }
  } else if (p->nb == OLIM26) {
    if (p->F == MP0) {
      construct_olim<olim26_mp0>(w, p);
    } else if (p->F == MP1) {
      construct_olim<olim26_mp1>(w, p);
    } else if (p->F == RHR) {
      construct_olim<olim26_rhr>(w, p);
    }
  } else if (p->nb == OLIM3D) {
    if (p->F == MP0) {
      construct_olim<olim3d_mp0>(w, p);
    } else if (p->F == MP1) {
      construct_olim<olim3d_mp1>(w, p);
    } else if (p->F == RHR) {
      construct_olim<olim3d_rhr>(w, p);
    }
  } else if (p->nb == FMM2) {
    if (p->F == RHR) {
      construct_olim<fmm<2>>(w, p);
    } else {
      throw std::runtime_error("FMM2 requires quad == RHR");
    }
  } else if (p->nb == FMM3) {
    if (p->F == RHR) {
      construct_olim<fmm<3>>(w, p);
    } else {
      throw std::runtime_error("FMM3 requires quad == RHR");
    }
  }

  return SUCCESS;
}

status_e olim_wrapper_deinit(olim_wrapper ** w_ptr)
{
  olim_wrapper * w = *w_ptr;

  if (w->nb == OLIM4) {
    if (w->F == MP0) {
      destruct_olim<olim4_mp0>(w);
    } else if (w->F == MP1) {
      destruct_olim<olim4_mp1>(w);
    } else if (w->F == RHR) {
      destruct_olim<olim4_rhr>(w);
    }
  } else if (w->nb == OLIM8) {
    if (w->F == MP0) {
      destruct_olim<olim8_mp0>(w);
    } else if (w->F == MP1) {
      destruct_olim<olim8_mp1>(w);
    } else if (w->F == RHR) {
      destruct_olim<olim8_rhr>(w);
    }
  } else if (w->nb == OLIM6) {
    if (w->F == MP0) {
      destruct_olim<olim6_mp0>(w);
    } else if (w->F == MP1) {
      destruct_olim<olim6_mp1>(w);
    } else if (w->F == RHR) {
      destruct_olim<olim6_rhr>(w);
    }
  } else if (w->nb == OLIM18) {
    if (w->F == MP0) {
      destruct_olim<olim18_mp0>(w);
    } else if (w->F == MP1) {
      destruct_olim<olim18_mp1>(w);
    } else if (w->F == RHR) {
      destruct_olim<olim18_rhr>(w);
    }
  } else if (w->nb == OLIM26) {
    if (w->F == MP0) {
      destruct_olim<olim26_mp0>(w);
    } else if (w->F == MP1) {
      destruct_olim<olim26_mp1>(w);
    } else if (w->F == RHR) {
      destruct_olim<olim26_rhr>(w);
    }
  } else if (w->nb == OLIM3D) {
    if (w->F == MP0) {
      destruct_olim<olim3d_mp0>(w);
    } else if (w->F == MP1) {
      destruct_olim<olim3d_mp1>(w);
    } else if (w->F == RHR) {
      destruct_olim<olim3d_rhr>(w);
    }
  } else if (w->nb == FMM2) {
    if (w->F == RHR) {
      destruct_olim<fmm<2>>(w);
    } else {
      throw std::runtime_error("FMM2 requires quad == RHR");
    }
  } else if (w->nb == FMM3) {
    if (w->F == RHR) {
      destruct_olim<fmm<3>>(w);
    } else {
      throw std::runtime_error("FMM3 requires quad == RHR");
    }
  }

  delete *w_ptr;

  return SUCCESS;
}

status_e olim_wrapper_run(olim_wrapper * w)
{
  if (w->nb == OLIM4) {
    if (w->F == MP0) {
      w->olim<olim4_mp0>().run();
    } else if (w->F == MP1) {
      w->olim<olim4_mp1>().run();
    } else if (w->F == RHR) {
      w->olim<olim4_rhr>().run();
    }
  } else if (w->nb == OLIM8) {
    if (w->F == MP0) {
      w->olim<olim8_mp0>().run();
    } else if (w->F == MP1) {
      w->olim<olim8_mp1>().run();
    } else if (w->F == RHR) {
      w->olim<olim8_rhr>().run();
    }
  } else if (w->nb == OLIM6) {
    if (w->F == MP0) {
      w->olim<olim6_mp0>().run();
    } else if (w->F == MP1) {
      w->olim<olim6_mp1>().run();
    } else if (w->F == RHR) {
      w->olim<olim6_rhr>().run();
    }
  } else if (w->nb == OLIM18) {
    if (w->F == MP0) {
      w->olim<olim18_mp0>().run();
    } else if (w->F == MP1) {
      w->olim<olim18_mp1>().run();
    } else if (w->F == RHR) {
      w->olim<olim18_rhr>().run();
    }
  } else if (w->nb == OLIM26) {
    if (w->F == MP0) {
      w->olim<olim26_mp0>().run();
    } else if (w->F == MP1) {
      w->olim<olim26_mp1>().run();
    } else if (w->F == RHR) {
      w->olim<olim26_rhr>().run();
    }
  } else if (w->nb == OLIM3D) {
    if (w->F == MP0) {
      w->olim<olim3d_mp0>().run();
    } else if (w->F == MP1) {
      w->olim<olim3d_mp1>().run();
    } else if (w->F == RHR) {
      w->olim<olim3d_rhr>().run();
    }
  } else if (w->nb == FMM2) {
    if (w->F == RHR) {
      w->olim<fmm<2>>().run();
    } else {
      throw std::runtime_error("FMM2 requires quad == RHR");
    }
  } else if (w->nb == FMM3) {
    if (w->F == RHR) {
      w->olim<fmm<3>>().run();
    } else {
      throw std::runtime_error("FMM3 requires quad == RHR");
    }
  }

  return SUCCESS;
}

status_e olim_wrapper_add_src(olim_wrapper * w, int * inds, double U)
{
  if (w->nb == OLIM4) {
    if (w->F == MP0) {
      w->olim<olim4_mp0>().add_src(inds, U);
    } else if (w->F == MP1) {
      w->olim<olim4_mp1>().add_src(inds, U);
    } else if (w->F == RHR) {
      w->olim<olim4_rhr>().add_src(inds, U);
    }
  } else if (w->nb == OLIM8) {
    if (w->F == MP0) {
      w->olim<olim8_mp0>().add_src(inds, U);
    } else if (w->F == MP1) {
      w->olim<olim8_mp1>().add_src(inds, U);
    } else if (w->F == RHR) {
      w->olim<olim8_rhr>().add_src(inds, U);
    }
  } else if (w->nb == OLIM6) {
    if (w->F == MP0) {
      w->olim<olim6_mp0>().add_src(inds, U);
    } else if (w->F == MP1) {
      w->olim<olim6_mp1>().add_src(inds, U);
    } else if (w->F == RHR) {
      w->olim<olim6_rhr>().add_src(inds, U);
    }
  } else if (w->nb == OLIM18) {
    if (w->F == MP0) {
      w->olim<olim18_mp0>().add_src(inds, U);
    } else if (w->F == MP1) {
      w->olim<olim18_mp1>().add_src(inds, U);
    } else if (w->F == RHR) {
      w->olim<olim18_rhr>().add_src(inds, U);
    }
  } else if (w->nb == OLIM26) {
    if (w->F == MP0) {
      w->olim<olim26_mp0>().add_src(inds, U);
    } else if (w->F == MP1) {
      w->olim<olim26_mp1>().add_src(inds, U);
    } else if (w->F == RHR) {
      w->olim<olim26_rhr>().add_src(inds, U);
    }
  } else if (w->nb == OLIM3D) {
    if (w->F == MP0) {
      w->olim<olim3d_mp0>().add_src(inds, U);
    } else if (w->F == MP1) {
      w->olim<olim3d_mp1>().add_src(inds, U);
    } else if (w->F == RHR) {
      w->olim<olim3d_rhr>().add_src(inds, U);
    }
  } else if (w->nb == FMM2) {
    if (w->F == RHR) {
      w->olim<fmm<2>>().add_src(inds, U);
    } else {
      throw std::runtime_error("FMM2 requires quad == RHR");
    }
  } else if (w->nb == FMM3) {
    if (w->F == RHR) {
      w->olim<fmm<3>>().add_src(inds, U);
    } else {
      throw std::runtime_error("FMM3 requires quad == RHR");
    }
  }

  return SUCCESS;
}

status_e
olim_wrapper_set_fac_src(olim_wrapper * w, int * inds, fac_src_wrapper * fs)
{
  if (w->nb == OLIM4) {
    if (w->F == MP0) {
      w->olim<olim4_mp0>().set_fac_src({inds}, &fs->_fac_src_2);
    } else if (w->F == MP1) {
      w->olim<olim4_mp1>().set_fac_src({inds}, &fs->_fac_src_2);
    } else if (w->F == RHR) {
      w->olim<olim4_rhr>().set_fac_src({inds}, &fs->_fac_src_2);
    }
  } else if (w->nb == OLIM8) {
    if (w->F == MP0) {
      w->olim<olim8_mp0>().set_fac_src({inds}, &fs->_fac_src_2);
    } else if (w->F == MP1) {
      w->olim<olim8_mp1>().set_fac_src({inds}, &fs->_fac_src_2);
    } else if (w->F == RHR) {
      w->olim<olim8_rhr>().set_fac_src({inds}, &fs->_fac_src_2);
    }
  } else if (w->nb == OLIM6) {
    if (w->F == MP0) {
      w->olim<olim6_mp0>().set_fac_src({inds}, &fs->_fac_src_3);
    } else if (w->F == MP1) {
      w->olim<olim6_mp1>().set_fac_src({inds}, &fs->_fac_src_3);
    } else if (w->F == RHR) {
      w->olim<olim6_rhr>().set_fac_src({inds}, &fs->_fac_src_3);
    }
  } else if (w->nb == OLIM18) {
    if (w->F == MP0) {
      w->olim<olim18_mp0>().set_fac_src({inds}, &fs->_fac_src_3);
    } else if (w->F == MP1) {
      w->olim<olim18_mp1>().set_fac_src({inds}, &fs->_fac_src_3);
    } else if (w->F == RHR) {
      w->olim<olim18_rhr>().set_fac_src({inds}, &fs->_fac_src_3);
    }
  } else if (w->nb == OLIM26) {
    if (w->F == MP0) {
      w->olim<olim26_mp0>().set_fac_src({inds}, &fs->_fac_src_3);
    } else if (w->F == MP1) {
      w->olim<olim26_mp1>().set_fac_src({inds}, &fs->_fac_src_3);
    } else if (w->F == RHR) {
      w->olim<olim26_rhr>().set_fac_src({inds}, &fs->_fac_src_3);
    }
  } else if (w->nb == OLIM3D) {
    if (w->F == MP0) {
      w->olim<olim3d_mp0>().set_fac_src({inds}, &fs->_fac_src_3);
    } else if (w->F == MP1) {
      w->olim<olim3d_mp1>().set_fac_src({inds}, &fs->_fac_src_3);
    } else if (w->F == RHR) {
      w->olim<olim3d_rhr>().set_fac_src({inds}, &fs->_fac_src_3);
    }
  } else if (w->nb == FMM2) {
    throw std::runtime_error("FMM2 doesn't currently support factoring");
  } else if (w->nb == FMM3) {
    throw std::runtime_error("FMM3 doesn't currently support factoring");
  }

  return SUCCESS;
}

status_e olim_wrapper_add_bd(olim_wrapper * w, int * inds)
{
  if (w->nb == OLIM4) {
    if (w->F == MP0) {
      w->olim<olim4_mp0>().add_bd(inds);
    } else if (w->F == MP1) {
      w->olim<olim4_mp1>().add_bd(inds);
    } else if (w->F == RHR) {
      w->olim<olim4_rhr>().add_bd(inds);
    }
  } else if (w->nb == OLIM8) {
    if (w->F == MP0) {
      w->olim<olim8_mp0>().add_bd(inds);
    } else if (w->F == MP1) {
      w->olim<olim8_mp1>().add_bd(inds);
    } else if (w->F == RHR) {
      w->olim<olim8_rhr>().add_bd(inds);
    }
  } else if (w->nb == OLIM6) {
    if (w->F == MP0) {
      w->olim<olim6_mp0>().add_bd(inds);
    } else if (w->F == MP1) {
      w->olim<olim6_mp1>().add_bd(inds);
    } else if (w->F == RHR) {
      w->olim<olim6_rhr>().add_bd(inds);
    }
  } else if (w->nb == OLIM18) {
    if (w->F == MP0) {
      w->olim<olim18_mp0>().add_bd(inds);
    } else if (w->F == MP1) {
      w->olim<olim18_mp1>().add_bd(inds);
    } else if (w->F == RHR) {
      w->olim<olim18_rhr>().add_bd(inds);
    }
  } else if (w->nb == OLIM26) {
    if (w->F == MP0) {
      w->olim<olim26_mp0>().add_bd(inds);
    } else if (w->F == MP1) {
      w->olim<olim26_mp1>().add_bd(inds);
    } else if (w->F == RHR) {
      w->olim<olim26_rhr>().add_bd(inds);
    }
  } else if (w->nb == OLIM3D) {
    if (w->F == MP0) {
      w->olim<olim3d_mp0>().add_bd(inds);
    } else if (w->F == MP1) {
      w->olim<olim3d_mp1>().add_bd(inds);
    } else if (w->F == RHR) {
      w->olim<olim3d_rhr>().add_bd(inds);
    }
  } else if (w->nb == FMM2) {
    if (w->F == RHR) {
      w->olim<fmm<2>>().add_bd(inds);
    } else {
      throw std::runtime_error("FMM2 requires quad == RHR");
    }
  } else if (w->nb == FMM3) {
    if (w->F == RHR) {
      w->olim<fmm<3>>().add_bd(inds);
    } else {
      throw std::runtime_error("FMM3 requires quad == RHR");
    }
  }

  return SUCCESS;
}

status_e olim_wrapper_get_U_ptr(olim_wrapper * w, double ** U_ptr)
{
  if (w->nb == OLIM4) {
    if (w->F == MP0) {
      *U_ptr = w->olim<olim4_mp0>().get_U_ptr();
    } else if (w->F == MP1) {
      *U_ptr = w->olim<olim4_mp1>().get_U_ptr();
    } else if (w->F == RHR) {
      *U_ptr = w->olim<olim4_rhr>().get_U_ptr();
    }
  } else if (w->nb == OLIM8) {
    if (w->F == MP0) {
      *U_ptr = w->olim<olim8_mp0>().get_U_ptr();
    } else if (w->F == MP1) {
      *U_ptr = w->olim<olim8_mp1>().get_U_ptr();
    } else if (w->F == RHR) {
      *U_ptr = w->olim<olim8_rhr>().get_U_ptr();
    }
  } else if (w->nb == OLIM6) {
    if (w->F == MP0) {
      *U_ptr = w->olim<olim6_mp0>().get_U_ptr();
    } else if (w->F == MP1) {
      *U_ptr = w->olim<olim6_mp1>().get_U_ptr();
    } else if (w->F == RHR) {
      *U_ptr = w->olim<olim6_rhr>().get_U_ptr();
    }
  } else if (w->nb == OLIM18) {
    if (w->F == MP0) {
      *U_ptr = w->olim<olim18_mp0>().get_U_ptr();
    } else if (w->F == MP1) {
      *U_ptr = w->olim<olim18_mp1>().get_U_ptr();
    } else if (w->F == RHR) {
      *U_ptr = w->olim<olim18_rhr>().get_U_ptr();
    }
  } else if (w->nb == OLIM26) {
    if (w->F == MP0) {
      *U_ptr = w->olim<olim26_mp0>().get_U_ptr();
    } else if (w->F == MP1) {
      *U_ptr = w->olim<olim26_mp1>().get_U_ptr();
    } else if (w->F == RHR) {
      *U_ptr = w->olim<olim26_rhr>().get_U_ptr();
    }
  } else if (w->nb == OLIM3D) {
    if (w->F == MP0) {
      *U_ptr = w->olim<olim3d_mp0>().get_U_ptr();
    } else if (w->F == MP1) {
      *U_ptr = w->olim<olim3d_mp1>().get_U_ptr();
    } else if (w->F == RHR) {
      *U_ptr = w->olim<olim3d_rhr>().get_U_ptr();
    }
  } else if (w->nb == FMM2) {
    if (w->F == RHR) {
      *U_ptr = w->olim<fmm<2>>().get_U_ptr();
    } else {
      throw std::runtime_error("FMM2 requires quad == RHR");
    }
  } else if (w->nb == FMM3) {
    if (w->F == RHR) {
      *U_ptr = w->olim<fmm<3>>().get_U_ptr();
    } else {
      throw std::runtime_error("FMM3 requires quad == RHR");
    }
  }

  return SUCCESS;
}

status_e olim_wrapper_get_s_ptr(olim_wrapper * w, double ** s_ptr)
{
  if (w->nb == OLIM4) {
    if (w->F == MP0) {
      *s_ptr = w->olim<olim4_mp0>().get_s_ptr();
    } else if (w->F == MP1) {
      *s_ptr = w->olim<olim4_mp1>().get_s_ptr();
    } else if (w->F == RHR) {
      *s_ptr = w->olim<olim4_rhr>().get_s_ptr();
    }
  } else if (w->nb == OLIM8) {
    if (w->F == MP0) {
      *s_ptr = w->olim<olim8_mp0>().get_s_ptr();
    } else if (w->F == MP1) {
      *s_ptr = w->olim<olim8_mp1>().get_s_ptr();
    } else if (w->F == RHR) {
      *s_ptr = w->olim<olim8_rhr>().get_s_ptr();
    }
  } else if (w->nb == OLIM6) {
    if (w->F == MP0) {
      *s_ptr = w->olim<olim6_mp0>().get_s_ptr();
    } else if (w->F == MP1) {
      *s_ptr = w->olim<olim6_mp1>().get_s_ptr();
    } else if (w->F == RHR) {
      *s_ptr = w->olim<olim6_rhr>().get_s_ptr();
    }
  } else if (w->nb == OLIM18) {
    if (w->F == MP0) {
      *s_ptr = w->olim<olim18_mp0>().get_s_ptr();
    } else if (w->F == MP1) {
      *s_ptr = w->olim<olim18_mp1>().get_s_ptr();
    } else if (w->F == RHR) {
      *s_ptr = w->olim<olim18_rhr>().get_s_ptr();
    }
  } else if (w->nb == OLIM26) {
    if (w->F == MP0) {
      *s_ptr = w->olim<olim26_mp0>().get_s_ptr();
    } else if (w->F == MP1) {
      *s_ptr = w->olim<olim26_mp1>().get_s_ptr();
    } else if (w->F == RHR) {
      *s_ptr = w->olim<olim26_rhr>().get_s_ptr();
    }
  } else if (w->nb == OLIM3D) {
    if (w->F == MP0) {
      *s_ptr = w->olim<olim3d_mp0>().get_s_ptr();
    } else if (w->F == MP1) {
      *s_ptr = w->olim<olim3d_mp1>().get_s_ptr();
    } else if (w->F == RHR) {
      *s_ptr = w->olim<olim3d_rhr>().get_s_ptr();
    }
  } else if (w->nb == FMM2) {
    if (w->F == RHR) {
      *s_ptr = w->olim<fmm<2>>().get_s_ptr();
    } else {
      throw std::runtime_error("FMM2 requires quad == RHR");
    }
  } else if (w->nb == FMM3) {
    if (w->F == RHR) {
      *s_ptr = w->olim<fmm<3>>().get_s_ptr();
    } else {
      throw std::runtime_error("FMM3 requires quad == RHR");
    }
  }

  return SUCCESS;
}

status_e olim_wrapper_get_state_ptr(olim_wrapper * w, char ** state_ptr)
{
  if (w->nb == OLIM4) {
    if (w->F == MP0) {
      *state_ptr = w->olim<olim4_mp0>().get_state_ptr();
    } else if (w->F == MP1) {
      *state_ptr = w->olim<olim4_mp1>().get_state_ptr();
    } else if (w->F == RHR) {
      *state_ptr = w->olim<olim4_rhr>().get_state_ptr();
    }
  } else if (w->nb == OLIM8) {
    if (w->F == MP0) {
      *state_ptr = w->olim<olim8_mp0>().get_state_ptr();
    } else if (w->F == MP1) {
      *state_ptr = w->olim<olim8_mp1>().get_state_ptr();
    } else if (w->F == RHR) {
      *state_ptr = w->olim<olim8_rhr>().get_state_ptr();
    }
  } else if (w->nb == OLIM6) {
    if (w->F == MP0) {
      *state_ptr = w->olim<olim6_mp0>().get_state_ptr();
    } else if (w->F == MP1) {
      *state_ptr = w->olim<olim6_mp1>().get_state_ptr();
    } else if (w->F == RHR) {
      *state_ptr = w->olim<olim6_rhr>().get_state_ptr();
    }
  } else if (w->nb == OLIM18) {
    if (w->F == MP0) {
      *state_ptr = w->olim<olim18_mp0>().get_state_ptr();
    } else if (w->F == MP1) {
      *state_ptr = w->olim<olim18_mp1>().get_state_ptr();
    } else if (w->F == RHR) {
      *state_ptr = w->olim<olim18_rhr>().get_state_ptr();
    }
  } else if (w->nb == OLIM26) {
    if (w->F == MP0) {
      *state_ptr = w->olim<olim26_mp0>().get_state_ptr();
    } else if (w->F == MP1) {
      *state_ptr = w->olim<olim26_mp1>().get_state_ptr();
    } else if (w->F == RHR) {
      *state_ptr = w->olim<olim26_rhr>().get_state_ptr();
    }
  } else if (w->nb == OLIM3D) {
    if (w->F == MP0) {
      *state_ptr = w->olim<olim3d_mp0>().get_state_ptr();
    } else if (w->F == MP1) {
      *state_ptr = w->olim<olim3d_mp1>().get_state_ptr();
    } else if (w->F == RHR) {
      *state_ptr = w->olim<olim3d_rhr>().get_state_ptr();
    }
  } else if (w->nb == FMM2) {
    if (w->F == RHR) {
      *state_ptr = w->olim<fmm<2>>().get_state_ptr();
    } else {
      throw std::runtime_error("FMM2 requires quad == RHR");
    }
  } else if (w->nb == FMM3) {
    if (w->F == RHR) {
      *state_ptr = w->olim<fmm<3>>().get_state_ptr();
    } else {
      throw std::runtime_error("FMM3 requires quad == RHR");
    }
  }

  return SUCCESS;
}
