#include  <module.h>

private  int  get_polygons(
    Marching_cubes_methods  method,
    Real                    corners[2][2][2],
    Real                    isovalue,
    int                     *sizes[],
    voxel_point_type        *points[] )
{
    int           n_polygons;
    static   int  static_sizes[4] = { 3, 3, 3, 3 };

    switch( method )
    {
    case MARCHING_CUBES:
        n_polygons = compute_isotriangles_in_voxel( corners, isovalue, points );
        *sizes = static_sizes;
        break;

    case MARCHING_NO_HOLES:
        n_polygons = get_holeless_isosurface_polygons( corners, isovalue, sizes,
                                                    points );
        break;
    }

    return( n_polygons );
}

private  BOOLEAN  is_binary_inside(
    Real   value,
    Real   min_value,
    Real   max_value )
{
    return( min_value <= value && value <= max_value );
}

public  int  compute_isosurface_in_voxel(
    Marching_cubes_methods  method,
    Real                    corners[2][2][2],
    BOOLEAN                 binary_flag,
    Real                    min_value,
    Real                    max_value,
    int                     *sizes[],
    voxel_point_type        *points[] )
{
    int   i, j, k;
    Real  binary_corners[2][2][2];

    if( binary_flag )
    {
        for_less( i, 0, 2 )
            for_less( j, 0, 2 )
                for_less( k, 0, 2 )
                {
                    if( is_binary_inside( corners[i][j][k],
                                          min_value, max_value ) )
                    {
                        binary_corners[i][j][k] = 1.0;
                    }
                    else
                        binary_corners[i][j][k] = 0.0;
                }

        return( get_polygons( method, binary_corners, 0.5, sizes, points ) );
    }
    else
        return( get_polygons( method, corners, min_value, sizes, points ) );
}

public  Point_classes  get_isosurface_point(
    Point           *point1,
    Real            val1,
    Point           *point2,
    Real            val2,
    BOOLEAN         binary_flag,
    Real            min_value,
    Real            max_value,
    Point           *point )
{
    Real            alpha;
    Point_classes   point_class;

    if( binary_flag )
    {
        if( is_binary_inside( val1, min_value, max_value ) !=
            is_binary_inside( val2, min_value, max_value ) )
        {
            point_class = ON_EDGE;
            INTERPOLATE_POINTS( *point, *point1, *point2, 0.5 );
        }
        else
            point_class = (Point_classes) -1;
    }
    else
    {
        if( val1 == min_value )
        {
            point_class = ON_FIRST_CORNER;
            *point = *point1;
        }
        else if( val2 == min_value )
        {
            point_class = ON_SECOND_CORNER;
            *point = *point2;
        }
        else
        {
            if( val1 < min_value && val2 > min_value ||
                val1 > min_value && val2 < min_value )
            {
                point_class = ON_EDGE;
                alpha = (min_value - val1) / (val2 - val1);
                INTERPOLATE_POINTS( *point, *point1, *point2, alpha );
            }
            else
                point_class = (Point_classes) -1;
        }
    }

    return( point_class );
}
