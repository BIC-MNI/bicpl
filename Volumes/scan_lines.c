#include  <internal_volume_io.h>
#include  <bicpl.h>

private  void  scan_line_segment_to_voxels(
    Volume    volume,
    Volume    label_volume,
    int       label,
    Point     *p1,
    Point     *p2,
    Real      radius );

public  void  scan_lines_to_voxels(
    lines_struct     *lines,
    Volume           volume,
    Volume           label_volume,
    int              label )
{
    int        i, line, size;
    Point      p1, p2;

    for_less( line, 0, lines->n_items )
    {
        size = GET_OBJECT_SIZE( *lines, line );

        for_less( i, 0, size-1 )
        {
            p1 = lines->points[
              lines->indices[POINT_INDEX(lines->end_indices,line,i)]];
            p2 = lines->points[
              lines->indices[POINT_INDEX(lines->end_indices,line,i+1)]];

            scan_line_segment_to_voxels( volume, label_volume, label,
                                         &p1, &p2,
                                         (Real) lines->line_thickness );
        }
    }
}

private  Real  point_segment_distance_squared(
    Point    *point,
    Point    *p1,
    Point    *p2 )
{
    Real     t, mag;
    Point    line_point;
    Vector   p1_p2, p1_p, offset;

    SUB_POINTS( p1_p2, *p2, *p1 );
    SUB_POINTS( p1_p, *point, *p1 );

    mag = DOT_VECTORS( p1_p2, p1_p2 );

    if( mag == 0.0 )
        line_point = *p1;
    else
    {
        t = DOT_VECTORS( p1_p, p1_p2 ) / mag;

        if( t <= 0.0 )
            line_point = *p1;
        else if( t >= 1.0 )
            line_point = *p2;
        else
            INTERPOLATE_POINTS( line_point, *p1, *p2, t );
    }

    SUB_POINTS( offset, *point, line_point );

    return( DOT_VECTORS( offset, offset ) );
}

private  void  scan_line_segment_to_voxels(
    Volume    volume,
    Volume    label_volume,
    int       label,
    Point     *p1,
    Point     *p2,
    Real      radius )
{
    int        c, sizes[MAX_DIMENSIONS];
    Point      min_point, max_point;
    Real       min_voxel[MAX_DIMENSIONS];
    Real       max_voxel[MAX_DIMENSIONS];
    Real       x_world, y_world, z_world;
    int        int_voxel[MAX_DIMENSIONS];
    int        int_min_voxel[MAX_DIMENSIONS];
    int        int_max_voxel[MAX_DIMENSIONS];
    Point      world_voxel;

    for_less( c, 0, N_DIMENSIONS )
    {
        Point_coord(min_point,c) = MIN(Point_coord(*p1,c),Point_coord(*p2,c)) -
                                   radius;
        Point_coord(max_point,c) = MAX(Point_coord(*p1,c),Point_coord(*p2,c)) +
                                   radius;
    }

    convert_world_to_voxel( volume,
                            (Real) Point_x(min_point),
                            (Real) Point_y(min_point),
                            (Real) Point_z(min_point), min_voxel );
    convert_world_to_voxel( volume,
                            (Real) Point_x(max_point), 
                            (Real) Point_y(max_point),
                            (Real) Point_z(max_point), max_voxel );

    get_volume_sizes( volume, sizes );

    for_less( c, 0, N_DIMENSIONS )
    {
        int_min_voxel[c] = ROUND( min_voxel[c] ) - 1.0;
        int_max_voxel[c] = ROUND( max_voxel[c] ) + 1.0;

        if( int_min_voxel[c] < 0 )
            int_min_voxel[c] = 0;
        if( int_max_voxel[c] > sizes[c]-1 )
            int_max_voxel[c] = sizes[c]-1;
    }

    for_inclusive( int_voxel[X], int_min_voxel[X], int_max_voxel[X] )
    for_inclusive( int_voxel[Y], int_min_voxel[Y], int_max_voxel[Y] )
    for_inclusive( int_voxel[Z], int_min_voxel[Z], int_max_voxel[Z] )
    {
        convert_3D_voxel_to_world( volume, (Real) int_voxel[X],
                                   (Real) int_voxel[Y], (Real) int_voxel[Z],
                                   &x_world, &y_world, &z_world );

        fill_Point( world_voxel, x_world, y_world, z_world );

        if( point_segment_distance_squared( &world_voxel, p1, p2 ) <
                     radius * radius )
        {
            set_volume_label_data( label_volume, int_voxel, label );
        }
    }
}
