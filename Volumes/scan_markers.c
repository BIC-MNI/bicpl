
#include  <internal_volume_io.h>
#include  <bicpl.h>

public  void  scan_marker_to_voxels(
    marker_struct    *marker,
    Volume           volume,
    Volume           label_volume,
    int              label )
{
    Real           low[N_DIMENSIONS], high[N_DIMENSIONS];
    int            min_voxel[N_DIMENSIONS], max_voxel[N_DIMENSIONS];
    Real           voxel[N_DIMENSIONS];
    int            c, int_voxel[N_DIMENSIONS];

    convert_world_to_voxel( volume,
                            Point_x(marker->position) - marker->size,
                            Point_y(marker->position) - marker->size,
                            Point_z(marker->position) - marker->size,
                            low );

    convert_world_to_voxel( volume,
                            Point_x(marker->position) + marker->size,
                            Point_y(marker->position) + marker->size,
                            Point_z(marker->position) + marker->size,
                            high );

    for_less( c, 0, N_DIMENSIONS )
    {
        if( IS_INT(low[c]) )
            min_voxel[c] = (int) low[c];
        else
            min_voxel[c] = (int) low[c] + 1;
        max_voxel[c] = (int) high[c];

        if( min_voxel[c] > max_voxel[c] )
        {
            min_voxel[c] = ROUND( (low[c] + high[c]) / 2.0 );
            max_voxel[c] = min_voxel[c];
        }
    }

    for_inclusive( voxel[X], min_voxel[X], max_voxel[X] )
    {
        for_inclusive( voxel[Y], min_voxel[Y], max_voxel[Y] )
        {
            for_inclusive( voxel[Z], min_voxel[Z], max_voxel[Z] )
            {
                if( voxel_is_within_volume( volume, voxel ) )

                {
                    convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
                    set_volume_label_data( label_volume, int_voxel, label );
                }
            }
        }
    }
}
