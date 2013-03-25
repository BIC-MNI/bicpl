#ifndef  DEF_geom_prototypes
#define  DEF_geom_prototypes

public  void  get_closest_point_on_line_segment(
    Point  *point,
    Point  *p1,
    Point  *p2,
    Point  *closest_point );

public  VIO_Real  get_point_object_distance(
    Point                 *point,
    object_struct         *object,
    int                   obj_index,
    Point                 *object_point );

public  VIO_Real  find_closest_point_on_object(
    Point           *point,
    object_struct   *object,
    int             *obj_index,
    Point           *point_on_object );

public  void  get_polygon_vertex_curvatures(
    polygons_struct   *polygons,
    VIO_Real              smoothing_distance,
    VIO_Real              low_threshold,
    VIO_Real              curvatures[] );

public  void  generate_tube(
    int    n_points,
    Point  points[],
    int    n_around,
    VIO_Real   radius,
    Point  tube_points[],
    Vector tube_normals[] );

public  void  project_vector_to_plane(
    Vector   *v,
    Vector   *direction,
    Vector   *normal,
    Vector   *projected );

public  VIO_BOOL  intersect_lines_3d(
    Point   *origin1,
    Vector  *delta1,
    Point   *origin2,
    Vector  *delta2,
    Point   *intersection_pt );

public  void  find_polygon_normal(
    int      n_points,
    Point    points[],
    Vector   *normal );

public  VIO_Real  get_polygon_2d_area(
    int      n_points,
    Point    points[] );

public  void   get_plane_through_points(
    int      n_points,
    Point    points[],
    Vector   *normal,
    VIO_Real     *plane_constant );

public  VIO_Real  distance_from_plane(
    Point    *point,
    Vector   *plane_normal,
    VIO_Real     plane_constant );

public  VIO_Real  distance_from_line(
    Point    *point,
    Point    *end_point1,
    Point    *end_point2 );

public  VIO_BOOL  clip_line_to_box(
    Point    *origin,
    Vector   *direction,
    VIO_Real     x_min,
    VIO_Real     x_max,
    VIO_Real     y_min,
    VIO_Real     y_max,
    VIO_Real     z_min,
    VIO_Real     z_max,
    VIO_Real     *t_min,
    VIO_Real     *t_max );

public  VIO_BOOL   line_segment_intersects_plane(
    Point   *p1,
    Point   *p2,
    Point   *plane_origin,
    Vector  *plane_normal,
    Point   *intersection_point );

public  void  create_line_circle(
    Point            *centre,
    int              plane_axis,
    VIO_Real             x_size,
    VIO_Real             y_size,
    int              n_points,
    lines_struct     *lines );

public  VIO_BOOL  is_circle_topology(
    lines_struct   *lines );

public  void  get_polygon_interpolation_weights(
    Point       *point,
    int         n_points,
    Point       points[],
    VIO_Real        weights[] );

public  void  map_point_between_polygons(
    polygons_struct  *p1,
    int              poly_index,
    Point            *p1_point,
    polygons_struct  *p2,
    Point            *p2_point );

public  void  map_point_to_unit_sphere(
    polygons_struct  *p,
    Point            *point,
    polygons_struct  *unit_sphere,
    Point            *unit_sphere_point );

public  void  polygon_transform_points(
    polygons_struct   *src_polygons,
    polygons_struct   *dest_polygons,
    int               n_points,
    Point             src_points[],
    Point             dest_points[] );

public   void   find_path_between_polygons(
    int            polygon1,
    int            polygon2,
    int            n_polygons,
    int            end_indices[],
    Smallest_int   visibilities[],
    int            neighbours[],
    VIO_BOOL        *path_exists,
    int            *path_length,
    int            *path[] );

public  VIO_BOOL  null_Point(
    Point   *p );

public  VIO_BOOL  null_Vector(
    Vector   *v );

public  VIO_Real  distance_between_points(
    Point  *p1,
    Point  *p2 );

public  VIO_BOOL  points_within_distance(
    Point  *p1,
    Point  *p2,
    VIO_Real   distance );

public  void  apply_point_to_min_and_max(
    Point   *point,
    Point   *min_point,
    Point   *max_point );

public  void  expand_min_and_max_points(
    Point   *min_point,
    Point   *max_point,
    Point   *min_to_check,
    Point   *max_to_check );

public  void  get_range_points(
    int                n_points,
    Point              points[],
    Point              *min_corner,
    Point              *max_corner );

public  void  get_points_centroid(
    int     n_points,
    Point   points[],
    Point   *centroid );

public   void     reverse_vectors(
    int       n_vectors,
    Vector    vectors[] );

public  VIO_Real  get_angle_between_points(
    Point  *prev,
    Point  *this,
    Point  *next );

public  VIO_Real  find_point_polygon_distance(
    Point     *point,
    int       n_points,
    Point     poly_points[],
    Point     *closest_point );

public  int  find_closest_polygon_point(
    Point              *point,
    polygons_struct    *polygons,
    Point              *closest_point );

public  void  create_polygons_sphere(
    Point            *centre,
    VIO_Real             x_size,
    VIO_Real             y_size,
    VIO_Real             z_size,
    int              n_up,
    int              n_around,
    VIO_BOOL          subdividing_flag,
    polygons_struct  *polygons );

public  int  get_sphere_point_index(
    int   up,
    int   around,
    int   n_up,
    int   n_around );

public  VIO_BOOL  is_this_sphere_topology(
    polygons_struct  *polygons );

public  VIO_BOOL  get_tessellation_of_polygons_sphere(
    polygons_struct  *polygons,
    int              *tess );

public  int  get_tessellation_with_n_points(
    int   n_points );

public  void  half_sample_sphere_tessellation(
    polygons_struct   *polygons,
    polygons_struct   *half );

public  VIO_BOOL  point_within_polygon(
    Point   *pt,
    int     n_points,
    Point   points[],
    Vector  *polygon_normal );

public  VIO_BOOL  line_intersects_ellipsoid(
    Point    *line_origin,
    Vector   *line_direction,
    Point    *sphere_centre,
    VIO_Real     x_size,
    VIO_Real     y_size,
    VIO_Real     z_size,
    Point    *intersection );

public  VIO_BOOL  ray_intersects_sphere(
    Point       *origin,
    Vector      *direction,
    Point       *centre,
    VIO_Real        radius,
    VIO_Real        *dist );

public  void  intersect_ray_object(
    Point                 *origin,
    Vector                *direction,
    VIO_Real                  t_min,
    object_struct         *object,
    int                   obj_index,
    int                   *closest_obj_index,
    VIO_Real                  *closest_dist,
    int                   *n_intersections,
    VIO_Real                  *distances[] );

public  int  intersect_ray_with_object(
    Point           *origin,
    Vector          *direction,
    object_struct   *object,
    int             *obj_index,
    VIO_Real            *dist,
    VIO_Real            *distances[] );

public  void   remove_invisible_polygons(
    polygons_struct  *polygons );

public  VIO_Real  get_smooth_surface_curvature(
    polygons_struct   *polygons,
    int               poly,
    int               vertex,
    VIO_Real              smoothing_distance );

public  void  smooth_lines(
    lines_struct  *lines,
    VIO_Real          smooth_length );

public  void  create_line_spline(
    lines_struct  *lines,
    int           n_curve_segments,
    lines_struct  *new_lines );

public  void  smooth_polygon(
    polygons_struct  *polygons,
    VIO_Real             max_dist_from_original,
    VIO_Real             fraction_to_move,
    VIO_Real             stop_threshold,
    VIO_Real             normal_ratio,
    VIO_BOOL          range_flag,
    volume_struct    *volume,
    int              min_value,
    int              max_value );

public  VIO_BOOL is_single_closed_curve(
    lines_struct   *lines );

public  void  subdivide_lines(
    lines_struct  *lines );

public  void  subdivide_polygons(
    polygons_struct  *polygons );

public  VIO_Real  get_polygon_surface_area(
    int      n_points,
    Point    points[] );

public  VIO_Real  get_polygons_surface_area(
    polygons_struct  *polygons );

public  VIO_BOOL  is_this_tetrahedral_topology(
    polygons_struct   *polygons );

public  int  get_tetra_tessellation_with_n_points(
    int   n_points );

public  void  create_tetrahedral_sphere(
    Point            *centre,
    VIO_Real             rx,
    VIO_Real             ry,
    VIO_Real             rz,
    int              n_triangles,
    polygons_struct  *polygons );

public  void  half_sample_tetrahedral_tessellation(
    polygons_struct  *polygons,
    polygons_struct  *half );

public  void  create_tube(
    int               n_points,
    Point             points[],
    int               n_around,
    VIO_Real              radius,
    Colour            colour,
    quadmesh_struct   *quadmesh );

public  int  convert_lines_to_tubes(
    lines_struct     *lines,
    int              n_around,
    VIO_Real             radius,
    quadmesh_struct  *quadmeshes[] );

public  void   create_slice_quadmesh(
    Volume           volume,
    int              axis_index,
    VIO_Real             voxel_position,
    int              x_tess,
    int              y_tess,
    VIO_Real             x_min,
    VIO_Real             x_max,
    VIO_Real             y_min,
    VIO_Real             y_max,
    quadmesh_struct  *quadmesh );

public  void   create_slice_3d(
    Volume           volume,
    Point            *origin,
    Vector           *normal,
    polygons_struct  *polygons );
#endif
