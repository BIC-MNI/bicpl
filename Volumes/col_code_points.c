#include  <internal_volume_io.h>
#include  <vols.h>

/* -------------------------------------------------------------------- */

private  void  colour_code_points(
    colour_coding_struct  *colour_coding,
    Volume                volume,
    int                   continuity,
    Colour_flags          *colour_flag,
    Colour                *colours[],
    int                   n_points,
    Point                 points[] )
{
    int      i;
    Real     val;

    if( *colour_flag != PER_VERTEX_COLOURS )
    {
        if( n_points > 0 )
        {
            REALLOC( *colours, n_points );
        }
        else
        {
            FREE( *colours );
        }
        *colour_flag = PER_VERTEX_COLOURS;
    }

    for_less( i, 0, n_points )
    {
        evaluate_3D_volume_in_world( volume,
                                     Point_x(points[i]),
                                     Point_y(points[i]),
                                     Point_z(points[i]), continuity,
                                     &val, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL,
                                     (Real *) NULL, (Real *) NULL );

        (*colours)[i] = get_colour_code( colour_coding, val );
    }
}

public  void  colour_code_object(
    Volume                 volume,
    int                    continuity,
    colour_coding_struct   *colour_coding,
    object_struct          *object )
{
    polygons_struct         *polygons;
    quadmesh_struct         *quadmesh;
    lines_struct            *lines;

    switch( object->object_type )
    {
    case POLYGONS:
        polygons = get_polygons_ptr( object );
        colour_code_points( colour_coding, volume, continuity,
                            &polygons->colour_flag, &polygons->colours,
                            polygons->n_points, polygons->points );
        break;

    case QUADMESH:
        quadmesh = get_quadmesh_ptr( object );
        colour_code_points( colour_coding, volume, continuity,
                            &quadmesh->colour_flag, &quadmesh->colours,
                            quadmesh->m * quadmesh->n,
                            quadmesh->points );
        break;

    case LINES:
        lines = get_lines_ptr( object );
        colour_code_points( colour_coding, volume, continuity,
                            &lines->colour_flag, &lines->colours,
                            lines->n_points, lines->points );
        break;
    }
}
