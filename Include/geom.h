#ifndef  _GEOM_STUFF_H
#define  _GEOM_STUFF_H

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
static char geom_rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Include/Attic/geom.h,v 1.3 1995-07-31 13:44:50 david Exp $";
#endif

#include  <volume_io.h>
#include  <objects.h>

#ifndef  public
#define       public   extern
#define       public_was_defined_here
#endif

#include  <geom_prototypes.h>

#ifdef  public_was_defined_here
#undef       public
#undef       public_was_defined_here
#endif

#endif
