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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/tetrahedrons.c,v 1.8 1996-04-25 20:17:49 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : is_this_tetrahedral_topology
@INPUT      : polygons
@OUTPUT     : 
@RETURNS    : TRUE if correct topology
@DESCRIPTION: Tests if the polygons are a tetrahedron (4 polygons),
              two tetrahedrons glued together (6 polygons), two pyramids glued
              together (8 polygons) or a subdivision of one of these.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  is_this_tetrahedral_topology(
    polygons_struct   *polygons )
{
    int    n_polygons;

    n_polygons = polygons->n_items;

    while( n_polygons > 8 && n_polygons % 4 == 0 )
         n_polygons /= 4;

    return( n_polygons == 4 || n_polygons == 6 || n_polygons == 8 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_tetra_tessellation_with_n_points
@INPUT      : n_points
@OUTPUT     : 
@RETURNS    : tessellation number
@DESCRIPTION: Returns the number of polygons that correspond to a tetrahedral
              topology with n_points points.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_tetra_tessellation_with_n_points(
    int   n_points )
{
    int  n_polygons;

    n_polygons = 2 * (n_points - 2);

    return( n_polygons );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_tetrahedral_sphere
@INPUT      : centre
              rx
              ry
              rz
              n_triangles
@OUTPUT     : polygons
@RETURNS    : 
@DESCRIPTION: Creates a tetrahedral tessellation of a sphere.  This is
              either a tetrahedron, two tetrahedrons glued together, or
              two pyramids glued together, or some subdivision of these.
              This means that n_triangles must be either 4, 6, 8, or
              a power of 4 times one of these, since a subdivision step
              quadruples the number of triangles.  If it is not, then the
              smallest power of 4 larger than n_triangles will be used,
              resulting in the subdivision of a tetrahedron.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

    /*--- determine the base object, either 4, 6, or 8 */

    start_size = n_triangles;

    while( start_size > 8 )
        start_size /= 4;

    cx = (Real) Point_x( *centre );
    cy = (Real) Point_y( *centre );
    cz = (Real) Point_z( *centre );

    if( start_size == 6 )  /* create 2 points at poles and 3 around equator */
    {
        create_polygons_sphere( centre, rx, ry, rz, 2, 3, FALSE, polygons );
    }
    else if( start_size == 8 ) /* create 2 points at poles and 4 around equat */
    {
        create_polygons_sphere( centre, rx, ry, rz, 2, 4, FALSE, polygons );
    }
    else                       /* create a tetrahedron */
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

    /*--- having created the base object, subdivide it until we have the
          required number of triangles */

    while( polygons->n_items < n_triangles )
    {
        subdivide_polygons( polygons );

        /*--- project the points to the ellipsoid */

        for_less( p, 0, polygons->n_points )
        {
            dx = (Real) Point_x(polygons->points[p]) - cx;
            dy = (Real) Point_y(polygons->points[p]) - cy;
            dz = (Real) Point_z(polygons->points[p]) - cz;
            scale = dx * dx / rx / rx + dy * dy / ry / ry + dz * dz / rz / rz;
            scale = 1.0 / sqrt( scale );
            dx *= scale;
            dy *= scale;
            dz *= scale;
            fill_Point( polygons->points[p], cx + dx, cy + dy, cz + dz );
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : half_sample_tetrahedral_tessellation
@INPUT      : polygons
@OUTPUT     : half
@RETURNS    : 
@DESCRIPTION: Half samples the tetrahedron tesselation by combining every
              set of 4 triangles into 1.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  half_sample_tetrahedral_tessellation(
    polygons_struct  *polygons,
    polygons_struct  *half )
{
    int             i, quarter_n_polygons;
    static  Point   dummy_centre = { 0.0f, 0.0f, 0.0f };

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
