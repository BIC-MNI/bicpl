#ifndef  _DEF_AMOEBA_H
#define  _DEF_AMOEBA_H

#include  <mni.h>

typedef  Real    (*amoeba_function) ( void *, float [] );

typedef  struct
{
    int               n_parameters;
    float             **parameters;
    float             *values;
    amoeba_function   function;
    void              *function_data;
    Real              tolerance;
} amoeba_struct;

#endif
