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
#include  <geom.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/curvature.c,v 1.14 1996-05-17 19:35:23 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_polygon_vertex_curvatures
@INPUT      : polygons
              smoothing_distance
              low_threshold
@OUTPUT     : curvatures
@RETURNS    : 
@DESCRIPTION: Computes the curvatures at each vertex of the polygon, using
              1 of two methods.  If smoothing distance is zero, computes
              instantaneous curvature in terms of a fractional relative
              curvature.  If non-zero, returns +/- angle in degrees of the
              smoothed curvature.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1994    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_polygon_vertex_curvatures(
    polygons_struct   *polygons,
    Real              smoothing_distance,
    Real              low_threshold,
    Real              curvatures[] )
{
    int              size, point_index, vertex_index, poly;
    Real             curvature, base_length;
    Smallest_int     *point_done;
    Point            centroid;
    Vector           normal;
    progress_struct  progress;

    check_polygons_neighbours_computed( polygons );

    ALLOC( point_done, polygons->n_points );

    for_less( point_index, 0, polygons->n_points )
        point_done[point_index] = FALSE;

    initialize_progress_report( &progress, FALSE, polygons->n_items,
                                "Computing Curvatures" );

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );

        for_less( vertex_index, 0, size )
        {
            point_index = polygons->indices[
                POINT_INDEX(polygons->end_indices,poly,vertex_index)];

            if( !point_done[point_index] )
            {
                point_done[point_index] = TRUE;

                if( smoothing_distance <= 0.0 )
                {
                    compute_polygon_point_centroid( polygons, poly,
                          vertex_index, point_index, &centroid,
                          &normal, &base_length, &curvature );
                }
                else
                {
                    curvature = get_smooth_surface_curvature( polygons,
                                       poly, vertex_index, smoothing_distance );
                }

                if( FABS( curvature ) < low_threshold )
                    curvature = 0.0;

                curvatures[point_index] = curvature;
            }
        }

        update_progress_report( &progress, poly + 1 );
    }

    terminate_progress_report( &progress );
}
