
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

    quadmesh->bintree = NULL;
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

    if( quadmesh->bintree != NULL )
        delete_quadmesh_bintree( quadmesh );   
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

public  void  create_quadmesh_bintree(
    quadmesh_struct   *quadmesh,
    int               max_nodes )
{
    int              i, j, m, n, obj_index;
    range_struct     *bound_vols;
    Point            min_range, max_range;
    Point            points[4];

    ALLOC( quadmesh->bintree, 1 );

    get_quadmesh_n_objects( quadmesh, &m, &n );

    ALLOC( bound_vols, m * n );

    for_less( i, 0, m )
    {
        for_less( j, 0, n )
        {
            obj_index = IJ( i, j, n );
            get_quadmesh_patch( quadmesh, i, j, points );

            get_range_points( 4, points, &min_range, &max_range );

            bound_vols[obj_index].limits[X][0] = Point_x(min_range);
            bound_vols[obj_index].limits[Y][0] = Point_y(min_range);
            bound_vols[obj_index].limits[Z][0] = Point_z(min_range);
            bound_vols[obj_index].limits[X][1] = Point_x(max_range);
            bound_vols[obj_index].limits[Y][1] = Point_y(max_range);
            bound_vols[obj_index].limits[Z][1] = Point_z(max_range);
        }
    }

    create_object_bintree( m * n, bound_vols,
                           quadmesh->bintree, max_nodes );

    FREE( bound_vols );
}

public  void  delete_quadmesh_bintree(
    quadmesh_struct   *quadmesh )
{
    if( quadmesh->bintree != (bintree_struct *) 0 )
    {
        delete_bintree( quadmesh->bintree );

        FREE( quadmesh->bintree );
        quadmesh->bintree = (bintree_struct *) 0;
    }
}
