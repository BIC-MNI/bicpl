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
#include  <geom.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Objects/lines.c,v 1.14 1996-05-02 14:26:09 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_lines
@INPUT      : col
@OUTPUT     : lines
@RETURNS    : 
@DESCRIPTION: Initializes the lines to empty.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  initialize_lines(
    lines_struct    *lines,
    Colour          col )
{
    ALLOC( lines->colours, 1 );

    lines->colour_flag = ONE_COLOUR;
    lines->colours[0] = col;

    lines->line_thickness = 1.0f;
    lines->n_points = 0;
    lines->n_items = 0;

    lines->bintree = (bintree_struct_ptr) NULL;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_lines
@INPUT      : lines
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees up memory associated with the lines.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  delete_lines( lines_struct *lines )
{
    free_colours( lines->colour_flag, lines->colours, lines->n_points,
                  lines->n_items );

    if( lines->n_points > 0 )
        FREE( lines->points );

    if( lines->n_items > 0 )
        FREE( lines->end_indices );

    if( lines->n_items > 0 )
        FREE( lines->indices );

    delete_bintree_if_any( &lines->bintree );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : start_new_line
@INPUT      : lines
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Creates a new line in the structure of size 0, in preparation
              for adding points to the line.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  start_new_line( lines_struct *lines )
{
    int      n_indices;

    n_indices = NUMBER_INDICES( *lines );

    ADD_ELEMENT_TO_ARRAY( lines->end_indices, lines->n_items,
                          n_indices, DEFAULT_CHUNK_SIZE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : add_point_to_line
@INPUT      : lines
              point
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Adds a point to the line.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  add_point_to_line(
    lines_struct   *lines,
    Point          *point )
{
    if( lines->n_items == 0 )
        start_new_line( lines );

    ADD_ELEMENT_TO_ARRAY( lines->indices, lines->end_indices[lines->n_items-1],
                          lines->n_points, DEFAULT_CHUNK_SIZE );

    ADD_ELEMENT_TO_ARRAY( lines->points, lines->n_points,
                          *point, DEFAULT_CHUNK_SIZE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_line_segment_index
@INPUT      : lines
              obj_index
@OUTPUT     : line
              seg
@RETURNS    : 
@DESCRIPTION: Converts an obj_index to a line index, and a line segment
              index.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_line_segment_index(
    lines_struct  *lines,
    int           obj_index,
    int           *line,
    int           *seg )
{
    *line = 0;
    while( obj_index > lines->end_indices[*line] - *line - 2 )
        ++(*line);

    if( *line == 0 )
        *seg = obj_index;
    else
        *seg = obj_index - (lines->end_indices[(*line)-1] - *line);
}

static  void  (*bintree_delete_function) ( bintree_struct_ptr* ) = NULL;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_bintree_delete_function
@INPUT      : func
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the bintree delete function for objects.  This is done
              so that if the application does not use any bintree code, the
              bintree library does not get linked into the executable, simply
              because the delete objects code might have to delete a bintree.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_bintree_delete_function(
    void  (*func)( bintree_struct_ptr * ) )
{
    bintree_delete_function = func;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_bintree_if_any
@INPUT      : bintree
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes the bintree, if non-null.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  delete_bintree_if_any(
    bintree_struct_ptr   *bintree )
{
    if( bintree_delete_function != NULL )
        (*bintree_delete_function) ( bintree );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_lines_length
@INPUT      : lines
@OUTPUT     : 
@RETURNS    : length
@DESCRIPTION: Gets the total length of the lines.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  get_lines_length(
    lines_struct  *lines )
{
    Real  len;
    int   line, i, p0, p1, size;

    len = 0.0;

    for_less( line, 0, lines->n_items )
    {
        size = GET_OBJECT_SIZE( *lines, line );

        for_less( i, 0, size-1 )
        {
            p0 = lines->indices[POINT_INDEX(lines->end_indices,line,i)];
            p1 = lines->indices[POINT_INDEX(lines->end_indices,line,i+1)];
            len += distance_between_points( &lines->points[p0],
                                            &lines->points[p1] );
        }
    }

    return( len );
}
