#include "bicpl_internal.h"

#define  STATIC_STORAGE  1000

BICAPI  int  clip_polygon_against_box(
    int     n_points,
    VIO_Point   points[],
    VIO_Real    x_min,
    VIO_Real    x_max,
    VIO_Real    y_min,
    VIO_Real    y_max,
    VIO_Real    z_min,
    VIO_Real    z_max,
    int     n_output_points,
    VIO_Point   output_points[] )
{
    int     p, n_planes, n_out, n, i;
    int     n_input[7], dim, which;
    VIO_BOOL last[7][2], first_flags[6], last_flag;
    VIO_Point   prev[6], input[7][2], point, first_points[6];
    VIO_Real    box[2][VIO_N_DIMENSIONS], prev_dist[6], first_dist[6], dist, ratio;

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
		    if ( n_out >= n_output_points )
		    {
			handle_internal_error( "too many points generated in clip_polygon_against_box()" );
			return( n_out );
		    }
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

BICAPI  int  clip_polygon_against_plane(
    int     n_points,
    VIO_Point   points[],
    VIO_Real    plane_constant,
    VIO_Vector  *normal,
    VIO_Point   output_points[] )
{
    int     p, n_output;
    VIO_Real    next_dist, dist, ratio;
    VIO_Point   point, next_point;

    n_output = 0;
    dist = 0.0;

    next_dist = DOT_POINT_VECTOR( *normal, points[0] ) + plane_constant;
    next_point = points[0];

    for_less( p, 0, n_points )
    {
        dist = next_dist;
        point = next_point;
        next_point = points[(p+1) % n_points];
        next_dist = DOT_POINT_VECTOR( *normal, next_point ) + plane_constant;

        if( dist >= 0.0 )
        {
            output_points[n_output] = point;
            ++n_output;
        }

        if( (n_points > 2 || (p == 0 && n_points == 2))
            && dist * next_dist < 0.0 )
        {
            ratio = dist / (dist - next_dist);
            INTERPOLATE_POINTS( output_points[n_output],
                                point, next_point, ratio );
            ++n_output;
        }
    }

    return( n_output );
}

BICAPI  void  split_polygon_with_plane(
    int     n_points,
    VIO_Point   points[],
    VIO_Real    plane_constant,
    VIO_Vector  *normal,
    int     *n_in,
    VIO_Point   in_points[],
    int     *n_out,
    VIO_Point   out_points[] )
{
    int     p;
    VIO_Real    next_dist, dist, ratio;
    VIO_Point   point, next_point, interp;

    *n_in = 0;
    *n_out = 0;
    dist = 0.0;

    next_dist = DOT_POINT_VECTOR( *normal, points[0] ) + plane_constant;
    next_point = points[0];

    for_less( p, 0, n_points )
    {
        dist = next_dist;
        point = next_point;
        next_point = points[(p+1) % n_points];
        next_dist = DOT_POINT_VECTOR( *normal, next_point ) + plane_constant;

        if( dist >= 0.0 )
        {
            in_points[*n_in] = point;
            ++(*n_in);
        }

        if( dist <= 0.0 )
        {
            out_points[*n_out] = point;
            ++(*n_out);
        }

        if( (n_points > 2 || (p == 0 && n_points == 2))
            && dist * next_dist < 0.0 )
        {
            ratio = dist / (dist - next_dist);
            INTERPOLATE_POINTS( interp, point, next_point, ratio );
            in_points[*n_in] = interp;
            ++(*n_in);
            out_points[*n_out] = interp;
            ++(*n_out);
        }
    }
}
