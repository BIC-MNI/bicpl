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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/geodesic_distance.c,v 1.1 1996-09-10 17:18:05 david Exp $";
#endif

#include  <internal_volume_io.h>
#include  <geom.h>
#include  <priority_queue.h>

public  void  compute_distances_from_point(
    polygons_struct   *polygons,
    int               n_neighbours[],
    int               *neighbours[],
    Point             *point,
    int               poly,
    Real              max_distance,
    float             distances[] ) 
{
    int                    i, p, size, point_index, next_point_index, neigh;
    Real                   dist;
    PRIORITY_QUEUE_STRUCT( int )   queue;

    if( poly == -1 )
    {
        if( !lookup_polygon_vertex( polygons, point, &point_index ) ||
            !find_polygon_with_vertex( polygons, point_index, &poly, &p ) )
        {
            print_error( "compute_distances_from_point incorrect arguments.\n");
            return;
        }
    }

    for_less( i, 0, polygons->n_points )
        distances[i] = -1.0f;

    INITIALIZE_PRIORITY_QUEUE( queue );

    size = GET_OBJECT_SIZE( *polygons, poly );

    for_less( p, 0, size )
    {
        point_index = polygons->indices[
                            POINT_INDEX( polygons->end_indices, poly, p )];

        dist = distance_between_points( &polygons->points[point_index], point );

        if( max_distance <= 0.0 || dist < max_distance )
        {
            distances[point_index] = (float) dist;
            INSERT_IN_PRIORITY_QUEUE( queue, point_index, -dist );
        }
    }

    while( !IS_PRIORITY_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_PRIORITY_QUEUE( queue, point_index, dist );

        if( max_distance > 0.0 && (Real) distances[point_index] > max_distance )
            break;

        for_less( neigh, 0, n_neighbours[point_index] )
        {
            next_point_index = neighbours[point_index][neigh];

            if( distances[next_point_index] < 0.0f ||
                distances[next_point_index] > distances[point_index] )
            {
                dist = (Real) distances[point_index] +
                       distance_between_points( &polygons->points[point_index],
                                         &polygons->points[next_point_index] );

                if( distances[next_point_index] < 0.0f ||
                    (float) dist < distances[next_point_index] )
                {
                    distances[next_point_index] = (float) dist;
                    INSERT_IN_PRIORITY_QUEUE( queue, next_point_index, -dist );
                }
            }
        }
    }

    DELETE_PRIORITY_QUEUE( queue );
}
