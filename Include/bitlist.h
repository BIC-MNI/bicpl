#ifndef  DEF_BITLIST
#define  DEF_BITLIST

/* ----------------------------- MNI Header -----------------------------------
@NAME       : def_bitlist.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A header file for routines that manipulate bitlists, which
            : are simply arrays of individually addressable bits.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <def_mni.h>

typedef  unsigned int   bitlist_type;

#define  BITS_PER_BITLIST_WORD               32
#define  BITS_PER_BITLIST_WORD_MINUS_1       31
#define  LOG_BITS_PER_BITLIST_WORD            5

typedef  struct
{
    int            n_words;
    bitlist_type   *bits;
}  bitlist_struct;

typedef  struct
{
    int            nx, ny, nz;
    bitlist_type   ***bits;
}  bitlist_3d_struct;

#endif
