#include  <module.h>

public  Real  get_polygon_surface_area(
    int      n_points,
    Point    points[] )
{
    int    i;
    Vector prev, this, cross_prod, sum;
    Real   surface_area;

    fill_Vector( sum, 0.0, 0.0, 0.0 );
    SUB_VECTORS( this, points[1], points[0] );

    for_less( i, 2, n_points )
    {
        prev = this;
        SUB_VECTORS( this, points[i], points[0] );
        CROSS_VECTORS( cross_prod, prev, this );
        ADD_VECTORS( sum, sum, cross_prod );
    }

    surface_area = MAGNITUDE( sum ) / 2.0;

    return( surface_area );
}

public  Real  get_polygons_surface_area(
    polygons_struct  *polygons )
{
    int      poly, size;
    Point    points[MAX_POINTS_PER_POLYGON];
    Real     surface_area;

    surface_area = 0.0;

    for_less( poly, 0, polygons->n_items )
    {
        size = get_polygon_points( polygons, poly, points );
        surface_area += get_polygon_surface_area( size, points );
    }

    return( surface_area );
}
