#ifndef  _NUMERICAL_H
#define  _NUMERICAL_H

#include  <volume_io.h>
#include  <objects.h>
#include  <amoeba.h>
#include  <histogram.h>
#include  <minimization.h>

typedef  struct
{
    int   n_parameters;
    Real  **second_derivs;
    Real  *constants;
} linear_least_squares;

#ifndef  public
#define       public   extern
#define       public_was_defined_here
#endif

#include  <numeric_prototypes.h>

#ifdef  public_was_defined_here
#undef       public
#undef       public_was_defined_here
#endif

#endif
