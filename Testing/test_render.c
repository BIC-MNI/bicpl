#include  <bicpl.h>

int  main(
    int   argc,
    char  *argv[] )
{
    int            n_alloced, x_size, y_size, i, sizes[MAX_DIMENSIONS];
    VIO_Real           intensity, separations[MAX_DIMENSIONS];
    VIO_Real           min_value, max_value;
    VIO_Real           x_axis[N_DIMENSIONS];
    VIO_Real           y_axis[N_DIMENSIONS];
    VIO_Real           origin[N_DIMENSIONS];
    int            used_x_viewport_size, used_y_viewport_size, n_iters;
    VIO_Real           start_time, end_time;
    pixels_struct  pixels;
    Volume         volume;
    VIO_Real           x_scale, y_scale, x_translation, y_translation;
    VIO_Real           slice_fit_oversize = 0.1;
    Colour         *rgb_map;
    char           *filename;
    static char    *dim_names[] = { MIxspace, MIyspace, MIzspace };

    initialize_argument_processing( argc, argv );
    (void) get_string_argument( "$AVG", &filename );
    (void) get_int_argument( 10, &n_iters );

    if( input_volume( filename, 3, dim_names, NC_UNSPECIFIED, FALSE,
                           0.0, 0.0, TRUE, &volume,
                           (minc_input_options *) NULL ) != OK )
        return( 1 );

    get_volume_voxel_range( volume, &min_value, &max_value );

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );

    x_size = 500;
    y_size = 500;

    n_alloced = 0;

    get_volume_voxel_range( volume, &min_value, &max_value );
    ALLOC( rgb_map, (int)max_value+1 );

    for_less( i, 0, (int) max_value+1 )
    {
        intensity = (VIO_Real) i / max_value;
        rgb_map[i] = make_Colour_0_1( intensity, intensity, intensity );
    }

    origin[X] = 0.0;
    origin[Y] = 0.0;
    origin[Z] = (VIO_Real) (sizes[Z] - 1) / 2.0;
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
        create_volume_slice( volume, BOX_FILTER, 0.0,
                         origin, x_axis, y_axis,
                         x_translation, y_translation,
                         x_scale, y_scale,
                         (Volume) NULL, BOX_FILTER, 0.0,
                         (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL,
                         0.0, 0.0, 0.0, 0.0,
                         x_size, y_size, 0, -1, 0, -1, RGB_PIXEL, FALSE,
                         (unsigned short **) NULL,
                         &rgb_map, BLACK, NULL, FALSE, &n_alloced, &pixels );
    }

    end_time = current_cpu_seconds();

    print( "Time: %g per render.\n", (end_time - start_time) / (VIO_Real) n_iters );

    return( 0 );
}
