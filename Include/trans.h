#ifndef  _TRANSFORM_STUFF_H
#define  _TRANSFORM_STUFF_H

#include  <volume_io.h>
#include  <compute_xfm.h>

#ifndef  public
#define       public   extern
#define       public_was_defined_here
#endif

#include  <trans_prototypes.h>

#ifdef  public_was_defined_here
#undef       public
#undef       public_was_defined_here
#endif

#endif
