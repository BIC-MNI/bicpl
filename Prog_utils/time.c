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

#include  <internal_volume_io.h>
#include  <bicpl.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Prog_utils/time.c,v 1.3 1995-07-31 13:45:56 david Exp $";
#endif

private  Real  cpu_start_time;
private  Real  real_start_time;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : start_timing
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Starts timing a subset of a process.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  start_timing()
{
    cpu_start_time = current_cpu_seconds();
    real_start_time = current_realtime_seconds();
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : end_timing
@INPUT      : title
              n     - number of iterations of job that were performed.
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Computes the cpu and real time since the start time,
              prints this out divided by the number of iterations performed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  end_timing(
    char   title[],
    int    n )
{
    Real  cpu_end_time, real_end_time, per_unit_real_time, per_unit_cpu_time;

    if( n < 1 )
        n = 1;

    cpu_end_time = current_cpu_seconds();
    real_end_time = current_realtime_seconds();

    per_unit_cpu_time = (cpu_end_time - cpu_start_time) / (Real) n;
    per_unit_real_time = (real_end_time - real_start_time) / (Real) n;

    if( title != NULL )
        print( "%s: ", title );
    print( "per cycle: " );
    print_time( "Cpu:  %g %s\t", per_unit_cpu_time );
    print_time( "Real:  %g %s\n", per_unit_real_time );
}
