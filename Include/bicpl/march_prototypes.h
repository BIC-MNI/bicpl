#ifndef  DEF_march_prototypes
#define  DEF_march_prototypes

#ifdef __cplusplus
extern "C" {
#endif

public  int  compute_isosurface_in_voxel(
    Marching_cubes_methods  method,
    int                     x,
    int                     y,
    int                     z,
    Real                    corners[2][2][2],
    BOOLEAN                 binary_flag,
    Real                    min_value,
    Real                    max_value,
    int                     *sizes[],
    voxel_point_type        *points[] );

public  Point_classes  get_isosurface_point(
    Real              corners[2][2][2],
    int               voxel[],
    int               edge_intersected,
    BOOLEAN           binary_flag,
    Real              min_value,
    Real              max_value,
    Real              point[] );

public  int  get_max_marching_edges(
    Marching_cubes_methods  method );

public  int  get_max_marching_polygons_per_voxel(
    Marching_cubes_methods  method );

public  int  compute_isotriangles_in_voxel(
    Real                corner_values[2][2][2],
    Real                target_value,
    voxel_point_type    *points_list_ptr[] );

public  int  get_holeless_isosurface_polygons(
    Real              corners[2][2][2],
    Real              isovalue,
    int               *sizes[],
    voxel_point_type  *points[] );

public  void  delete_marching_cubes_table( void );

public  void  translate_to_edge_index(
    int                x1,
    int                y1,
    int                z1,
    int                x2,
    int                y2,
    int                z2,
    voxel_point_type   *edge_point );

public  void  translate_from_edge_index(
    int       edge_index,
    int       offset[] );

public  int  get_tetra_isosurface_polygons(
    int               x,
    int               y,
    int               z,
    Real              corners[2][2][2],
    Real              isovalue,
    int               *sizes[],
    voxel_point_type  *points[] );

public  void  delete_tetra_marching_cubes_table( void );

#ifdef __cplusplus
}
#endif

#endif
