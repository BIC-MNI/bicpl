#ifndef  _OBJECTS_H
#define  _OBJECTS_H

#include  <volume_io.h>
#include  <obj_defs.h>
#include  <colours.h>
#include  <data_structures.h>

#ifndef  public
#define       public   extern
#define       public_was_defined_here
#endif

#include  <object_prototypes.h>

#ifdef  public_was_defined_here
#undef       public
#undef       public_was_defined_here
#endif

#endif
