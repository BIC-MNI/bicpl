
#include  <mni.h>

public  void  create_tube(
    int               n_points,
    Point             points[],
    int               n_around,
    VIO_Real              radius,
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
