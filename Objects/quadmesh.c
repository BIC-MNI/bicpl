
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
    free_colours( quadmesh->colour_flag, quadmesh->colours,
                  quadmesh->m * quadmesh->n,
                  (quadmesh->m-1) * (quadmesh->n-1) );

    if( quadmesh->m > 0 && quadmesh->n > 0 )
    {
        FREE( quadmesh->points );

        if( quadmesh->normals != (Vector *) NULL )
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

public  void  compute_quadmesh_normals(
    quadmesh_struct  *quadmesh )
{
    int                i, j, m, n;
    Point              neighbours[4];
    progress_struct    progress;

    m = quadmesh->m;
    n = quadmesh->n;

    if( quadmesh->normals == (Vector *) NULL )
        ALLOC( quadmesh->normals, m * n );

    for_less( i, 0, m * n )
        fill_Vector( quadmesh->normals[i], 0.0, 0.0, 0.0 );

    initialize_progress_report( &progress, FALSE, m, "Computing Normals" );

    for_less( i, 1, m-1 )
    {
        for_less( j, 1, n-1 )
        {
            neighbours[0] = quadmesh->points[IJ(i,j-1,n)];
            neighbours[1] = quadmesh->points[IJ(i+1,j,n)];
            neighbours[2] = quadmesh->points[IJ(i,j+1,n)];
            neighbours[3] = quadmesh->points[IJ(i-1,j,n)];

            find_polygon_normal( 4, neighbours, &quadmesh->normals[IJ(i,j,n)] );
            NORMALIZE_VECTOR( quadmesh->normals[IJ(i,j,n)],
                              quadmesh->normals[IJ(i,j,n)] );
        }

        update_progress_report( &progress, i + 1 );
    }

    terminate_progress_report( &progress );
}
