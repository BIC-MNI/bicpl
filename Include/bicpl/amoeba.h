#ifndef  _DEF_AMOEBA_H
#define  _DEF_AMOEBA_H

/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include  <volume_io.h>


typedef  VIO_Real    (*amoeba_function) ( void *, float [] );

typedef  struct
{
    int               n_parameters;
    float             **parameters;
    VIO_Real              *values;
    amoeba_function   function;
    void              *function_data;
    VIO_Real              tolerance;
    VIO_Real              *sum;
    int               n_steps_no_improvement;
} amoeba_struct;

#endif
