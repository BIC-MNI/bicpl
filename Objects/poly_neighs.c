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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Objects/poly_neighs.c,v 1.11 1996-05-17 19:35:35 david Exp $";
#endif

#define  INVALID_ID       -1

#define  INITIAL_HASH_TABLE_SIZE   2.0   /* times number of polygons */
#define  ENLARGE_THRESHOLD         0.25
#define  NEW_DENSITY               0.125

private  void  assign_neighbours(
    int       indices[],
    int       end_indices[],
    int       neighbours[],
    int       polygon1,
    int       edge1,
    int       polygon2,
    BOOLEAN   *topology_error );
private   void   create_polygon_neighbours(
    int    n_points,
    int    n_polygons,
    int    indices[],
    int    end_indices[],
    int    neighbours[] );

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
        create_polygon_neighbours( polygons->n_points,
                                   polygons->n_items, polygons->indices,
                                   polygons->end_indices,
                                   polygons->neighbours );
    }
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
    int    n_points,
    int    n_polygons,
    int    indices[],
    int    end_indices[],
    int    neighbours[] )
{
    int                 keys[2], i, edge, i0, i1, size;
    int                 start_index, end_index, polygon_index;
    BOOLEAN             topology_error;
    hash_table_struct   edge_table;
    progress_struct     progress;

    topology_error = FALSE;

    if( n_polygons > 0 )
    {
        for_less( i, 0, end_indices[n_polygons-1] )
            neighbours[i] = INVALID_ID;
    }

    initialize_hash_table( &edge_table,
                           (int) (INITIAL_HASH_TABLE_SIZE* (Real) n_polygons),
                           sizeof(int), ENLARGE_THRESHOLD, NEW_DENSITY );

    end_index = 0;

    initialize_progress_report( &progress, FALSE, n_polygons,
                                "Neighbour-finding" );

    for_less( i, 0, n_polygons )
    {
        start_index = end_index;
        end_index = end_indices[i];

        size = end_index - start_index;

        for_less( edge, 0, size )
        {
            i0 = indices[start_index+edge];
            i1 = indices[start_index+(edge+1) % size];

            keys[0] = MIN( i0, i1 );
            keys[1] = MAX( i0, i1 );

            if( remove_from_hash_table( &edge_table,
                                        IJ(keys[0],keys[1],n_points),
                                        &polygon_index ) )
            {
                assign_neighbours( indices, end_indices, neighbours,
                                   i, edge, polygon_index,
                                   &topology_error );
            }
            else
            {
                insert_in_hash_table( &edge_table,
                                      IJ(keys[0],keys[1],n_points),
                                      (void *) &i );
            }
        }

        update_progress_report( &progress, i+1 );
    }

    terminate_progress_report( &progress );

    end_index = 0;

    delete_hash_table( &edge_table );

    if( topology_error )
    {
        print(
          "create_polygon_neighbours():  more than 2 faces share an edge.\n" );
    }

#ifdef DEBUG_NEIGHBOURS
    {
        int   i, n_open;

        n_open = 0;
        for_less( i, 0, end_indices[n_polygons-1] )
        {
            if( neighbours[i] == INVALID_ID )
                ++n_open;
        }
    
        if( n_open > 0 )
        {
            print( "Polygon not closed: %d/%d.\n", n_open,
                   end_indices[n_polygons-1] );
        }
    }
#endif
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : assign_neighbours
@INPUT      : indices
              end_indices
              polygon1
              edge1
              polygon2
@OUTPUT     : neighbours
              topology_error
@RETURNS    : 
@DESCRIPTION: Assigns the neighbour for edge1 of polygon1 to be polygon2
              and for the corresponding edge of polygon2 to be polygon1.
              If either one has already been assigned, then a topology error
              is flagged.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  assign_neighbours(
    int       indices[],
    int       end_indices[],
    int       neighbours[],
    int       polygon1,
    int       edge1,
    int       polygon2,
    BOOLEAN   *topology_error )
{
    int   i0, i1, edge2, start_index1, size1, start_index2, size2;
    int   p2_i0, p2_i1;

    if( polygon1 == 0 )
        start_index1 = 0;
    else
        start_index1 = end_indices[polygon1-1];
    size1 = end_indices[polygon1] - start_index1;

    if( polygon2 == 0 )
        start_index2 = 0;
    else
        start_index2 = end_indices[polygon2-1];
    size2 = end_indices[polygon2] - start_index2;

    i0 = indices[start_index1 + edge1];
    i1 = indices[start_index1 + (edge1 + 1) % size1];

    for_less( edge2, 0, size2 )
    {
        p2_i0 = indices[start_index2 + edge2];
        p2_i1 = indices[start_index2 + (edge2 + 1) % size2];

        if( (p2_i0 == i0 && p2_i1 == i1) ||
            (p2_i0 == i1 && p2_i1 == i0) )
        {
            break;
        }
    }

    if( edge2 == size2 )
        handle_internal_error( "assign neighbours" );

    if( neighbours[POINT_INDEX( end_indices, polygon1, edge1 )] != INVALID_ID ||
        neighbours[POINT_INDEX( end_indices, polygon2, edge2 )] != INVALID_ID )
    {
        *topology_error = TRUE;
    }

    neighbours[POINT_INDEX( end_indices, polygon1, edge1 )] = polygon2;
    neighbours[POINT_INDEX( end_indices, polygon2, edge2 )] = polygon1;
}
