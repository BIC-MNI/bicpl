#ifndef  _OBJECTS_H
#define  _OBJECTS_H

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

#ifndef lint
static char objects_rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Include/Attic/objects.h,v 1.14 1995-07-31 13:44:43 david Exp $";
#endif

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
