
#include  <def_mni.h>
#include  <def_module.h>

#define  CHECK_INTERVAL     1.0

private  void  smooth_points(
    polygons_struct  *polygons,
    Point            current_points[],
    Point            new_points[],
    Smallest_int     point_done[],
    Real             max_dist_from_original,
    Real             fraction_to_move,
    Real             normal_ratio,
    Boolean          range_flag,
    volume_struct    *volume,
    int              min_value,
    int              max_value,
    Real             *avg_moved,
    Real             *max_moved );
private  Real  update_point_position(
    polygons_struct  *polygons,
    int              poly,
    int              vertex_index,
    int              point_index,
    Point            current_points[],
    Point            *new_point,
    Real             max_dist_from_original,
    Real             fraction_to_move,
    Real             normal_ratio,
    Boolean          range_flag,
    volume_struct    *volume,
    int              min_value,
    int              max_value );
private  Boolean   point_inside_range(
    volume_struct  *volume,
    Point          *point,
    int            min_value,
    int            max_value );

public  void  smooth_polygon(
    polygons_struct  *polygons,
    Real             max_dist_from_original,
    Real             fraction_to_move,
    Real             stop_threshold,
    Real             normal_ratio,
    Boolean          range_flag,
    volume_struct    *volume,
    int              min_value,
    int              max_value )
{
    Real               avg_moved, max_moved;
    int                i, iteration;
    Point              *new_points, *tmp, *current_points;
    Smallest_int       *point_done;
    Real               next_check_time;

    if( polygons->n_points <= 0 )
        return;

    ALLOC( new_points, polygons->n_points );
    ALLOC( current_points, polygons->n_points );
    ALLOC( point_done, polygons->n_points );

    check_polygons_neighbours_computed( polygons );

    for_less( i, 0, polygons->n_points )
    {
        current_points[i] = polygons->points[i];
    }

    iteration = 0;
    next_check_time = current_realtime_seconds() + CHECK_INTERVAL;
    do
    {
        smooth_points( polygons, current_points, new_points, point_done,
                       max_dist_from_original, fraction_to_move,
                       normal_ratio, range_flag,
                       volume, min_value, max_value,
                       &avg_moved, &max_moved );

        tmp = current_points;
        current_points = new_points;
        new_points = tmp;

        ++iteration;

        print( "Iteration %d -- avg distance %g  max distance %g\n",
               iteration, avg_moved, max_moved );
        if( current_realtime_seconds() > next_check_time )
        {
            next_check_time = current_realtime_seconds() + CHECK_INTERVAL;

            if( file_exists("interrupt") )
            {
                print( "Interrupting as requested\n" );
                remove_file( "interrupt" );
                break;
            }
        }
    }
    while( max_moved > stop_threshold );

    for_less( i, 0, polygons->n_points )
        polygons->points[i] = current_points[i];

    FREE( new_points );
    FREE( current_points );
    FREE( point_done );
}

private  void  smooth_points(
    polygons_struct  *polygons,
    Point            current_points[],
    Point            new_points[],
    Smallest_int     point_done[],
    Real             max_dist_from_original,
    Real             fraction_to_move,
    Real             normal_ratio,
    Boolean          range_flag,
    volume_struct    *volume,
    int              min_value,
    int              max_value,
    Real             *avg_moved,
    Real             *max_moved )
{
    int              p, vertex_index, point_index, poly, size;
    progress_struct  progress;
    Real             moved;

    for_less( p, 0, polygons->n_points )
    {
        new_points[p] = current_points[p];
        point_done[p] = FALSE;
    }

    *avg_moved = 0.0;
    *max_moved = 0.0;

    initialize_progress_report( &progress, TRUE, polygons->n_items,
                                "Averaging Points" );

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

                moved = update_point_position( polygons, poly, vertex_index,
                        point_index, current_points, &new_points[point_index],
                        max_dist_from_original, fraction_to_move, normal_ratio,
                        range_flag, volume, min_value, max_value );

                *avg_moved += moved;
                if( moved > *max_moved )
                    *max_moved = moved;
            }
        }

        update_progress_report( &progress, poly+1 );
    }

    terminate_progress_report( &progress );

    *avg_moved /= polygons->n_points;
}

#define  MAX_NEIGHBOURS   100

private  Real  update_point_position(
    polygons_struct  *polygons,
    int              poly,
    int              vertex_index,
    int              point_index,
    Point            current_points[],
    Point            *new_point,
    Real             max_dist_from_original,
    Real             fraction_to_move,
    Real             normal_ratio,
    Boolean          range_flag,
    volume_struct    *volume,
    int              min_value,
    int              max_value )
{
    int              i, halves, n_neighbours;
    int              neighbours[MAX_NEIGHBOURS];
    Point            neigh_points[MAX_NEIGHBOURS];
    Point            new_pos, current_scaled, centroid, destination;
    Real             len, ratio, movement, sin_of_angle;
    Vector           diff, delta, normal, unit_delta, unit_normal, cross;
    Boolean          interior_point;

    n_neighbours = get_neighbours_of_point( polygons, poly, vertex_index,
                                            neighbours, MAX_NEIGHBOURS,
                                            &interior_point );

    if( interior_point && n_neighbours > 2 )
    {
        for_less( i, 0, n_neighbours )
            neigh_points[i] = current_points[neighbours[i]];

        get_points_centroid( n_neighbours, neigh_points,
                             &centroid );

        destination = centroid;

        if( normal_ratio > 0.0 )
        {
            find_polygon_normal( n_neighbours, neigh_points,
                                 &normal );

            if( !null_Vector(&normal) )
            {
                NORMALIZE_VECTOR( unit_normal, normal );
                SUB_POINTS( delta, centroid, current_points[point_index] );
                NORMALIZE_VECTOR( unit_delta, delta );
                CROSS_VECTORS( cross, unit_delta, unit_normal );
                sin_of_angle = MAGNITUDE( cross );
                ratio = DOT_VECTORS( delta, normal ) /
                        DOT_VECTORS( normal, normal );
                SCALE_VECTOR( normal, normal, -ratio * normal_ratio *
                              sin_of_angle);
                ADD_POINT_VECTOR( destination, centroid, normal );
            }
        }

        SCALE_POINT( new_pos, destination, fraction_to_move );
        SCALE_POINT( current_scaled, current_points[point_index],
                     1.0-fraction_to_move );
        ADD_POINTS( new_pos, new_pos, current_scaled );

        SUB_POINTS( diff, new_pos, polygons->points[point_index] );
        len = MAGNITUDE( diff );

        if( len > max_dist_from_original )
        {
            ratio = max_dist_from_original / len;
            SCALE_VECTOR( diff, diff, ratio );
            ADD_POINT_VECTOR( new_pos, polygons->points[point_index], diff );
        }

        if( range_flag )
        {
#define  MAX_CUTS_IN_HALF    4
            for_less( halves, 0, MAX_CUTS_IN_HALF )
            {
                if( point_inside_range( volume, &new_pos,
                                        min_value, max_value ) )
                {
                    break;
                }

                ADD_POINTS( new_pos, new_pos, current_points[point_index] );
                SCALE_POINT( new_pos, new_pos, 0.5 );
            }

            if( halves == MAX_CUTS_IN_HALF )
                new_pos = current_points[point_index];
        }

        *new_point = new_pos;

        SUB_POINTS( diff, new_pos, current_points[point_index] );

        movement = MAGNITUDE( diff );
    }
    else
        movement = 0.0;

    return( movement );
}

private  Boolean   point_inside_range(
    volume_struct  *volume,
    Point          *point,
    int            min_value,
    int            max_value )
{
    int     vx, vy, vz, val;
    Real    x, y, z;

    convert_world_to_voxel( volume,
                            Point_x(*point), Point_y(*point), Point_z(*point),
                            &x, &y, &z );

    vx = ROUND( x );
    vy = ROUND( y );
    vz = ROUND( z );

    val = GET_VOLUME_DATA( *volume, vx, vy, vz );

    return( min_value <= val &&  val <= max_value );
}
