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
    Point   prev[6], input[7][2], point;
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
            point = input[p][0];
            last_flag = last[p][0];
            if( n_input[p] == 1 )
                n_input[p] = 0;
            else
            {
                input[p][0] = input[p][1];
                last[p][0] = last[p][1];
                n_input[p] = 1;
            }

            if( !last_flag )
            {
                dim = p / 2;
                which = p % 2;
                if( which == 0 )
                    dist = RPoint_coord(point,dim) - box[0][dim];
                else
                    dist = box[1][dim] - RPoint_coord(point,dim);
            }
            else
                dist = first_dist[p];

            if( first_flags[p] )
            {
                first_flags[p] = FALSE;
                first_dist[p] = dist;
            }
            else if( prev_dist[p] * dist < 0.0 )
            {
                ratio = prev_dist[p] / (prev_dist[p] - dist);
                INTERPOLATE_POINTS( input[p+1][n_input[p+1]], prev[p],
                                    point, ratio );
                last[p+1][n_input[p+1]] = FALSE;
                ++n_input[p+1];
            }

            if( dist >= 0.0 && (!last_flag || p < n_planes-1) )
            {
                input[p+1][n_input[p+1]] = point;
                last[p+1][n_input[p+1]] = last_flag;
                ++n_input[p+1];
            }

            prev[p] = point;
            prev_dist[p] = dist;

            ++p;

            if( p == n_planes && n_input[p] > 0 )
            {
                for_less( n, 0, n_input[p] )
                    output_points[n_out+n] = input[p][n];
                n_out += n_input[p];
                n_input[p] = 0;
                --p;
            }

            while( p >= 0 && n_input[p] == 0 )
                --p;
        }
    }

    return( n_out );
}
