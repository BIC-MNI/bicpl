
#include  <def_mni.h>

public  void  initialize_quadmesh(
    quadmesh_struct   *quadmesh,
    Colour            col,
    Surfprop          *spr,
    int               m,
    int               n )
{
    ALLOC( quadmesh->colours, 1 );

    quadmesh->colour_flag = ONE_COLOUR;

    quadmesh->colours[0] = col;

    if( spr != (Surfprop *) 0 )
        quadmesh->surfprop = *spr;

    quadmesh->m = m;
    quadmesh->n = n;
    quadmesh->m_closed = FALSE;
    quadmesh->n_closed = FALSE;
    if( quadmesh->m > 0 && quadmesh->n > 0 )
    {
        ALLOC( quadmesh->points, m * n );
        ALLOC( quadmesh->normals, m * n );
    }
}

public  void  delete_quadmesh(
    quadmesh_struct *quadmesh )
{
    FREE( quadmesh->colours );

    if( quadmesh->m > 0 && quadmesh->n > 0 )
    {
        FREE( quadmesh->points );
        FREE( quadmesh->normals );
    }
}

public  void  set_quadmesh_point(
    quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    Point            *point,
    Vector           *normal )
{
    int  ind;

    ind = IJ( i, j, quadmesh->n );

    quadmesh->points[ind] = *point;
    if( normal != (Vector *) NULL )
        quadmesh->normals[ind] = *normal;
}
