#include  <module.h>

private  void  recursive_find_closest_point(
    Point                 *point,
    bintree_node_struct   *node,
    range_struct          *range,
    object_struct         *object,
    int                   *obj_index,
    Real                  *closest_dist,
    Point                 *closest_point );
private  Real  get_point_range_dist(
    Point         *point,
    range_struct  *range );

public  Real  find_closest_point_in_bintree(
    Point           *point,
    bintree_struct  *bintree,
    object_struct   *object,
    int             *obj_index,
    Point           *point_on_object )
{
    Real      dist;

    dist = 1.0e30;

    if( obj_index != (int *) NULL )
        *obj_index = -1;

    recursive_find_closest_point( point, bintree->root, &bintree->range,
                                  object, obj_index, &dist, point_on_object );

    return( dist );
}

private  void  recursive_find_closest_point(
    Point                 *point,
    bintree_node_struct   *node,
    range_struct          *range,
    object_struct         *object,
    int                   *obj_index,
    Real                  *closest_dist,
    Point                 *closest_point )
{
    int                   i, n_objects, *object_list, axis_index;
    int                   n_to_search;
    bintree_node_struct   *children[2], *tmp_node;
    range_struct          children_ranges[2], tmp_range;
    Real                  children_distances[2];
    Real                  dist;
    Point                 object_point;
    Real                  left_limit, right_limit;

    if( bintree_node_is_leaf( node ) )
    {
        n_objects = get_bintree_leaf_objects( node, &object_list );

        for_less( i, 0, n_objects )
        {
            dist = get_point_object_distance( point, object, object_list[i],
                                              &object_point );

            if( dist < *closest_dist )
            {
                *closest_dist = dist;
                *closest_point = object_point;
                *obj_index = object_list[i];
            }
        }
    }
    else
    {
        axis_index = get_node_split_axis( node );

        n_to_search = 0;

        if( get_bintree_left_child( node, &children[n_to_search] ) )
        {
            left_limit = get_node_split_position( children[n_to_search] );
            children_ranges[n_to_search] = *range;
            children_ranges[n_to_search].limits[axis_index][1] = left_limit;
            children_distances[n_to_search] = get_point_range_dist( point,
                                           &children_ranges[n_to_search] );

            if( children_distances[n_to_search] <= *closest_dist )
                ++n_to_search;
        }

        if( get_bintree_right_child( node, &children[n_to_search] ) )
        {
            right_limit = get_node_split_position( children[n_to_search] );
            children_ranges[n_to_search] = *range;
            children_ranges[n_to_search].limits[axis_index][0] = right_limit;
            children_distances[n_to_search] = get_point_range_dist( point,
                                             &children_ranges[n_to_search] );

            if( children_distances[n_to_search] <= *closest_dist )
                ++n_to_search;
        }

        if( n_to_search == 2 && children_distances[1] < children_distances[0] )
        {
            tmp_range = children_ranges[0];
            children_ranges[0] = children_ranges[1];
            children_ranges[1] = tmp_range;
            tmp_node = children[0];
            children[0] = children[1];
            children[1] = tmp_node;
        }

        for_less( i, 0, n_to_search )
        {
            recursive_find_closest_point( point, children[i],
                                          &children_ranges[i],
                                          object, obj_index,
                                          closest_dist, closest_point );
        }
    }
}

private  Real  get_point_range_dist(
    Point         *point,
    range_struct  *range )
{
    int      c;
    Real     min_plane, max_plane, point_pos;
    Vector   offset;

    for_less( c, 0, N_DIMENSIONS )
    {
        min_plane = range->limits[c][0];
        max_plane = range->limits[c][1];
        point_pos = Point_coord(*point,c);
        if( point_pos < min_plane )
            Vector_coord( offset, c ) = min_plane - point_pos;
        else if( point_pos > max_plane )
            Vector_coord( offset, c ) = point_pos - max_plane;
        else
            Vector_coord( offset, c ) = 0.0;
    }

    return( MAGNITUDE( offset ) );
}
