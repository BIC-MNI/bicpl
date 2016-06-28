#ifndef  DEF_object_prototypes
#define  DEF_object_prototypes


#ifdef __cplusplus
extern "C" {
#endif

BICAPI  void  coalesce_object_points(
    int      *n_points,
    VIO_Point    *points[],
    int      n_indices,
    int      indices[] );

BICAPI  void  separate_object_points(
    int           *new_n_points,
    VIO_Point         *points[],
    int           n_indices,
    int           indices[],
    Colour_flags  colour_flag,
    VIO_Colour        *colours[] );

BICAPI  void  get_default_surfprop(
    VIO_Surfprop  *surfprop );

BICAPI  VIO_BOOL  lookup_colour(
    VIO_STR    colour_name,
    VIO_Colour    *col );

BICAPI  VIO_BOOL  lookup_colour_name(
    VIO_Colour  col,
    VIO_STR  *colour_name );

BICAPI  void  rgb_to_hsl(
    VIO_Real    r,
    VIO_Real    g,
    VIO_Real    b,
    VIO_Real    *h,
    VIO_Real    *s,
    VIO_Real    *l );

BICAPI  void  hsl_to_rgb(
    VIO_Real   h,
    VIO_Real   sl,
    VIO_Real   l,
    VIO_Real   *r,
    VIO_Real   *g,
    VIO_Real   *b );

BICAPI  void  convert_colour_to_hsl(
    VIO_Colour   rgb,
    VIO_Colour   *hsl );

BICAPI  void  convert_colour_to_rgb(
    VIO_Colour   hsl,
    VIO_Colour   *rgb );

BICAPI  VIO_BOOL  equal_colours(
    VIO_Colour  col1,
    VIO_Colour  col2 );

BICAPI  VIO_STR  convert_colour_to_string(
    VIO_Colour   col );

BICAPI  VIO_Colour  convert_string_to_colour(
    VIO_STR     string );

BICAPI  int  get_colour_distance(
    int      r,
    int      g,
    int      b,
    VIO_Colour   c2 );

BICAPI  int   find_closest_colour(
    int     r,
    int     g,
    int     b,
    int     n_colours,
    VIO_Colour  colours[] );

BICAPI  int  get_Colour_luminance(
    VIO_Colour   colour );

BICAPI  VIO_Colour  scale_colour(
    VIO_Colour   colour,
    VIO_Real     factor );

BICAPI  VIO_Status   input_graphics_file(
    VIO_STR         filename,
    VIO_File_formats   *format,
    int            *n_objects,
    object_struct  ***object_list );

BICAPI  VIO_Status   output_graphics_file(
    VIO_STR         filename,
    VIO_File_formats   format,
    int            n_objects,
    object_struct  *object_list[] );

BICAPI  VIO_Status   input_objects_any_format(
    VIO_Volume         volume,
    VIO_STR         filename,
    VIO_Colour         marker_colour,
    VIO_Real           marker_size,
    Marker_types   marker_type,
    int            *n_objects,
    object_struct  **object_list[] );

BICAPI  VIO_STR  get_default_landmark_file_suffix( void );

BICAPI  VIO_Status   input_landmark_file(
    VIO_Volume         volume,
    VIO_STR         filename,
    VIO_Colour         colour,
    VIO_Real           size,
    Marker_types   type,
    int            *n_objects,
    object_struct  **object_list[] );

BICAPI  VIO_Status  io_tag_point(
    FILE            *file,
    VIO_IO_types        io_direction,
    VIO_Volume          volume,
    VIO_Real            size,
    marker_struct   *marker );

BICAPI  void  initialize_lines(
    lines_struct    *lines,
    VIO_Colour          col );

BICAPI  void  initialize_lines_with_size(
    lines_struct    *lines,
    VIO_Colour          col,
    int             size,
    VIO_BOOL         closed );

BICAPI  void  delete_lines( lines_struct *lines );

BICAPI  void  start_new_line( lines_struct *lines );

BICAPI  void  add_point_to_line(
    lines_struct   *lines,
    VIO_Point          *point );

BICAPI  void  get_line_segment_index(
    lines_struct  *lines,
    int           obj_index,
    int           *line,
    int           *seg );

BICAPI  void  set_bintree_delete_function(
    void  (*func)( bintree_struct_ptr * ) );

BICAPI  void  delete_bintree_if_any(
    bintree_struct_ptr   *bintree );

BICAPI  VIO_Real  get_lines_length(
    lines_struct  *lines );

BICAPI  void  get_lines_arc_point(
    lines_struct  *lines,
    VIO_Real          arc_length,
    VIO_Point         *point );

BICAPI  void   initialize_marker(
    marker_struct     *marker,
    Marker_types      type,
    VIO_Colour            colour );

BICAPI  void   delete_marker(
    marker_struct     *marker );

BICAPI  void  initialize_model(
    model_struct  *model );

BICAPI  void  delete_model(
    model_struct   *model );

BICAPI  void  assign_model_extra_ptr(
    model_struct   *model,
    void           *ptr );

BICAPI  void  *get_model_extra_ptr(
    model_struct   *model );

BICAPI  void  add_object_to_list(
    int            *n_objects,
    object_struct  ***object_list,
    object_struct  *new_object );

BICAPI  void  remove_object_from_list(
    int            *n_objects,
    object_struct  **object_list[],
    int            obj_index );

BICAPI  void  delete_object_list(
    int              n_objects,
    object_struct    *object_list[] );

BICAPI  int  find_object_index_in_model(
    model_struct   *model,
    object_struct  *object );

BICAPI  void  add_object_to_model(
    model_struct   *model,
    object_struct  *new_object );

BICAPI  void  insert_object_in_model(
    model_struct   *model,
    object_struct  *new_object,
    int            obj_index );

BICAPI  void  remove_ith_object_from_model(
    model_struct   *model,
    int            obj_index );

BICAPI  void  remove_object_from_model(
    model_struct   *model,
    object_struct  *object );

BICAPI  VIO_Status  io_lines(
    FILE                *file,
    VIO_IO_types            io_flag,
    VIO_File_formats        format,
    lines_struct        *lines );

BICAPI  VIO_Status  io_marker(
    FILE                *file,
    VIO_IO_types            io_flag,
    VIO_File_formats        format,
    marker_struct       *marker );

BICAPI  VIO_Status  io_model(
    FILE            *file,
    VIO_IO_types        io_flag,
    VIO_File_formats    format,
    model_struct    *model );

BICAPI  VIO_Status  io_pixels(
    FILE                *file,
    VIO_IO_types            io_flag,
    VIO_File_formats        format,
    pixels_struct       *pixels );

BICAPI  void  set_use_compressed_polygons_flag(
    VIO_BOOL  value );

BICAPI  VIO_BOOL  get_use_compressed_polygons_flag( void );

BICAPI  VIO_Status  io_polygons(
    FILE                *file,
    VIO_IO_types            io_flag,
    VIO_File_formats        format,
    polygons_struct     *polygons );

BICAPI  VIO_Status  io_quadmesh(
    FILE                *file,
    VIO_IO_types            io_flag,
    VIO_File_formats        format,
    quadmesh_struct     *quadmesh );

BICAPI  VIO_Status  io_text(
    FILE            *file,
    VIO_IO_types        io_flag,
    VIO_File_formats    format,
    text_struct     *text );

BICAPI  VIO_Status  io_point(
    FILE            *file,
    VIO_IO_types        io_flag,
    VIO_File_formats    format,
    VIO_Point           *point );

BICAPI  VIO_Status  io_vector(
    FILE            *file,
    VIO_IO_types        io_flag,
    VIO_File_formats    format,
    VIO_Vector          *v );

BICAPI  VIO_Status  io_colour(
    FILE            *file,
    VIO_IO_types        io_flag,
    VIO_File_formats    format,
    VIO_Colour          *colour );

BICAPI  VIO_Status  io_colours(
    FILE            *file,
    VIO_IO_types        io_flag,
    VIO_File_formats    format,
    Colour_flags    *colour_flag,
    int             n_items,
    int             n_points,
    VIO_Colour          **colours );

BICAPI  VIO_Status  io_surfprop(
    FILE            *file,
    VIO_IO_types        io_flag,
    VIO_File_formats    format,
    VIO_Surfprop        *surfprop );

BICAPI  VIO_Status  io_object_type(
    FILE           *file,
    VIO_IO_types       io_flag,
    VIO_File_formats   format,
    Object_types   type );

BICAPI  VIO_Status  input_object_type(
    FILE           *file,
    Object_types   *type,
    VIO_File_formats   *format,
    VIO_BOOL        *eof );

BICAPI  VIO_Status  io_pixel_colours(
    FILE            *file,
    VIO_IO_types        io_flag,
    VIO_File_formats    format,
    int             n,
    VIO_Colour          *pixel_colours[] );

BICAPI  VIO_Status  io_pixel_colour(
    FILE            *file,
    VIO_IO_types        io_flag,
    VIO_File_formats    format,
    VIO_Colour          *pixel_colour );

BICAPI  VIO_Status  input_object(
    VIO_STR         directory,
    FILE           *file,
    VIO_File_formats   *format,
    object_struct  **object,
    VIO_BOOL        *eof );

BICAPI  VIO_Status  output_object(
    FILE           *file,
    VIO_File_formats   format,
    object_struct  *object );

BICAPI  object_struct  *create_object(
    Object_types   object_type );

BICAPI  Object_types  get_object_type(
    object_struct   *object );

BICAPI  VIO_BOOL  get_object_visibility(
    object_struct  *object );

BICAPI  void  set_object_visibility(
    object_struct  *object,
    VIO_BOOL        visibility );

BICAPI  lines_struct  *get_lines_ptr(
    object_struct  *object );

BICAPI  marker_struct  *get_marker_ptr(
    object_struct  *object );

BICAPI  model_struct  *get_model_ptr(
    object_struct  *object );

BICAPI  pixels_struct  *get_pixels_ptr(
    object_struct  *object );

BICAPI  polygons_struct  *get_polygons_ptr(
    object_struct  *object );

BICAPI  quadmesh_struct  *get_quadmesh_ptr(
    object_struct  *object );

BICAPI  text_struct  *get_text_ptr(
    object_struct  *object );

BICAPI  void  delete_object(
    object_struct  *object );

BICAPI  int  get_object_points(
    object_struct   *object,
    VIO_Point           *points[] );

BICAPI  int  get_object_normals(
    object_struct   *object,
    VIO_Vector          *normals[] );

BICAPI  Colour_flags  *get_object_colours(
    object_struct   *object,
    VIO_Colour          *colours[] );

BICAPI  void  set_object_colours(
    object_struct   *object,
    VIO_Colour          colours[] );

BICAPI  VIO_Surfprop  *get_object_surfprop(
    object_struct   *object );

BICAPI  VIO_STR  get_object_name(
    object_struct  *object );

BICAPI  void  initialize_object_traverse(
    object_traverse_struct  *object_traverse,
    VIO_BOOL                 visible_ones_only,
    int                     n_objects,
    object_struct           *object_list[] );

BICAPI  VIO_BOOL  get_next_object_traverse(
    object_traverse_struct  *object_traverse,
    object_struct           **object );

BICAPI  void  terminate_object_traverse(
    object_traverse_struct   *object_traverse );

BICAPI  VIO_BOOL  get_range_of_object(
    object_struct   *object,
    VIO_BOOL         visible_ones_only,
    VIO_Point           *min_corner,
    VIO_Point           *max_corner );

BICAPI  void  reverse_object_normals(
    object_struct   *object );

BICAPI  VIO_BOOL  get_object_colour(
    object_struct  *object,
    VIO_Colour         *colour );

BICAPI  void  set_object_colour(
    object_struct  *object,
    VIO_Colour         col );

BICAPI  void  set_object_surfprop(
    object_struct  *object,
    VIO_Surfprop       *spr );

BICAPI  void  free_colours(
    Colour_flags   colour_flag,
    VIO_Colour         colours[],
    int            n_points,
    int            n_items );

BICAPI  VIO_BOOL  objects_are_same_topology(
    int    n_points1,
    int    n_items1,
    int    *end_indices1,
    int    *indices1,
    int    n_points2,
    int    n_items2,
    int    *end_indices2,
    int    *indices2 );

BICAPI  int  get_n_colours(
    Colour_flags  colour_flag,
    int           n_points,
    int           n_items );

BICAPI  void  initialize_pixels(
    pixels_struct  *pixels,
    int            x_position,
    int            y_position,
    int            x_size,
    int            y_size,
    VIO_Real           x_zoom,
    VIO_Real           y_zoom,
    Pixel_types    pixel_type );

BICAPI  void  delete_pixels( pixels_struct *pixels );

BICAPI  void  modify_pixels_size(
    int            *n_pixels_alloced,
    pixels_struct  *pixels,
    int            x_size,
    int            y_size,
    Pixel_types    pixel_type );

BICAPI  void  convert_pixels24_to_pixels8(
    pixels_struct   *pixels_rgb,
    pixels_struct   *pixels_8 );

BICAPI  void  convert_pixels24_to_index8(
    pixels_struct   *pixels_rgb,
    pixels_struct   *pixels_8,
    int             n_colours,
    VIO_Colour          colour_table[] );

BICAPI  void  convert_index8_to_pixels24(
    pixels_struct   *pixels_8,
    VIO_Colour          colour_table[],
    pixels_struct   *pixels_rgb );

BICAPI  void  convert_pixels24_to_gray_scale(
    pixels_struct   *pixels_rgb,
    pixels_struct   *pixels_8 );

BICAPI  void  convert_pixels24_to_dithered(
    pixels_struct   *pixels_rgb,
    pixels_struct   *pixels_8,
    int             n_colours,
    VIO_Colour          colour_table[] );

BICAPI  void  resample_pixels(
    pixels_struct   *pixels,
    VIO_Transform_2d    *transform,
    int             n_samples,
    pixels_struct   *new_pixels,
    VIO_Colour          background_colour );

BICAPI  void  copy_pixel_region(
    pixels_struct   *pixels,
    int             x_min,
    int             x_max,
    int             y_min,
    int             y_max,
    pixels_struct   *new_pixels );

BICAPI  void  check_polygons_neighbours_computed(
    polygons_struct   *polygons );

BICAPI  void  delete_polygon_point_neighbours(
    polygons_struct  *polygons,
    int              n_point_neighbours[],
    int              *point_neighbours[],
    VIO_SCHAR     interior_flags[],
    int              *point_polygons[] );

BICAPI   void   create_polygon_point_neighbours(
    polygons_struct  *polygons,
    VIO_BOOL          across_polygons_flag,
    int              *n_point_neighbours_ptr[],
    int              **point_neighbours_ptr[],
    VIO_SCHAR     *interior_flags_ptr[],
    int              **point_polygons_ptr[] );

BICAPI  void  initialize_polygons(
    polygons_struct   *polygons,
    VIO_Colour            col,
    VIO_Surfprop          *spr );

BICAPI  void  initialize_polygons_with_size(
    polygons_struct   *polygons,
    VIO_Colour            col,
    VIO_Surfprop          *spr,
    int               n_points,
    int               n_polygons,
    int               size );

BICAPI  void  free_polygon_neighbours(
    polygons_struct   *polygons );

BICAPI  void  delete_polygons( polygons_struct *polygons );

BICAPI  void  copy_polygons(
    polygons_struct   *src,
    polygons_struct   *dest );

BICAPI  void  set_polygon_per_item_colours(
    polygons_struct   *polygons );

BICAPI  void  create_polygons_visibilities(
    polygons_struct   *polygons );

BICAPI  void  set_polygons_visibilities(
    polygons_struct   *polygons,
    VIO_BOOL           state );

BICAPI  void  start_new_polygon(
    polygons_struct *polygons );

BICAPI  void  add_point_to_polygon(
    polygons_struct   *polygons,
    VIO_Point             *point,
    VIO_Vector            *normal );

BICAPI  int  get_polygon_points(
    polygons_struct   *polygons,
    int               poly,
    VIO_Point             points[] );

BICAPI  void  get_polygon_centroid(
    polygons_struct   *polygons,
    int               poly,
    VIO_Point             *centroid );

BICAPI  int  find_vertex_index(
    polygons_struct  *polygons,
    int              poly,
    int              point_index );

BICAPI  int  find_edge_index(
    polygons_struct  *polygons,
    int              poly,
    int              point_index1,
    int              point_index2 );

BICAPI  VIO_BOOL  find_polygon_with_edge(
    polygons_struct  *polygons,
    int              point_index1,
    int              point_index2,
    int              *poly_containing_edge,
    int              *edge_index );

BICAPI  VIO_BOOL  find_polygon_with_vertex(
    polygons_struct   *polygons,
    int               point_index,
    int               *poly_index,
    int               *vertex_index );

BICAPI  VIO_BOOL  lookup_polygon_vertex(
    polygons_struct   *polygons,
    VIO_Point             *point,
    int               *point_index );

BICAPI  VIO_BOOL  find_next_edge_around_point(
    polygons_struct   *polygons,
    int               poly,
    int               index_1,
    int               index_2,
    int               *next_poly,
    int               *next_index_1,
    int               *next_index_2 );

BICAPI  int  get_neighbours_of_point(
    polygons_struct   *polygons,
    int               poly,
    int               vertex_index,
    int               neighbours[],
    int               max_neighbours,
    VIO_BOOL           *interior_point );

BICAPI  int  get_polygons_around_vertex(
    polygons_struct   *polygons,
    int               poly,
    int               vertex_index,
    int               poly_indices[],
    int               n_polys_alloced,
    VIO_BOOL           *closed_flag );

BICAPI  void  compute_polygon_normal(
    polygons_struct  *polygons,
    int              poly,
    VIO_Vector           *normal );

BICAPI  void  compute_polygon_normals(
    polygons_struct  *polygons );

BICAPI  void  average_polygon_normals(
    polygons_struct  *polygons,
    int              n_iters,
    VIO_Real             neighbour_weight );

BICAPI  VIO_BOOL  get_plane_polygon_intersection(
    VIO_Vector           *normal,
    VIO_Real             d,
    polygons_struct  *polygons,
    int              poly,
    VIO_Point            intersection_points[] );

BICAPI  VIO_BOOL  get_plane_segment_intersection(
    VIO_Vector           *normal,
    VIO_Real             d,
    VIO_Point            *p1,
    VIO_Point            *p2,
    VIO_Point            *intersection_point );

BICAPI  void   reverse_polygons_vertices(
    polygons_struct  *polygons );

BICAPI  void   make_polygons_front_facing(
    polygons_struct  *polygons );

BICAPI  VIO_BOOL  polygon_is_back_facing(
    polygons_struct   *polygons,
    int               poly );

BICAPI  void  compute_points_centroid_and_normal(
    polygons_struct  *polygons,
    int              point_index,
    int              n_neighbours,
    int              neighbours[],
    VIO_Point            *centroid,
    VIO_Vector           *normal,
    VIO_Real             *base_length,
    VIO_Real             *curvature );

BICAPI  void  compute_polygon_point_centroid(
    polygons_struct  *polygons,
    int              poly,
    int              vertex_index,
    int              point_index,
    VIO_Point            *centroid,
    VIO_Vector           *normal,
    VIO_Real             *base_length,
    VIO_Real             *curvature );

BICAPI  VIO_Real  compute_polygon_vertex_curvature(
    polygons_struct  *polygons,
    int              point_index );

BICAPI  VIO_Real  get_polygon_edge_angle(
    polygons_struct  *polygons,
    int              poly,
    int              edge );

BICAPI  VIO_BOOL  polygons_are_same_topology(
    polygons_struct  *p1,
    polygons_struct  *p2 );

BICAPI  void  initialize_quadmesh(
    quadmesh_struct   *quadmesh,
    VIO_Colour            col,
    VIO_Surfprop          *spr,
    int               m,
    int               n );

BICAPI  void  delete_quadmesh(
    quadmesh_struct *quadmesh );

BICAPI  void  set_quadmesh_point(
    quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    VIO_Point            *point,
    VIO_Vector           *normal );

BICAPI  VIO_BOOL  get_quadmesh_point(
    const quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    VIO_Point            *point );

BICAPI  void  get_quadmesh_n_objects(
    const quadmesh_struct  *quadmesh,
    int              *m,
    int              *n );

BICAPI  void  compute_quadmesh_normals(
    quadmesh_struct  *quadmesh );

BICAPI  void  get_quadmesh_patch_indices(
    const quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    int              indices[] );

BICAPI  void  get_quadmesh_patch(
    const quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    VIO_Point            points[] );

BICAPI  void  reverse_quadmesh_vertices(
    quadmesh_struct  *quadmesh );

BICAPI  VIO_Colour     *get_8bit_rgb_pixel_lookup( void );

BICAPI  int  convert_rgb_pixel_to_8bit_lookup(
    VIO_Colour    colour );

BICAPI  VIO_Status   input_tag_objects_file(
    VIO_STR         filename,
    VIO_Colour         marker_colour,
    VIO_Real           default_size,
    Marker_types   default_type,
    int            *n_objects,
    object_struct  **object_list[] );

BICAPI  void   initialize_text(
    text_struct     *text,
    VIO_Point           *position,
    VIO_Colour          colour,
    Font_types      text_font,
    VIO_Real            size );

BICAPI  void   delete_text(
    text_struct     *text );

BICAPI  VIO_Status  output_texture_values(
    VIO_STR         filename,
    VIO_File_formats   format,
    int            n_values,
    VIO_Real           values[] );

BICAPI  VIO_Status  input_texture_values(
    VIO_STR         filename,
    int            *n_values,
    VIO_Real           *values[] );

BICAPI VIO_BOOL input_freesurfer_graphics_file(
    const char *filename,
    int *n_objects,
    object_struct **object_list[] );

BICAPI VIO_BOOL input_brainsuite_graphics_file(
    const char *filename,
    int *n_objects,
    object_struct **object_list[] );

BICAPI VIO_BOOL input_wavefront_graphics_file(
    const char *filename,
    int *n_objects,
    object_struct **object_list[] );

BICAPI VIO_BOOL input_ply_graphics_file(
    const char *filename,
    int *n_objects,
    object_struct **object_list[] );

#ifdef __cplusplus
}
#endif

#endif
