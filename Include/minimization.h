#ifndef  DEF_MINIMIZATION
#define  DEF_MINIMIZATION

#include  <def_geometry.h>

typedef struct
{
    void            *evaluation_ptr;
    double          (*evaluate_fit_function)();
    double          **p;
    double          *y;
    double          *psum;
    double          *ptry;
} downhill_simplex_struct;

typedef  enum  { DOWNHILL_SIMPLEX, ONE_PARAMETER_MINIMIZATION }
               Minimization_methods;

#endif
