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
#include  <objects.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Objects/poly_neighs.c,v 1.14 1996-09-12 15:53:09 david Exp $";
#endif

#define  INVALID_ID       -1

#define  INITIAL_HASH_TABLE_SIZE   2.0   /* times number of polygons */
#define  ENLARGE_THRESHOLD         0.25
#define  NEW_DENSITY               0.125

private   void   create_polygon_neighbours(
    polygons_struct  *polygons,
    int              neighbours[] );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_polygons_neighbours_computed
@INPUT      : polygons
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Creates the polygons neighbours (neighbouring polygon index for
              each polygon edge), if necessary.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  check_polygons_neighbours_computed(
    polygons_struct   *polygons )
{
    if( polygons->neighbours == NULL && polygons->n_items > 0 )
    {
        ALLOC( polygons->neighbours,polygons->end_indices[polygons->n_items-1]);
        create_polygon_neighbours( polygons, polygons->neighbours );
    }
}

public  void  delete_polygon_point_neighbours(
    polygons_struct  *polygons,
    int              n_point_neighbours[],
    int              *point_neighbours[],
    int              *point_polygons[] )
{
    int   i;

    FREE( n_point_neighbours );

    for_less( i, 0, polygons->n_points )
        FREE( point_neighbours[i] );
    FREE( point_neighbours );

    if( point_polygons != NULL )
    {
        FREE( point_polygons[0] );
        FREE( point_polygons );
    }
}

public   void   create_polygon_point_neighbours(
    polygons_struct  *polygons,
    BOOLEAN          across_polygons_flag,
    int              *n_point_neighbours_ptr[],
    int              **point_neighbours_ptr[],
    int              **point_polygons_ptr[] )
{
    int                 edge, i0, i1, size, poly, total_neighbours, p0, p1;
    int                 *n_point_neighbours, **point_neighbours;
    int                 **point_polygons, point, index0, index1;
    int                 i, v, indices[MAX_POINTS_PER_POLYGON], position, ii;
    int                 n_to_add, points_to_add[MAX_POINTS_PER_POLYGON];
    BOOLEAN             done;
    progress_struct     progress;

    if( across_polygons_flag && point_polygons_ptr != NULL )
    {
        print_error(
                "create_polygon_point_neighbours: conflicting argument.\n" );
        return;
    }

    ALLOC( n_point_neighbours, polygons->n_points );
    for_less( point, 0, polygons->n_points )
        n_point_neighbours[point] = 0;

    ALLOC( point_neighbours, polygons->n_points );

    total_neighbours = 0;

    do
    {
        done = TRUE;

        initialize_progress_report( &progress, FALSE, polygons->n_items,
                                    "Neighbour-finding" );

        for_less( poly, 0, polygons->n_items )
        {
            size = GET_OBJECT_SIZE( *polygons, poly );
            for_less( v, 0, size )
            {
                indices[v] = polygons->indices[
                           POINT_INDEX(polygons->end_indices,poly,v)];
            }

            for_less( i0, 0, size )
            {
                p0 = indices[i0];
                n_to_add = 0;
                for_less( ii, 0, size-1 )
                {
                    i1 = (i0 + 1 + ii) % size;
                    if( !across_polygons_flag && ii != 0 && ii != size-2 )
                        continue;

                    p1 = indices[i1];
                    points_to_add[n_to_add] = p1;
                    ++n_to_add;
                }

                for_less( i1, 0, n_to_add )
                {
                    p1 = points_to_add[i1];

                    for_less( i, 0, n_point_neighbours[p0] )
                    {
                        if( point_neighbours[p0][i] == p1 )
                            break;
                    }

                    if( i < n_point_neighbours[p0] )
                        continue;

                    for_less( position, 0, n_point_neighbours[p0] )
                    {
                        if( point_neighbours[p0][position] ==
                                 points_to_add[(i1-1+n_to_add)%n_to_add] )
                            break;
                    }

                    if( position < n_point_neighbours[p0] )
                    {
                        ++position;
                    }
                    else
                    {
                        for_less( position, 0, n_point_neighbours[p0] )
                        {
                            if( point_neighbours[p0][position] ==
                                     points_to_add[(i1+1+n_to_add)%n_to_add] )
                                break;
                        }

                        if( position >= n_point_neighbours[p0] )
                        {
                            if( n_point_neighbours[p0] == 0 )
                                position = 0;
                            else
                                position = -1;
                        }
                    }

                    if( position >= 0 )
                    {
                        SET_ARRAY_SIZE( point_neighbours[p0],
                                        n_point_neighbours[p0],
                                        n_point_neighbours[p0]+1, 5 );
                        for_down( i, n_point_neighbours[p0], position+1 )
                        {
                            point_neighbours[p0][i] = 
                                              point_neighbours[p0][i-1];
                        }
                        point_neighbours[p0][position] = p1;
                        ++n_point_neighbours[p0];
                        ++total_neighbours;
                        continue;
                    }
                    else
                        done = FALSE;
                }
            }

            update_progress_report( &progress, poly+1 );
        }
    }
    while( !done );

    terminate_progress_report( &progress );

    for_less( i, 0, polygons->n_points )
        REALLOC( point_neighbours[i], n_point_neighbours[i] );

    *n_point_neighbours_ptr = n_point_neighbours;   
    *point_neighbours_ptr = point_neighbours;   

    if( point_polygons_ptr == NULL )
        return;

    ALLOC( point_polygons, polygons->n_points );
    ALLOC( point_polygons[0], total_neighbours );

    for_less( point, 0, total_neighbours )
        point_polygons[0][point] = -1;

    for_less( point, 1, polygons->n_points )
    {
        point_polygons[point] = &point_polygons[point-1]
                                               [n_point_neighbours[point-1]];
    }

    initialize_progress_report( &progress, FALSE, polygons->n_items,
                                "Neighbour-finding step 2" );

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );

        for_less( edge, 0, size )
        {
            i0 = polygons->indices[
                  POINT_INDEX(polygons->end_indices,poly,edge)];
            i1 = polygons->indices[
                  POINT_INDEX(polygons->end_indices,poly,(edge+1)%size)];

            for_less( index0, 0, n_point_neighbours[i0] )
            {
                if( point_neighbours[i0][index0] == i1 )
                    break;
            }

            for_less( index1, 0, n_point_neighbours[i1] )
            {
                if( point_neighbours[i1][index1] == i0 )
                    break;
            }

            if( point_polygons[i0][index0] < 0 )
                point_polygons[i0][index0] = poly;
            else
                point_polygons[i1][index1] = poly;
        }

        update_progress_report( &progress, poly+1 );
    }

    terminate_progress_report( &progress );

    *point_polygons_ptr = point_polygons;   
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_polygon_neighbours
@INPUT      : n_polygons
              indices
              end_indices
@OUTPUT     : neighbours
@RETURNS    : 
@DESCRIPTION: Computes the neighbours for each edge of the polygons
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private   void   create_polygon_neighbours(
    polygons_struct  *polygons,
    int              neighbours[] )
{
    int                 i0, i1, size1, size2, n1, n2;
    int                 poly1, poly2, point1, point2, edge1, edge2;
    int                 *n_point_neighbours, **point_neighbours;
    int                 **point_polygons;
    progress_struct     progress;

    for_less( i0, 0, polygons->end_indices[polygons->n_items-1] )
        neighbours[i0] = -1;

    create_polygon_point_neighbours( polygons, FALSE, &n_point_neighbours,
                                     &point_neighbours, &point_polygons );

    initialize_progress_report( &progress, FALSE, polygons->n_items,
                                "Neighbour-finding step 2" );

    for_less( point1, 0, polygons->n_points )
    {
        for_less( n1, 0, n_point_neighbours[point1] )
        {
            point2 = point_neighbours[point1][n1];

            if( point2 <= point1 )
                continue;

            poly1 = point_polygons[point1][n1];
            if( poly1 < 0 )
                continue;

            for_less( n2, 0, n_point_neighbours[point2] )
            {
                if( point_neighbours[point2][n2] == point1 )
                    break;
            }

            if( n2 >= n_point_neighbours[point2] )
                handle_internal_error( "create_polygon_neighbours" );

            poly2 = point_polygons[point2][n2];

            if( poly2 < 0 )
                continue;

            size1 = GET_OBJECT_SIZE( *polygons, poly1 );
            for_less( edge1, 0, size1 )
            {
                i0 = polygons->indices[POINT_INDEX(polygons->end_indices,
                                                   poly1,edge1)];
                i1 = polygons->indices[POINT_INDEX(polygons->end_indices,
                                                   poly1,(edge1+1)%size1)];

                if( i0 == point1 && i1 == point2 ||
                    i1 == point1 && i0 == point2 )
                    break;
            }

            if( edge1 >= size1 )
                handle_internal_error( "create_polygon_neighbours" );

            size2 = GET_OBJECT_SIZE( *polygons, poly2 );
            for_less( edge2, 0, size2 )
            {
                i0 = polygons->indices[POINT_INDEX(polygons->end_indices,
                                                   poly2,edge2)];
                i1 = polygons->indices[POINT_INDEX(polygons->end_indices,
                                                   poly2,(edge2+1)%size2)];

                if( i0 == point1 && i1 == point2 ||
                    i1 == point1 && i0 == point2 )
                    break;
            }

            if( edge2 >= size2 )
                handle_internal_error( "create_polygon_neighbours" );

            neighbours[POINT_INDEX( polygons->end_indices, poly1, edge1 )] =
                                                               poly2;
            neighbours[POINT_INDEX( polygons->end_indices, poly2, edge2 )] =
                                                               poly1;
        }

        update_progress_report( &progress, point1+1 );
    }

    terminate_progress_report( &progress );

    delete_polygon_point_neighbours( polygons,
                                     n_point_neighbours, point_neighbours,
                                     point_polygons );

#ifdef  DEBUG
    for_less( i0, 0, polygons->end_indices[polygons->n_items-1] )
    {
        if( neighbours[i0] < 0 )
            break;
    }

    if( i0 < polygons->end_indices[polygons->n_items-1] )
        handle_internal_error( "create_polygon_neighbours: topology\n" );
#endif
}
