
#include  <def_mni.h>
#include  <def_bitlist.h>
#include  <def_alloc.h>
#include  <def_files.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_bitlist
@INPUT      : n_bits
@OUTPUT     : bitlist
@RETURNS    : Status
@DESCRIPTION: Creates a bitlist containing the required number of bits.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  create_bitlist( n_bits, bitlist )
    int             n_bits;
    bitlist_struct  *bitlist;
{
    Status   status;
    void     zero_bitlist();

    bitlist->n_words = (n_bits + BITS_PER_BITLIST_WORD - 1) /
                        (BITS_PER_BITLIST_WORD);

    status = OK;

    if( bitlist->n_words > 0 )
        ALLOC( status, bitlist->bits, bitlist->n_words );

    if( status == OK )
        zero_bitlist( bitlist );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : zero_bitlist
@INPUT      : bitlist
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets all bits to zero (FALSE).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  zero_bitlist( bitlist )
    bitlist_struct  *bitlist;
{
    int   i;

    for_less( i, 0, bitlist->n_words )
        bitlist->bits[i] = (bitlist_type) 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fill_bitlist
@INPUT      : bitlist
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets all bits to 1 (TRUE).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  fill_bitlist( bitlist )
    bitlist_struct  *bitlist;
{
    int   i;

    for_less( i, 0, bitlist->n_words )
        bitlist->bits[i] = (bitlist_type) -1;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_bitlist_bit
@INPUT      : bitlist
            : bit_index
            : value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the bit_index'th bit to the given value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void     set_bitlist_bit( bitlist, bit_index, value )
    bitlist_struct  *bitlist;
    int             bit_index;
    Boolean         value;
{
    int           word_index, bit_sub_index;
    bitlist_type  bit;

    word_index = bit_index >> LOG_BITS_PER_BITLIST_WORD;
    bit_sub_index = bit_index & BITS_PER_BITLIST_WORD_MINUS_1;

    bit = (bitlist_type) 1 << (bitlist_type) bit_sub_index;

    if( value )                                    /* setting bit to 1 */
        bitlist->bits[word_index] |= bit;
    else if( bitlist->bits[word_index] & bit )     /* setting bit to 0 */
            bitlist->bits[word_index] ^= bit;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_bitlist_bit
@INPUT      : bitlist
            : bit_index
@OUTPUT     : 
@RETURNS    : TRUE or FALSE
@DESCRIPTION: Returns the bit_index'th bit.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean     get_bitlist_bit( bitlist, bit_index )
    bitlist_struct  *bitlist;
    int             bit_index;
{
    int           word_index, bit_sub_index;
    bitlist_type  word_bits;
    Boolean       value;

    word_index = bit_index >> LOG_BITS_PER_BITLIST_WORD;
    bit_sub_index = bit_index & BITS_PER_BITLIST_WORD_MINUS_1;

    word_bits = bitlist->bits[word_index];

    value = ((word_bits >> bit_sub_index) & 1);

    return( value );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_bitlist
@INPUT      : bitlist
@OUTPUT     : 
@RETURNS    : Status
@DESCRIPTION: Deletes the given bitlist.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  delete_bitlist( bitlist )
    bitlist_struct  *bitlist;
{
    Status   status;

    status = OK;

    if( bitlist->n_words > 0 )
    {
        FREE( status, bitlist->bits );
    }

    return( status );
}

/* ------------------------ bitlist 3d ------------------------------- */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_bitlist_3d
@INPUT      : nx
            : ny
            : nz
@OUTPUT     : bitlist
@RETURNS    : Status
@DESCRIPTION: Creates a 3D bitlist.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  create_bitlist_3d( nx, ny, nz, bitlist )
    int                nx, ny, nz;
    bitlist_3d_struct  *bitlist;
{
    Status   status;
    void     zero_bitlist_3d();

    nz = (nz + BITS_PER_BITLIST_WORD - 1) / BITS_PER_BITLIST_WORD;

    bitlist->nx = nx;
    bitlist->ny = ny;
    bitlist->nz = nz;

    status = OK;

    if( nx > 0 && ny > 0 && nz > 0 )
        ALLOC3D( status, bitlist->bits, nx, ny, nz );

    if( status == OK )
        zero_bitlist_3d( bitlist );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : zero_bitlist_3d
@INPUT      : bitlist
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets all bits to 0 (FALSE).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  zero_bitlist_3d( bitlist )
    bitlist_3d_struct  *bitlist;
{
    int   x, y, z;

    for_less( x, 0, bitlist->nx )
    {
        for_less( y, 0, bitlist->ny )
        {
            for_less( z, 0, bitlist->nz )
            {
                bitlist->bits[x][y][z] = (bitlist_type) 0;
            }
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fill_bitlist_3d
@INPUT      : bitlist
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets all bits to 1 (TRUE).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  fill_bitlist_3d( bitlist )
    bitlist_3d_struct  *bitlist;
{
    int   x, y, z;

    for_less( x, 0, bitlist->nx )
    {
        for_less( y, 0, bitlist->ny )
        {
            for_less( z, 0, bitlist->nz )
            {
                bitlist->bits[x][y][z] = (bitlist_type) -1;
            }
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_bitlist_bit_3d
@INPUT      : bitlist
            : x
            : y
            : z
            : value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the bit indicated by x y z to the given value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void     set_bitlist_bit_3d( bitlist, x, y, z, value )
    bitlist_3d_struct  *bitlist;
    int                x, y, z;
    Boolean            value;
{
    int           word_index, bit_sub_index;
    bitlist_type  bit;

    word_index = z >> LOG_BITS_PER_BITLIST_WORD;
    bit_sub_index = z & BITS_PER_BITLIST_WORD_MINUS_1;

    bit = (bitlist_type) 1 << (bitlist_type) bit_sub_index;

    if( value )
    {
        bitlist->bits[x][y][word_index] |= bit;
    }
    else
    {
        if( bitlist->bits[x][y][word_index] & bit )
        {
            bitlist->bits[x][y][word_index] ^= bit;
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_bitlist_bit_3d
@INPUT      : bitlist
            : x
            : y
            : z
@OUTPUT     : 
@RETURNS    : TRUE or FALSE
@DESCRIPTION: Returns the bit indicated by x, y, z.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean     get_bitlist_bit_3d( bitlist, x, y, z )
    bitlist_3d_struct  *bitlist;
    int                x, y, z;
{
    int           word_index, bit_sub_index;
    bitlist_type  word;

    bit_sub_index = z & BITS_PER_BITLIST_WORD_MINUS_1;

    word_index = z >> LOG_BITS_PER_BITLIST_WORD;

    word = bitlist->bits[x][y][word_index];

    if( word == 0 )
        return( FALSE );
    else
        return( (word >> bit_sub_index) & 1 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_bitlist_3d
@INPUT      : bitlist
@OUTPUT     : 
@RETURNS    : Status
@DESCRIPTION: Deletes the 3D bitlist.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  delete_bitlist_3d( bitlist )
    bitlist_3d_struct  *bitlist;
{
    Status   status;

    status = OK;

    if( bitlist->nx > 0 && bitlist->ny > 0 && bitlist->nz > 0 )
    {
        FREE3D( status, bitlist->bits );
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : io_bitlist_3d
@INPUT      : file
            : io_type         READ_FILE or WRITE_FILE 
            : bitlist
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Inputs or outputs the 3D bitlist.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  io_bitlist_3d( file, io_type, bitlist )
    FILE               *file;
    IO_types           io_type;
    bitlist_3d_struct  *bitlist;
{
    Status  status;
    Status  io_binary_data();
    int     x, y;

    status = OK;

    for_less( x, 0, bitlist->nx )
    {
        for_less( y, 0, bitlist->ny )
        {
            status = io_binary_data( file, io_type,
                                     (VOID *) (bitlist->bits[x][y]),
                                     sizeof( bitlist->bits[x][y] ),
                                     bitlist->nz );
       
            if( status != OK )
                break;
        }
        if( status != OK )
            break;
    }

    return( status );
}
