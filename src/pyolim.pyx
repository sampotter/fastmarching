# cython: embedsignature=True
# cython: language_level=3

import numpy as np

from enum import Enum

from libc.stdlib cimport malloc, free

class Neighborhood(Enum):
    OLIM4 = 0
    OLIM8 = 1
    OLIM6 = 2
    OLIM18 = 3
    OLIM26 = 4
    OLIM3D = 5
    FMM2 = 6
    FMM3 = 7

class Quadrature(Enum):
    MP0 = 0
    MP1 = 1
    RHR = 2

class State(Enum):
    VALID = 0
    TRIAL = 1
    FAR = 2
    BOUNDARY = 3

cdef extern from "olim_wrapper.h":
    enum neighborhood:
        OLIM4
        OLIM8
        OLIM6
        OLIM18
        OLIM26
        OLIM3D
        FMM2
        FMM3

    enum cost_func:
        MP0
        MP1
        RHR

    enum status:
        SUCCESS

    struct fac_src_wrapper_params:
        int ndims
        double *coords
        double s

    struct fac_src_wrapper:
        pass

    status fac_src_wrapper_init(fac_src_wrapper**, fac_src_wrapper_params *);
    status fac_src_wrapper_deinit(fac_src_wrapper**);

    struct olim_wrapper_params:
        neighborhood nb
        cost_func F
        double h
        int *dims
        int ndims

    struct olim_wrapper:
        pass

    status olim_wrapper_init(olim_wrapper**, olim_wrapper_params*)
    status olim_wrapper_deinit(olim_wrapper**)
    status olim_wrapper_run(olim_wrapper*)
    status olim_wrapper_add_src(olim_wrapper*, int*)
    status olim_wrapper_add_bd(olim_wrapper*, int*, double)
    status olim_wrapper_set_fac_src(olim_wrapper*, int*, void*)
    status olim_wrapper_get_U_ptr(olim_wrapper*, double**)
    status olim_wrapper_get_s_ptr(olim_wrapper*, double**)
    status olim_wrapper_get_state_ptr(olim_wrapper*, char**)

cdef class FacSrc:
    cdef:
        fac_src_wrapper_params _p
        fac_src_wrapper* _w

    def __cinit__(self, coords, s):
        self._p.ndims = len(coords)
        self._p.coords = <double*>malloc(self._p.ndims*sizeof(double))
        self._p.s = s
        err = fac_src_wrapper_init(&self._w, &self._p)
        if err != SUCCESS:
            raise Exception('error!')

    def __dealloc__(self):
        free(self._p.coords)
        fac_src_wrapper_deinit(&self._w)

cdef class Olim:
    cdef:
        olim_wrapper_params _p
        olim_wrapper* _w

    @property
    def dims(self):
        if self._p.ndims == 2:
            return self._p.dims[0], self._p.dims[1]
        elif self._p.ndims == 3:
            return self._p.dims[0], self._p.dims[1], self._p.dims[2]

    @property
    def U(self):
        cdef double * U
        cdef double[:, ::1] mv_2
        cdef double[:, :, ::1] mv_3
        olim_wrapper_get_U_ptr(self._w, &U)
        if self._p.ndims == 2:
            M, N = self.dims
            mv_2 = <double[:(M + 2), :(N + 2)]> U
            return np.asarray(mv_2[1:-1, 1:-1])
        elif self._p.ndims == 3:
            M, N, P = self.dims
            mv_3 = <double[:(M + 2), :(N + 2), :(P + 2)]> U
            return np.asarray(mv_3[1:-1, 1:-1, 1:-1])

    @U.setter
    def U(self, U_mv):
        if self._p.ndims == 2:
            self.set_U_2(U_mv)
        if self._p.ndims == 3:
            self.set_U_3(U_mv)

    cdef set_U_2(self, double[:, ::1] U_mv):
        cdef double * U = NULL
        olim_wrapper_get_U_ptr(self._w, &U)
        M, N = self.dims
        cdef double[:, ::1] mv = <double[:(M + 2), :(N + 2)]> U
        mv[1:-1, 1:-1] = U_mv

    cdef set_U_3(self, double[:, :, ::1] U_mv):
        cdef double * U = NULL
        olim_wrapper_get_U_ptr(self._w, &U)
        M, N, P = self.dims
        cdef double[:, :, ::1] mv = <double[:(M + 2), :(N + 2), :(P + 2)]> U
        mv[1:-1, 1:-1, 1:-1] = U_mv

    @property
    def s(self):
        cdef double * s
        cdef double[:, ::1] mv_2
        cdef double[:, :, ::1] mv_3
        olim_wrapper_get_s_ptr(self._w, &s)
        if self._p.ndims == 2:
            M, N = self.dims
            mv_2 = <double[:(M + 2), :(N + 2)]> s
            return np.asarray(mv_2[1:-1, 1:-1])
        elif self._p.ndims == 3:
            M, N, P = self.dims
            mv_3 = <double[:(M + 2), :(N + 2), :(P + 2)]> s
            return np.asarray(mv_3[1:-1, 1:-1, 1:-1])

    @s.setter
    def s(self, s_mv):
        if self._p.ndims == 2:
            self.set_s_2(s_mv)
        if self._p.ndims == 3:
            self.set_s_3(s_mv)

    cdef set_s_2(self, double[:, ::1] s_mv):
        cdef double * s = NULL
        olim_wrapper_get_s_ptr(self._w, &s)
        M, N = self.dims
        cdef double[:, ::1] mv = <double[:(M + 2), :(N + 2)]> s
        mv[1:-1, 1:-1] = s_mv

    cdef set_s_3(self, double[:, :, ::1] s_mv):
        cdef double * s = NULL
        olim_wrapper_get_s_ptr(self._w, &s)
        M, N, P = self.dims
        cdef double[:, :, ::1] mv = <double[:(M + 2), :(N + 2), :(P + 2)]> s
        mv[1:-1, 1:-1, 1:-1] = s_mv

    @property
    def state(self):
        cdef char * state
        cdef char[:, ::1] mv_2
        cdef char[:, :, ::1] mv_3
        olim_wrapper_get_state_ptr(self._w, &state)
        if self._p.ndims == 2:
            M, N = self.dims
            mv_2 = <char[:(M + 2), :(N + 2)]> state
            return np.asarray(mv_2[1:-1, 1:-1])
        elif self._p.ndims == 3:
            M, N, P = self.dims
            mv_3 = <char[:M, :N, :P]> state
            return np.asarray(mv_3[1:-1, 1:-1, 1:-1])

    def __cinit__(self, nb, quad, s, double h):
        self._p.nb = nb.value
        self._p.F = quad.value
        self._p.h = h
        self._p.dims = <int *> malloc(s.ndim*sizeof(int))
        for i in range(s.ndim):
            self._p.dims[i] = s.shape[i]
        self._p.ndims = s.ndim

        err = olim_wrapper_init(&self._w, &self._p)
        if err != SUCCESS:
            raise Exception('error!')

        self.s[...] = s

    def __dealloc__(self):
        free(self._p.dims)
        err = olim_wrapper_deinit(&self._w)
        if err != SUCCESS:
            raise Exception('error!')

    def run(self):
        olim_wrapper_run(self._w)

    def add_src(self, *args):
        if self._p.ndims == 2:
            if len(args) == 2:
                self.add_src_2(list(args[:2]))
            elif len(args) == 3:
                self.add_src_2(list(args[:2]), U=args[2])
            else:
                raise Exception('error!')
        elif self._p.ndims == 3:
            if len(args) == 3:
                self.add_src_3(list(args[:3]))
            elif len(args) == 4:
                self.add_src_3(list(args[:3]), U=args[3])
            else:
                raise Exception('error!')

    cdef add_src_2(self, list inds, double U=0):
        cdef int inds_[2]
        inds_[0] = inds[0]
        inds_[1] = inds[1]
        olim_wrapper_add_src(self._w, inds_, U)

    cdef add_src_3(self, list inds, double U=0):
        cdef int inds_[3]
        inds_[0] = inds[0]
        inds_[1] = inds[1]
        inds_[2] = inds[2]
        olim_wrapper_add_src(self._w, inds_, U)

    def add_bd(self, *args):
        if self._p.ndims == 2:
            if len(args) == 2:
                self.add_bd_2(list(args[:2]))
            else:
                raise Exception('error!')
        elif self._p.ndims == 3:
            if len(args) == 3:
                self.add_bd_3(list(args[:3]))
            else:
                raise Exception('error!')

    cdef add_bd_2(self, list inds):
        cdef int inds_[2]
        inds_[0] = inds[0]
        inds_[1] = inds[1]
        olim_wrapper_add_bd(self._w, inds_)

    cdef add_bd_3(self, list inds):
        cdef int inds_[3]
        inds_[0] = inds[0]
        inds_[1] = inds[1]
        inds_[2] = inds[2]
        olim_wrapper_add_bd(self._w, inds_)

    cpdef set_fac_src(self, inds, FacSrc fs):
        cdef int[::1] mv = self.get_inds_mv(inds)
        olim_wrapper_set_fac_src(self._w, &mv[0], fs._w)
