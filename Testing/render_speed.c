#include  <bicpl.h>

int  main(
    int   argc,
    char  *argv[] )
{
    int            n_alloced, x_size, y_size, i, sizes[MAX_DIMENSIONS];
    Real           intensity, separations[MAX_DIMENSIONS];
    Real           min_value, max_value;
    Real           x_axis[N_DIMENSIONS];
    Real           y_axis[N_DIMENSIONS];
    Real           origin[N_DIMENSIONS];
    int            used_x_viewport_size, used_y_viewport_size, n_iters;
    int            n_alloced2, x, y, j;
    Real           start_time, end_time;
    Real           alpha1, alpha2;
    pixels_struct  pixels, pixels2;
    Volume         volume;
    Real           x_scale, y_scale, x_translation, y_translation;
    Real           slice_fit_oversize = 0.1;
    Real           r1, g1, b1, a1, r2, g2, b2, a2;
    Colour         **rgb_map, c1, c2, *src, *dest;
    char           *filename;
    static char    *dim_names[] = { MIxspace, MIyspace, MIzspace };

    initialize_argument_processing( argc, argv );
    (void) get_string_argument( "$AVG", &filename );
    (void) get_int_argument( 10, &n_iters );
    (void) get_int_argument( 500, &x_size );
    (void) get_int_argument( x_size, &y_size );

    if( input_volume( filename, 3, dim_names, NC_UNSPECIFIED, FALSE,
                           0.0, 0.0, TRUE, &volume,
                           (minc_input_options *) NULL ) != OK )
        return( 1 );

    get_volume_voxel_range( volume, &min_value, &max_value );

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );

    n_alloced = 0;
    n_alloced2 = 0;

    get_volume_voxel_range( volume, &min_value, &max_value );
    ALLOC2D( rgb_map, (int)max_value+1, (int) max_value+1 );

/*    for_less( i, 0, (int) max_value+1 ) */

    for_less( i, 0, 1 )
    for_less( j, 0, (int) max_value+1 )
    {
        intensity = (Real) i / max_value;
        rgb_map[i][j] = make_Colour_0_1( intensity, intensity, intensity );
    }

    origin[X] = 0.0;
    origin[Y] = 0.0;
    origin[Z] = (Real) (sizes[Z] - 1) / 2.0;
    x_axis[X] = 1.0;
    x_axis[Y] = 0.0;
    x_axis[Z] = 0.0;
    y_axis[X] = 0.0;
    y_axis[Y] = 1.0;
    y_axis[Z] = 0.0;
    fit_volume_slice_to_viewport( volume, origin, x_axis, y_axis,
                                  x_size, y_size, slice_fit_oversize,
                                  &x_translation, &y_translation,
                                  &x_scale, &y_scale,
                                  &used_x_viewport_size, &used_y_viewport_size);

    start_time = current_cpu_seconds();

    for_less( i, 0, n_iters )
    {
        create_volume_slice( volume, NEAREST_NEIGHBOUR, 0.0,
                             origin, x_axis, y_axis,
                             x_translation, y_translation,
                             x_scale, y_scale,
                             NULL, BOX_FILTER, 0.0,
                             origin, x_axis, y_axis,
                             x_translation, y_translation,
                             x_scale, y_scale,
                             x_size, y_size, 0, -1, 0, -1, RGB_PIXEL, -1,
                             (unsigned short **) NULL,
                             rgb_map, BLACK, NULL, TRUE, &n_alloced, &pixels );
    }

    end_time = current_cpu_seconds();

    print( "Time: %g per render.\n", (end_time - start_time) / (Real) n_iters );

return( 0 );

    start_time = current_cpu_seconds();

    for_less( i, 0, n_iters )
    {
        create_volume_slice( volume, BOX_FILTER, 0.0,
                             origin, x_axis, y_axis,
                             x_translation, y_translation,
                             x_scale, y_scale,
                             NULL, BOX_FILTER, 0.0,
                             NULL, NULL, NULL,
                             0.0, 0.0, 0.0, 0.0,
                             x_size, y_size, 0, -1, 0, -1, RGB_PIXEL, -1,
                             (unsigned short **) NULL,
                             rgb_map, BLACK, NULL, TRUE, &n_alloced, &pixels );

        create_volume_slice( volume, BOX_FILTER, 0.0,
                             origin, x_axis, y_axis,
                             x_translation, y_translation,
                             x_scale, y_scale,
                             (Volume) NULL, BOX_FILTER, 0.0,
                             (Real *) NULL, (Real *) NULL, (Real *) NULL,
                             0.0, 0.0, 0.0, 0.0,
                             x_size, y_size, 0, -1, 0, -1, RGB_PIXEL, -1,
                             (unsigned short **) NULL,
                             rgb_map, BLACK, NULL, TRUE, &n_alloced2, &pixels2);

        alpha1 = 0.3;
        alpha2 = 0.7;

        for_less( y, 0, pixels.y_size )
        {
            src = &PIXEL_RGB_COLOUR( pixels, 0, y );
            dest = &PIXEL_RGB_COLOUR( pixels, 0, y );

            for_less( x, 0, pixels.x_size )
            {
                c1 = *dest;
                c2 = *src;

                if( alpha1 != 1.0 )
                {
                    r1 = (Real) get_Colour_r( c1 );
                    g1 = (Real) get_Colour_g( c1 );
                    b1 = (Real) get_Colour_b( c1 );
                    a1 = (Real) get_Colour_a( c1 );

                    r2 = (Real) get_Colour_r( c2 );
                    g2 = (Real) get_Colour_g( c2 );
                    b2 = (Real) get_Colour_b( c2 );
                    a2 = (Real) get_Colour_a( c2 );

                    *dest = make_rgba_Colour(
                            (int) (alpha1 * r1 + alpha2 * r2),
                            (int) (alpha1 * g1 + alpha2 * g2),
                            (int) (alpha1 * b1 + alpha2 * b2),
                            (int) (alpha1 * a1 + alpha2 * a2));
                }

                ++src;
                ++dest;
            }
        }
    }

    end_time = current_cpu_seconds();

    print( "Time: %g per render.\n", (end_time - start_time) / (Real) n_iters );

    return( 0 );
}
