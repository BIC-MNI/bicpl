#include  <internal_volume_io.h>
#include  <geom.h>

public  BOOLEAN  is_this_tetrahedral_topology(
    polygons_struct   *polygons )
{
    int    n_polygons;

    n_polygons = polygons->n_items;

    while( n_polygons > 8 && n_polygons % 4 == 0 )
         n_polygons /= 4;

    return( n_polygons == 4 || n_polygons == 6 || n_polygons == 8 );
}

public  int  get_tetra_tessellation_with_n_points(
    int   n_points )
{
    int  n_polygons;

    n_polygons = 2 * (n_points - 2);

    return( n_polygons );
}

public  void  create_tetrahedral_sphere(
    Point            *centre,
    Real             rx,
    Real             ry,
    Real             rz,
    int              n_triangles,
    polygons_struct  *polygons )
{
    int             p, start_size;
    int             n_indices;
    Real            cx, cy, cz, dx, dy, dz, scale;
    Real            x, z, c;

    start_size = n_triangles;

    while( start_size > 8 )
        start_size /= 4;

    cx = Point_x( *centre );
    cy = Point_y( *centre );
    cz = Point_z( *centre );

    if( start_size == 6 )
    {
        create_polygons_sphere( centre, rx, ry, rz, 2, 3, FALSE, polygons );
    }
    else if( start_size == 8 )
    {
        create_polygons_sphere( centre, rx, ry, rz, 2, 4, FALSE, polygons );
    }
    else
    {
        initialize_polygons( polygons, WHITE, NULL );

        polygons->n_points = 4;
        ALLOC( polygons->points, polygons->n_points );
        ALLOC( polygons->normals, polygons->n_points );

        x = sqrt( 8.0 / 9.0 );
        z = - 1.0 / 3.0;
        c = sqrt( 0.75 );
        fill_Point( polygons->points[0], cx, cy, cz + rz );
        fill_Point( polygons->points[1], cx + x * rx, cy, cz + z * rz );
        fill_Point( polygons->points[2], cx - 0.5 * x * rx,
                                         cy + c * ry,
                                         cz + z * rz );
        fill_Point( polygons->points[3], cx - 0.5 * x * rx,
                                         cy - c * ry,
                                         cz + z * rz );

        polygons->n_items = 0;
        n_indices = 0;

        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 0,
                              DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 1,
                              DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 2,
                              DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                              n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 0,
                                  DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 2,
                                  DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 3,
                                  DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                              n_indices,
                                  DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 0,
                                  DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 3,
                                  DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 1,
                                  DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                              n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 1,
                                  DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 3,
                                  DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->indices, n_indices, 2,
                                  DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                              n_indices, DEFAULT_CHUNK_SIZE );
    }

    while( polygons->n_items < n_triangles )
    {
        subdivide_polygons( polygons );

        for_less( p, 0, polygons->n_points )
        {
            dx = Point_x(polygons->points[p]) - cx;
            dy = Point_y(polygons->points[p]) - cy;
            dz = Point_z(polygons->points[p]) - cz;
            scale = dx * dx / rx / rx + dy * dy / ry / ry + dz * dz / rz / rz;
            scale = 1.0 / sqrt( scale );
            dx *= scale;
            dy *= scale;
            dz *= scale;
            fill_Point( polygons->points[p], cx + dx, cy + dy, cz + dz );
        }
    }
}

public  void  half_sample_tetrahedral_tessellation(
    polygons_struct  *polygons,
    polygons_struct  *half )
{
    int             i, quarter_n_polygons;
    static  Point   dummy_centre = { 0.0, 0.0, 0.0 };

    quarter_n_polygons = polygons->n_items / 4;

    create_tetrahedral_sphere( &dummy_centre, 1.0, 1.0, 1.0,
                               quarter_n_polygons, half );

    for_less( i, 0, half->n_points )
    {
        half->points[i] = polygons->points[i];
        half->normals[i] = polygons->normals[i];
    }

    if( polygons->colour_flag == PER_VERTEX_COLOURS )
    {
        half->colour_flag = PER_VERTEX_COLOURS;
        REALLOC( half->colours, half->n_points );
        for_less( i, 0, half->n_points )
            half->colours[i] = polygons->colours[i];
    }
}
