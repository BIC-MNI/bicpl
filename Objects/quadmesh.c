
#include  <mni.h>

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

    quadmesh->bintree = (bintree_struct_ptr) NULL;
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

    delete_bintree_if_any( &quadmesh->bintree );   
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

public  void  get_quadmesh_n_objects(
    quadmesh_struct  *quadmesh,
    int              *m,
    int              *n )
{
    if( quadmesh->m_closed )
        *m = quadmesh->m;
    else
        *m = quadmesh->m - 1;

    if( quadmesh->n_closed )
        *n = quadmesh->n;
    else
        *n = quadmesh->n - 1;
}

private  BOOLEAN  get_quadmesh_point(
    quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    Point            *point )
{
    if( (i == -1 || i == quadmesh->m) && !quadmesh->m_closed )
        return( FALSE );

    if( (j == -1 || j == quadmesh->n) && !quadmesh->n_closed )
        return( FALSE );

    i = (i + quadmesh->m) % quadmesh->m;
    j = (j + quadmesh->n) % quadmesh->n;

    *point = quadmesh->points[IJ(i,j,quadmesh->n)];

    return( TRUE );
}
    
public  void  compute_quadmesh_normals(
    quadmesh_struct  *quadmesh )
{
    int                i, j, m, n, n_neighs;
    Point              neighbours[4];
    progress_struct    progress;

    m = quadmesh->m;
    n = quadmesh->n;

    if( quadmesh->normals == (Vector *) NULL )
        ALLOC( quadmesh->normals, m * n );

    for_less( i, 0, m * n )
        fill_Vector( quadmesh->normals[i], 0.0, 0.0, 0.0 );

    initialize_progress_report( &progress, FALSE, m, "Computing Normals" );

    for_less( i, 0, m )
    {
        for_less( j, 0, n )
        {
            n_neighs = 0;
            if( get_quadmesh_point( quadmesh, i,   j-1, &neighbours[n_neighs]) )
                ++n_neighs;
            if( get_quadmesh_point( quadmesh, i+1, j  , &neighbours[n_neighs]) )
                ++n_neighs;
            if( get_quadmesh_point( quadmesh, i,   j+1, &neighbours[n_neighs]) )
                ++n_neighs;
            if( get_quadmesh_point( quadmesh, i-1, j  , &neighbours[n_neighs]) )
                ++n_neighs;

            if( n_neighs < 2 )
            {
                HANDLE_INTERNAL_ERROR( "compute_quadmesh_normals" );
            }
            else if( n_neighs == 2 )
            {
                neighbours[n_neighs] = quadmesh->points[IJ(i,j,n)];
                ++n_neighs;
            }

            find_polygon_normal( n_neighs, neighbours,
                                 &quadmesh->normals[IJ(i,j,n)] );
            NORMALIZE_VECTOR( quadmesh->normals[IJ(i,j,n)],
                              quadmesh->normals[IJ(i,j,n)] );
        }

        update_progress_report( &progress, i + 1 );
    }

    terminate_progress_report( &progress );
}

public  void  get_quadmesh_patch_indices(
    quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    int              indices[] )
{
    indices[0] = IJ(                   i,                   j, quadmesh->n );
    indices[1] = IJ( (i+1) % quadmesh->m,                   j, quadmesh->n );
    indices[2] = IJ( (i+1) % quadmesh->m, (j+1) % quadmesh->n, quadmesh->n );
    indices[3] = IJ(                   i, (j+1) % quadmesh->n, quadmesh->n );
}

public  void  get_quadmesh_patch(
    quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    Point            points[] )
{
    int   p, indices[4];

    get_quadmesh_patch_indices( quadmesh, i, j, indices );

    for_less( p, 0, 4 )
        points[p] = quadmesh->points[indices[p]];
}
