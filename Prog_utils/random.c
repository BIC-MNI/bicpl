
#include  <internal_volume_io.h>
#include  <prog_utils.h>
#include  <sys/time.h>

#define  MAX_RAND  2147483648.0

private  BOOLEAN  initialized = FALSE;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_random_seed
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the random seed for the generator
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_random_seed( int seed )
{
#ifndef sgi
    int  srandom();
#endif
    (void) srandom( seed );
    initialized = TRUE;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_initialized
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Ensures that the random seed has been set.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  check_initialized( void )
{
    struct   timeval   t;
    int                seed;

    if( !initialized )
    {
        (void) gettimeofday( &t, (struct timezone *) 0 );

        seed = (int) t.tv_usec;

        set_random_seed( seed );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_random
@INPUT      : 
@OUTPUT     : 
@RETURNS    : int
@DESCRIPTION: Gets a random number from 0 to 2^30
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  int  get_random( void )
{
    check_initialized();

#ifdef sgi
    return( random() );
#else
    {
        long random();

        return( (int) random() );
    }
#endif
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_random_int
@INPUT      : n
@OUTPUT     : 
@RETURNS    : int
@DESCRIPTION: Returns a random number from 0 to n-1.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_random_int( int n )
{
    return( get_random() % n );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_random_0_to_1
@INPUT      : 
@OUTPUT     : 
@RETURNS    : random number
@DESCRIPTION: Returns a random number greater than or equal to 0 and and less
            : than 1.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  get_random_0_to_1( void )
{
    return( (Real) get_random() / MAX_RAND );
}
