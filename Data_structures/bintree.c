
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

#include  "bicpl_internal.h"

#ifndef lint
static char rcsid[] = "$Header: /static-cvsroot/libraries/bicpl/Data_structures/bintree.c,v 1.10 2005-08-17 22:31:12 bert Exp $";
#endif

static  VIO_Status  io_range(
    FILE             *file,
    VIO_IO_types         direction,
    VIO_File_formats     format,
    range_struct     *range );
static  VIO_Status  output_bintree_node(
    FILE                    *file,
    VIO_File_formats            format,
    bintree_node_struct     *node );
static  VIO_Status  input_bintree_node(
    FILE                    *file,
    VIO_File_formats            format,
    bintree_node_struct     **node );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_bintree
@INPUT      : x_min
              x_max
              y_min
              y_max
              z_min
              z_max
@OUTPUT     : bintree
@RETURNS    : 
@DESCRIPTION: Initializes the bintree (a multidimensional binary tree)
              to empty.  Bintrees are usually used to organize geometric
              objects for fast intersection and proximity tests.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  initialize_bintree(
    VIO_Real                 x_min,
    VIO_Real                 x_max,
    VIO_Real                 y_min,
    VIO_Real                 y_max,
    VIO_Real                 z_min,
    VIO_Real                 z_max,
    bintree_struct_ptr   bintree )
{
    bintree->range.limits[VIO_X][0] = (float) x_min;
    bintree->range.limits[VIO_X][1] = (float) x_max;
    bintree->range.limits[VIO_Y][0] = (float) y_min;
    bintree->range.limits[VIO_Y][1] = (float) y_max;
    bintree->range.limits[VIO_Z][0] = (float) z_min;
    bintree->range.limits[VIO_Z][1] = (float) z_max;

    bintree->n_nodes = 0;
    bintree->root = (bintree_node_struct *) 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_bintree_node
@INPUT      : node
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes a node of the bintree.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  delete_bintree_node(
    bintree_node_struct   *node )
{
    FREE( node );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : recursive_delete_bintree
@INPUT      : node
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes a bintree node and its children.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  void  recursive_delete_bintree(
    bintree_node_struct   *node )
{
    bintree_node_struct  *left, *right;

    if( get_bintree_left_child( node, &left ) )
        recursive_delete_bintree( left );
    if( get_bintree_right_child( node, &right ) )
        recursive_delete_bintree( right );

    delete_bintree_node( node );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_bintree
@INPUT      : bintree
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes the bintree.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  delete_bintree(
    bintree_struct_ptr   bintree )
{
    if( bintree->root != NULL )
        recursive_delete_bintree( bintree->root );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_bintree_limits
@INPUT      : bintree
@OUTPUT     : limits
@RETURNS    : 
@DESCRIPTION: Passes back the x, y, z range of the bintree.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  get_bintree_limits(
    bintree_struct_ptr    bintree,
    range_struct          *limits )
{
    *limits = bintree->range;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_bintree_child
@INPUT      : node
              which    : LEFT_CHILD or RIGHT_CHILD
              child
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the given child of the node to the 'child'.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  void  set_bintree_child(
    bintree_node_struct   *node,
    int                   which,
    bintree_node_struct   *child )
{
    int                   ind;

    if( which == LEFT_CHILD )
        ind = 0;
    else
    {
        if( (node->node_info & LEFT_CHILD_EXISTS) != 0 )
            ind = 1;
        else
            ind = 0;
    }

    node->data.children[ind] = child;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_bintree_node
@INPUT      : bintree
@OUTPUT     : node_index
@RETURNS    : 
@DESCRIPTION: Creates an empty bintree leaf node.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  bintree_node_struct  *create_bintree_internal_node(
    int                   split_coord,
    VIO_Real                  split_position,
    bintree_node_struct   *left,
    bintree_node_struct   *right )
{
    bintree_node_struct  *node;
    int                  n_children, children_bits;

    n_children = 0;

    children_bits = 0;

    if( left != NULL )
    {
        children_bits |= LEFT_CHILD_EXISTS;
        ++n_children;
    }
    if( right != NULL )
    {
        children_bits |= RIGHT_CHILD_EXISTS;
        ++n_children;
    }

    if( n_children == 0 )
    {
        handle_internal_error( "create_bintree_internal_node" );
        return( NULL );
    }

    ALLOC_VAR_SIZED_STRUCT( node, bintree_node_struct *, n_children );

    node->node_info = (unsigned char) (split_coord | children_bits);
    node->split_position = (float) split_position;

    /* --- set the children */

    if( left != NULL )
        set_bintree_child( node, LEFT_CHILD, left );

    if( right != NULL )
        set_bintree_child( node, RIGHT_CHILD, right );

    return( node );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_bintree_leaf
@INPUT      : split_position
              n_objects
              object_list
@OUTPUT     : 
@RETURNS    : leaf
@DESCRIPTION: Creates a leaf node containing pointers to the objects.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  bintree_node_struct  *create_bintree_leaf(
    VIO_Real                  split_position,
    int                   n_objects,
    int                   object_list[] )
{
    bintree_node_struct  *node;
    int                  i, n_alloc, *node_list, n_objects_bits;

    if( n_objects <= MAX_NODE_INFO_OBJECTS )
    {
        n_alloc = n_objects;
        n_objects_bits = n_objects;
    }
    else
    {
        n_alloc = n_objects + 1;
        n_objects_bits = 0;
    }

    ALLOC_VAR_SIZED_STRUCT( node, int, n_alloc );

    node->node_info = (unsigned char)
                (LEAF_SIGNAL | (n_objects_bits << NODE_INFO_OBJECTS_SHIFT));
    node->split_position = (float) split_position;

    node_list = node->data.object_list;

    if( n_objects > MAX_NODE_INFO_OBJECTS )
    {
        node_list[0] = n_objects;
        ++node_list;
    }

    for_less( i, 0, n_objects )
        node_list[i] = object_list[i];

    return( node );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : bintree_node_is_leaf
@INPUT      : bintree
              node
@OUTPUT     : 
@RETURNS    : TRUE if node is leaf
@DESCRIPTION:
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  bintree_node_is_leaf(
    bintree_node_struct  *node )
{
    return( (node->node_info & SUBDIVISION_AXIS_BITS) == LEAF_SIGNAL );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_bintree_leaf_info
@INPUT      : bintree
              node
@OUTPUT     : object_list
@RETURNS    : # objects
@DESCRIPTION: Passes back the object list and returns the number in the list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  int  get_bintree_leaf_objects(
    bintree_node_struct  *node,
    int                  *object_list[] )
{
    int    *node_list, n_objects;

    node_list = node->data.object_list;
    n_objects = node->node_info >> NODE_INFO_OBJECTS_SHIFT;

    if( n_objects == 0 )
    {
        n_objects = node_list[0];
        ++node_list;
    }

    if( n_objects > 0 )
        *object_list = node_list;

    return( n_objects );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_bintree_left_child
@INPUT      : bintree
              node
@OUTPUT     : left_child
              left_limit
@RETURNS    : TRUE if child exists
@DESCRIPTION: Passes back the left child of the given node, if it exists.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI VIO_BOOL  get_bintree_left_child_ptr(
    bintree_node_struct  *node,
    bintree_node_struct  ***ptr_to_left_child )
{
    VIO_BOOL               child_exists;

    child_exists = !bintree_node_is_leaf(node) &&
                   (node->node_info & LEFT_CHILD_EXISTS) != 0;

    if( child_exists )
        *ptr_to_left_child = &node->data.children[0];

    return( child_exists );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_bintree_left_child
@INPUT      : bintree
              node
@OUTPUT     : left_child
@RETURNS    : TRUE if child exists
@DESCRIPTION: Passes back the left child of the given node, if it exists.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  get_bintree_left_child(
    bintree_node_struct  *node,
    bintree_node_struct  **left_child )
{
    VIO_BOOL              child_exists;
    bintree_node_struct  **ptr;

    child_exists = get_bintree_left_child_ptr( node, &ptr );

    if( child_exists )
        *left_child = *ptr;

    return( child_exists );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_bintree_right_child
@INPUT      : bintree
              node
@OUTPUT     : right_child
@RETURNS    : TRUE if child exists
@DESCRIPTION: Passes back the right child of the given node, if it exists.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  get_bintree_right_child_ptr(
    bintree_node_struct  *node,
    bintree_node_struct  ***ptr_to_right_child )
{
    VIO_BOOL               child_exists;
    int                   ind;

    child_exists = !bintree_node_is_leaf(node) &&
                   (node->node_info & RIGHT_CHILD_EXISTS) != 0;

    if( child_exists )
    {
        if( (node->node_info & LEFT_CHILD_EXISTS) != 0 )
            ind = 1;
        else
            ind = 0;

        *ptr_to_right_child = &node->data.children[ind];
    }

    return( child_exists );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_bintree_right_child
@INPUT      : bintree
              node
@OUTPUT     : right_child
@RETURNS    : TRUE if child exists
@DESCRIPTION: Passes back the right child of the given node, if it exists.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  get_bintree_right_child(
    bintree_node_struct  *node,
    bintree_node_struct  **right_child )
{
    VIO_BOOL              child_exists;
    bintree_node_struct  **ptr;

    child_exists = get_bintree_right_child_ptr( node, &ptr );

    if( child_exists )
        *right_child = *ptr;

    return( child_exists );
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_node_split_axis
@INPUT      : node
@OUTPUT     : 
@RETURNS    : axis VIO_X, VIO_Y, or VIO_Z
@DESCRIPTION: Returns the split axis of the node.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  int  get_node_split_axis(
    bintree_node_struct  *node )
{
    return( node->node_info & SUBDIVISION_AXIS_BITS );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_node_split_position
@INPUT      : node
@OUTPUT     : 
@RETURNS    : position
@DESCRIPTION: Returns the split position of the node.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Real  get_node_split_position(
    bintree_node_struct  *node )
{
    return( (VIO_Real) node->split_position );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : point_within_range
@INPUT      : point
              range
@OUTPUT     : 
@RETURNS    : TRUE if point is within the box
@DESCRIPTION: Checks if the point is within the given 3D box.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  point_within_range(
    VIO_Point         *point,
    range_struct  *range )
{
    VIO_BOOL   within;

    within = (Point_x(*point) >= range->limits[VIO_X][0] &&
              Point_x(*point) <= range->limits[VIO_X][1] &&
              Point_y(*point) >= range->limits[VIO_Y][0] &&
              Point_y(*point) <= range->limits[VIO_Y][1] &&
              Point_z(*point) >= range->limits[VIO_Z][0] &&
              Point_z(*point) <= range->limits[VIO_Z][1]);

    return( within );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : range_volume
@INPUT      : range
@OUTPUT     : 
@RETURNS    : returns volume
@DESCRIPTION: Returns the volume of the given range.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Real  range_volume(
    range_struct  *range )
{
    return( ((VIO_Real) range->limits[VIO_X][1] - (VIO_Real) range->limits[VIO_X][0]) *
            ((VIO_Real) range->limits[VIO_Y][1] - (VIO_Real) range->limits[VIO_Y][0]) *
            ((VIO_Real) range->limits[VIO_Z][1] - (VIO_Real) range->limits[VIO_Z][0]) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : range_surface_area
@INPUT      : range
@OUTPUT     : 
@RETURNS    : returns surface area
@DESCRIPTION: Returns the surface area of the given range.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Real  range_surface_area(
    range_struct  *range )
{
    VIO_Real   dx, dy, dz;

    dx = (VIO_Real) range->limits[VIO_X][1] - (VIO_Real) range->limits[VIO_X][0];
    dy = (VIO_Real) range->limits[VIO_Y][1] - (VIO_Real) range->limits[VIO_Y][0];
    dz = (VIO_Real) range->limits[VIO_Z][1] - (VIO_Real) range->limits[VIO_Z][0];

    return( 2.0 * (dx * dy + dy * dz + dz * dx) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : io_bintree
@INPUT      : file
              direction
              format
              n_objects
              bintree
@OUTPUT     : 
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Writes or reads the bintree.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status  io_bintree(
    FILE                 *file,
    VIO_IO_types             direction,
    VIO_File_formats         format,
    bintree_struct_ptr   bintree )
{
    VIO_Status   status;

    status = io_range( file, direction, format, &bintree->range );

    if( status == VIO_OK && direction == WRITE_FILE )
        status = output_bintree_node( file, format, bintree->root );
    else if( status == VIO_OK && direction == READ_FILE )
        status = input_bintree_node( file, format, &bintree->root );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : io_range
@INPUT      : file
              direction
              format
              range
@OUTPUT     : 
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Writes or reads a 3D box range.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  VIO_Status  io_range(
    FILE             *file,
    VIO_IO_types         direction,
    VIO_File_formats     format,
    range_struct     *range )
{
    VIO_Status   status;
    int      c, l;

    status = VIO_OK;

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        for_less( l, 0, 2 )
        {
            if( status == VIO_OK )
            {
                status = io_float( file, direction, format,
                                   &range->limits[c][l] );
            }
        }
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_leaf_node
@INPUT      : file
              format
              node
@OUTPUT     : 
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Outputs a leaf node.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  VIO_Status  output_leaf_node(
    FILE                    *file,
    VIO_File_formats            format,
    bintree_node_struct     *node )
{
    VIO_Status  status;
    int     *object_list, n_objects;

    status = VIO_OK ;

    n_objects = get_bintree_leaf_objects( node, &object_list );

    if( n_objects > MAX_NODE_INFO_OBJECTS )
        status = io_int( file, WRITE_FILE, format, &n_objects );

    if( status == VIO_OK )
        status = io_ints( file, WRITE_FILE, format, n_objects, &object_list );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_bintree_node
@INPUT      : file
              format
              node
@OUTPUT     : 
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Outputs a bintree node.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  VIO_Status  output_bintree_node(
    FILE                    *file,
    VIO_File_formats            format,
    bintree_node_struct     *node )
{
    VIO_Status               status;
    bintree_node_struct  *left, *right;

    status = io_binary_data( file, WRITE_FILE, &node->node_info,
                             sizeof(node->node_info), 1 );

    if( status == VIO_OK )
        status = io_float( file, WRITE_FILE, format, &node->split_position );

    if( status == VIO_OK )
    {
        if( bintree_node_is_leaf( node ) )
        {
            status = output_leaf_node( file, format, node );
        }
        else
        {
            if( get_bintree_left_child( node, &left ) )
                status = output_bintree_node( file, format, left );
            if( status == VIO_OK && get_bintree_right_child( node, &right ) )
                status = output_bintree_node( file, format, right );
        }
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_bintree_node
@INPUT      : file
              format
@OUTPUT     : node
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Inputs a bintree node
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  VIO_Status  input_bintree_node(
    FILE                    *file,
    VIO_File_formats            format,
    bintree_node_struct     **node )
{
    VIO_Status               status;
    node_info_type       node_info;
    float                split_position;
    int                  n_objects, *object_list;
    bintree_node_struct  *left, *right;

    *node = NULL;

    status = io_binary_data( file, READ_FILE, &node_info, sizeof(node_info), 1);

    if( status == VIO_OK )
        status = io_float( file, READ_FILE, format, &split_position );

    if( (node_info & SUBDIVISION_AXIS_BITS) == LEAF_SIGNAL )
    {
        n_objects = node_info >> NODE_INFO_OBJECTS_SHIFT;

        if( n_objects == 0 )
            status = io_int( file, READ_FILE, format, &n_objects );

        if( status == VIO_OK && n_objects > 0 )
            status = io_ints( file, READ_FILE, format, n_objects, &object_list);

        if( status == VIO_OK )
        {
            *node = create_bintree_leaf( (VIO_Real) split_position,
                                         n_objects, object_list );
            if( n_objects > 0 )
                FREE( object_list );
        }
    }
    else
    {
        status = input_bintree_node( file, format, &left );
        if( status == VIO_OK )
            status = input_bintree_node( file, format, &right );

        if( status == VIO_OK )
            *node = create_bintree_internal_node(
                                   node_info & SUBDIVISION_AXIS_BITS,
                                   (VIO_Real) split_position, left, right );
    }

    return( status );
}
