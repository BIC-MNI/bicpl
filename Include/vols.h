#ifndef  _VOLUME_STUFF_H
#define  _VOLUME_STUFF_H

#include  <volume_io.h>
#include  <colour_coding.h>

typedef  enum  { FOUR_NEIGHBOURS, EIGHT_NEIGHBOURS } Neighbour_types;

#ifndef  public
#define       public   extern
#define       public_was_defined_here
#endif

#include  <vol_prototypes.h>

#ifdef  public_was_defined_here
#undef       public
#undef       public_was_defined_here
#endif

#endif
