#include  <internal_volume_io.h>
#include  <geom.h>

#define  STATIC_STORAGE  1000

public  int  clip_polygon_against_box(
    int     n_points,
    Point   points[],
    Real    x_min,
    Real    x_max,
    Real    y_min,
    Real    y_max,
    Real    z_min,
    Real    z_max,
    Point   output_points[] )
{
    int     p, n_planes, n_out, n, i;
    int     n_input[7], dim, which;
    BOOLEAN last[7][2], first_flags[6], last_flag;
    Point   prev[6], input[7][2], point, first_points[6];
    Real    box[2][N_DIMENSIONS], prev_dist[6], first_dist[6], dist, ratio;

    box[0][X] = x_min;
    box[1][X] = x_max;
    box[0][Y] = y_min;
    box[1][Y] = y_max;
    box[0][Z] = z_min;
    box[1][Z] = z_max;
    n_planes = 6;

    for_less( p, 0, n_planes )
    {
        first_flags[p] = TRUE;
        n_input[p] = 0;
    }
    n_input[n_planes] = 0;

    n_out = 0;

    for_less( i, 0, n_points+1 )
    {
        input[0][0] = points[i%n_points];
        last[0][0] = (i == n_points);
        n_input[0] = 1;
        p = 0;

        while( p >= 0 )
        {
            --n_input[p];
            point = input[p][n_input[p]];
            last_flag = last[p][n_input[p]];

            if( !last_flag )
            {
                dim = p / 2;
                which = p % 2;
                if( which == 0 )
                    dist = RPoint_coord(point,dim) - box[0][dim];
                else
                    dist = box[1][dim] - RPoint_coord(point,dim);

                if( dist >= 0.0 )
                {
                    input[p+1][0] = point;
                    last[p+1][0] = FALSE;
                    n_input[p+1] = 1;
                }
            }
            else
            {
                dist = first_dist[p];
                point = first_points[p];

                if( !first_flags[p] && p < n_planes - 1 )
                {
                    last[p+1][0] = TRUE;
                    n_input[p+1] = 1;
                }
            }

            if( first_flags[p] )
            {
                first_points[p] = point;
                first_flags[p] = FALSE;
                first_dist[p] = dist;
            }
            else if( prev_dist[p] * dist < 0.0 )
            {
                ratio = prev_dist[p] / (prev_dist[p] - dist);
                n = n_input[p+1];
                INTERPOLATE_POINTS( input[p+1][n], prev[p], point, ratio );
                last[p+1][n] = FALSE;
                ++n_input[p+1];
            }

            prev[p] = point;
            prev_dist[p] = dist;

            ++p;

            if( p == n_planes && n_input[p] > 0 )
            {
                n = n_input[p];
                for_down( n, n_input[p]-1, 0 )
                {
                    output_points[n_out] = input[p][n];
                    ++n_out;
                }
                n_input[p] = 0;
                --p;
            }

            while( p >= 0 && n_input[p] == 0 )
                --p;
        }
    }

    return( n_out );
}
