#include  <internal_volume_io.h>
#include  <bicpl.h>

private  Real  cpu_start_time;
private  Real  real_start_time;

public  void  start_timing()
{
    cpu_start_time = current_cpu_seconds();
    real_start_time = current_realtime_seconds();
}

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
