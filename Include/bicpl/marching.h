#ifndef  DEF_MARCHING_STUFF
#define  DEF_MARCHING_STUFF

#include  <bicpl.h>
#include  <bicpl/marching_cubes.h>

#ifndef  public
#define       public   extern
#define       public_was_defined_here
#endif

#include  <bicpl/march_prototypes.h>

#ifdef  public_was_defined_here
#undef       public
#undef       public_was_defined_here
#endif

#endif
