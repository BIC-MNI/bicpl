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

Status   create_bitlist();
void     zero_bitlist();
void     fill_bitlist();
void     set_bitlist_bit();
Boolean  get_bitlist_bit();
Status   delete_bitlist();

typedef  struct
{
    int            nx, ny, nz;
    bitlist_type   ***bits;
}  bitlist_3d_struct;

Status   create_bitlist_3d();
void     zero_bitlist_3d();
void     fill_bitlist_3d();
void     set_bitlist_bit_3d();
Boolean  get_bitlist_bit_3d();
Status   delete_bitlist_3d();
Status   io_bitlist_3d();

#endif
