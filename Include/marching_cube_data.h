#ifndef  DEF_MARCHING_CUBES_DATA
#define  DEF_MARCHING_CUBES_DATA

/*  @(#)def_marching_cube_data.h	1.1  2/26/91  */

#include  <def_marching_cubes.h>

typedef struct
{

    int                  n_triangles;
    voxel_point_type     triangle_points[MAX_POINTS_PER_VOXEL];

}  marching_cube_data_struct;

#endif
