#ifndef  _PROGRAMMING_UTILS_H
#define  _PROGRAMMING_UTILS_H

#include  <volume_io.h>
#include  <global_lookup.h>

#ifndef  public
#define       public   extern
#define       public_was_defined_here
#endif

#include  <prog_prototypes.h>

#ifdef  public_was_defined_here
#undef       public
#undef       public_was_defined_here
#endif

#endif
