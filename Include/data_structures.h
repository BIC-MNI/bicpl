#ifndef  _DATA_STRUCTURES_H
#define  _DATA_STRUCTURES_H

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
static char data_structures_rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Include/Attic/data_structures.h,v 1.5 2000-01-19 22:59:50 stever Exp $";
#endif

#include  <bicpl/objects.h>
#include  <volume_io.h>
#include  <bicpl/bitlist.h>
#include  <bicpl/hash.h>
#include  <bicpl/priority_queue.h>
#include  <bicpl/queue.h>
#include  <bicpl/skiplist.h>
#include  <bicpl/stack.h>

#ifndef  public
#define       public   extern
#define       public_was_defined_here
#endif

#include  <bicpl/ds_prototypes.h>

#ifdef  public_was_defined_here
#undef       public
#undef       public_was_defined_here
#endif

#endif
