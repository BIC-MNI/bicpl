
#include  <module.h>

public  void  create_tube(
    int               n_points,
    Point             points[],
    int               n_around,
    Real              radius,
    Colour            colour,
    quadmesh_struct   *quadmesh )
{
    Surfprop    spr;

    get_default_surfprop( &spr );

    initialize_quadmesh( quadmesh, colour, &spr, n_points, n_around );
    quadmesh->m_closed = FALSE;
    quadmesh->n_closed = TRUE;

    generate_tube( n_points, points, n_around, radius,
                   quadmesh->points, quadmesh->normals );
}

public  int  convert_lines_to_tubes(
    lines_struct     *lines,
    int              n_around,
    Real             radius,
    quadmesh_struct  *quadmeshes[] )
{
    int              i, l, line_size;
    quadmesh_struct  *quadmesh;
    Point            *points;

    if( lines->n_items == 0 )
        return( 0 );

    ALLOC( *quadmeshes, lines->n_items );

    for_less( l, 0, lines->n_items )
    {
        line_size = GET_OBJECT_SIZE( *lines, l );

        ALLOC( points, line_size );

        for_less( i, 0, line_size )
        {
            points[i] = lines->points[lines->indices[
                           POINT_INDEX(lines->end_indices,l,i)]];
        }

        quadmesh = &(*quadmeshes)[l];

        create_tube( line_size, points, n_around, radius, lines->colours[0],
                     quadmesh );

        FREE( points );
    }

    return( lines->n_items );
}
