#ifndef  DEF_object_prototypes
#define  DEF_object_prototypes

public  void  get_default_surfprop(
    Surfprop  *surfprop );

public  BOOLEAN  lookup_colour(
    char    colour_name[],
    Colour  *col );

public  BOOLEAN  lookup_colour_name(
    Colour  col,
    char    colour_name[] );

public  void  rgb_to_hsl(
    Real    r,
    Real    g,
    Real    b,
    Real    *h,
    Real    *s,
    Real    *l );

public  void  hsl_to_rgb(
    Real   h,
    Real   sl,
    Real   l,
    Real   *r,
    Real   *g,
    Real   *b );

public  void  convert_colour_to_hsl(
    Colour   rgb,
    Colour   *hsl );

public  void  convert_colour_to_rgb(
    Colour   hsl,
    Colour   *rgb );

public  BOOLEAN  equal_colours(
    Colour  col1,
    Colour  col2 );

public  void  convert_colour_to_string(
    Colour   col,
    char     string[] );

public  Colour  convert_string_to_colour(
    char     string[] );

public  int  get_colour_distance(
    int      r,
    int      g,
    int      b,
    Colour   c2 );

public  int   find_closest_colour(
    int     r,
    int     g,
    int     b,
    int     n_colours,
    Colour  colours[] );

public  int  get_Colour_luminance(
    Colour   colour );

public  Colour  SCALE_COLOUR(
    Colour   colour,
    Real     factor );

public  Status   input_graphics_file(
    char           filename[],
    File_formats   *format,
    int            *n_objects,
    object_struct  ***object_list );

public  Status   output_graphics_file(
    char           filename[],
    File_formats   format,
    int            n_objects,
    object_struct  *object_list[] );

public  Status   input_objects_any_format(
    Volume         volume,
    char           filename[],
    Colour         marker_colour,
    Real           marker_size,
    Marker_types   marker_type,
    int            *n_objects,
    object_struct  **object_list[] );

public  char  *get_default_landmark_file_suffix();

public  Status   input_landmark_file(
    Volume         volume,
    char           filename[],
    Colour         colour,
    Real           size,
    Marker_types   type,
    int            *n_objects,
    object_struct  **object_list[] );

public  Status  io_tag_point(
    FILE            *file,
    IO_types        io_direction,
    Volume          volume,
    Real            size,
    marker_struct   *marker );

public  void  initialize_lines(
    lines_struct    *lines,
    Colour          col );

public  void  delete_lines( lines_struct *lines );

public  void  start_new_line( lines_struct *lines );

public  void  add_point_to_line(
    lines_struct   *lines,
    Point          *point );

public  void  get_line_segment_index(
    lines_struct  *lines,
    int           obj_index,
    int           *line,
    int           *seg );

public  void  set_bintree_delete_function(
    void  (*func)( bintree_struct_ptr * ) );

public  void  delete_bintree_if_any(
    bintree_struct_ptr   *bintree );

public  Real  get_lines_length(
    lines_struct  *lines );

public  void  initialize_model(
    model_struct  *model );

public  void  delete_model(
    model_struct   *model );

public  void  assign_model_extra_ptr(
    model_struct   *model,
    void           *ptr );

public  void  *get_model_extra_ptr(
    model_struct   *model );

public  void  add_object_to_list(
    int            *n_objects,
    object_struct  ***object_list,
    object_struct  *new_object );

public  void  remove_object_from_list(
    int            *n_objects,
    object_struct  **object_list[],
    int            obj_index );

public  void  add_object_to_model(
    model_struct   *model,
    object_struct  *new_object );

public  void  insert_object_in_model(
    model_struct   *model,
    object_struct  *new_object,
    int            obj_index );

public  int  find_object_index_in_model(
    model_struct   *model,
    object_struct  *object );

public  void  remove_ith_object_from_model(
    model_struct   *model,
    int            obj_index );

public  void  remove_object_from_model(
    model_struct   *model,
    object_struct  *object );

public  void  delete_object_list(
    int              n_objects,
    object_struct    *object_list[] );

public  Status  io_lines(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    lines_struct        *lines );

public  Status  io_marker(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    marker_struct       *marker );

public  Status  io_model(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    model_struct    *model );

public  Status  io_pixels(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    pixels_struct       *pixels );

public  Status  io_polygons(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    polygons_struct     *polygons );

public  Status  io_quadmesh(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    quadmesh_struct     *quadmesh );

public  Status  io_text(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    text_struct     *text );

public  Status  io_point(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Point           *point );

public  Status  io_vector(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Vector          *v );

public  Status  io_colour(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Colour          *colour );

public  Status  io_colours(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Colour_flags    *colour_flag,
    int             n_items,
    int             n_points,
    Colour          **colours );

public  Status  io_surfprop(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Surfprop        *surfprop );

public  Status  io_object_type(
    FILE           *file,
    IO_types       io_flag,
    File_formats   format,
    Object_types   type );

public  Status  input_object_type(
    FILE           *file,
    Object_types   *type,
    File_formats   *format,
    BOOLEAN        *eof );

public  Status  io_pixel_colours(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    int             n,
    Colour          *pixel_colours[] );

public  Status  io_pixel_colour(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Colour          *pixel_colour );

public  Status  input_object(
    char           directory[],
    FILE           *file,
    File_formats   *format,
    object_struct  **object,
    BOOLEAN        *eof );

public  Status  output_object(
    FILE           *file,
    File_formats   format,
    object_struct  *object );

public  object_struct  *create_object(
    Object_types   object_type );

public  Object_types  get_object_type(
    object_struct   *object );

public  BOOLEAN  get_object_visibility(
    object_struct  *object );

public  void  set_object_visibility(
    object_struct  *object,
    BOOLEAN        visibility );

public  lines_struct  *get_lines_ptr(
    object_struct  *object );

public  marker_struct  *get_marker_ptr(
    object_struct  *object );

public  model_struct  *get_model_ptr(
    object_struct  *object );

public  pixels_struct  *get_pixels_ptr(
    object_struct  *object );

public  polygons_struct  *get_polygons_ptr(
    object_struct  *object );

public  quadmesh_struct  *get_quadmesh_ptr(
    object_struct  *object );

public  text_struct  *get_text_ptr(
    object_struct  *object );

public  void  delete_object(
    object_struct  *object );

public  int  get_object_points(
    object_struct   *object,
    Point           *points[] );

public  int  get_object_normals(
    object_struct   *object,
    Vector          *normals[] );

public  Colour_flags  *get_object_colours(
    object_struct   *object,
    Colour          *colours[] );

public  void  set_object_colours(
    object_struct   *object,
    Colour          colours[] );

public  Surfprop  *get_object_surfprop(
    object_struct   *object );

public  void  get_object_name(
    object_struct  *object,
    char           name[] );

public  void  initialize_object_traverse(
    object_traverse_struct  *object_traverse,
    int                     n_objects,
    object_struct           *object_list[] );

public  BOOLEAN  get_next_object_traverse(
    object_traverse_struct  *object_traverse,
    object_struct           **object );

public  void  terminate_object_traverse(
    object_traverse_struct   *object_traverse );

public  BOOLEAN  get_range_of_object(
    object_struct   *object,
    BOOLEAN         visible_ones_only,
    Point           *min_corner,
    Point           *max_corner );

public  void  reverse_object_normals(
    object_struct   *object );

public  BOOLEAN  get_object_colour(
    object_struct  *object,
    Colour         *colour );

public  void  set_object_colour(
    object_struct  *object,
    Colour         col );

public  void  set_object_surfprop(
    object_struct  *object,
    Surfprop       *spr );

public  void  free_colours(
    Colour_flags   colour_flag,
    Colour         colours[],
    int            n_points,
    int            n_items );

public  BOOLEAN  objects_are_same_topology(
    int    n_points1,
    int    n_items1,
    int    *end_indices1,
    int    *indices1,
    int    n_points2,
    int    n_items2,
    int    *end_indices2,
    int    *indices2 );

public  int  get_n_colours(
    Colour_flags  colour_flag,
    int           n_points,
    int           n_items );

public  void  initialize_pixels(
    pixels_struct  *pixels,
    int            x_position,
    int            y_position,
    int            x_size,
    int            y_size,
    Real           x_zoom,
    Real           y_zoom,
    Pixel_types    pixel_type );

public  void  delete_pixels( pixels_struct *pixels );

public  void  modify_pixels_size(
    int            *n_pixels_alloced,
    pixels_struct  *pixels,
    int            x_size,
    int            y_size,
    Pixel_types    pixel_type );

public  void  convert_pixels24_to_pixels8(
    pixels_struct   *pixels_rgb,
    pixels_struct   *pixels_8 );

public  void  convert_pixels24_to_index8(
    pixels_struct   *pixels_rgb,
    pixels_struct   *pixels_8,
    int             n_colours,
    Colour          colour_table[] );

public  void  convert_index8_to_pixels24(
    pixels_struct   *pixels_8,
    Colour          colour_table[],
    pixels_struct   *pixels_rgb );

public  void  convert_pixels24_to_gray_scale(
    pixels_struct   *pixels_rgb,
    pixels_struct   *pixels_8 );

public  void  convert_pixels24_to_dithered(
    pixels_struct   *pixels_rgb,
    pixels_struct   *pixels_8,
    int             n_colours,
    Colour          colour_table[] );

public  void  resample_pixels(
    pixels_struct   *pixels,
    Transform_2d    *transform,
    int             n_samples,
    pixels_struct   *new_pixels,
    Colour          background_colour );

public  void  copy_pixel_region(
    pixels_struct   *pixels,
    int             x_min,
    int             x_max,
    int             y_min,
    int             y_max,
    pixels_struct   *new_pixels );

public  void  check_polygons_neighbours_computed(
    polygons_struct   *polygons );

public  void  initialize_polygons(
    polygons_struct   *polygons,
    Colour            col,
    Surfprop          *spr );

public  void  free_polygon_neighbours(
    polygons_struct   *polygons );

public  void  delete_polygons( polygons_struct *polygons );

public  void  copy_polygons(
    polygons_struct   *src,
    polygons_struct   *dest );

public  void  set_polygon_per_item_colours(
    polygons_struct   *polygons );

public  void  create_polygons_visibilities(
    polygons_struct   *polygons );

public  void  set_polygons_visibilities(
    polygons_struct   *polygons,
    BOOLEAN           state );

public  void  start_new_polygon( polygons_struct *polygons );

public  void  add_point_to_polygon(
    polygons_struct   *polygons,
    Point             *point,
    Vector            *normal );

public  int  get_polygon_points(
    polygons_struct   *polygons,
    int               poly,
    Point             points[] );

public  void  get_polygon_centroid(
    polygons_struct   *polygons,
    int               poly,
    Point             *centroid );

public  int  find_vertex_index(
    polygons_struct  *polygons,
    int              poly,
    int              point_index );

public  int  find_edge_index(
    polygons_struct  *polygons,
    int              poly,
    int              point_index1,
    int              point_index2 );

public  BOOLEAN  find_polygon_with_edge(
    polygons_struct  *polygons,
    int              point_index1,
    int              point_index2,
    int              *poly_containing_edge,
    int              *edge_index );

public  BOOLEAN  lookup_polygon_vertex(
    polygons_struct   *polygons,
    Point             *point,
    int               *point_index );

public  BOOLEAN  find_polygon_with_vertex(
    polygons_struct   *polygons,
    int               point_index,
    int               *poly_index,
    int               *vertex_index );

public  BOOLEAN  find_next_edge_around_point(
    polygons_struct   *polygons,
    int               poly,
    int               index_1,
    int               index_2,
    int               *next_poly,
    int               *next_index_1,
    int               *next_index_2 );

public  int  get_neighbours_of_point(
    polygons_struct   *polygons,
    int               poly,
    int               vertex_index,
    int               neighbours[],
    int               max_neighbours,
    BOOLEAN           *interior_point );

public  int  get_polygons_around_vertex(
    polygons_struct   *polygons,
    int               poly,
    int               vertex_index,
    int               poly_indices[],
    int               n_polys_alloced,
    BOOLEAN           *closed_flag );

public  void  compute_polygon_normal(
    polygons_struct  *polygons,
    int              poly,
    Vector           *normal );

public  void  compute_polygon_normals(
    polygons_struct  *polygons );

public  void  average_polygon_normals(
    polygons_struct  *polygons,
    int              n_iters,
    Real             neighbour_weight );

public  BOOLEAN  get_plane_polygon_intersection(
    Vector           *normal,
    Real             d,
    polygons_struct  *polygons,
    int              poly,
    Point            intersection_points[] );

public  BOOLEAN  get_plane_segment_intersection(
    Vector           *normal,
    Real             d,
    Point            *p1,
    Point            *p2,
    Point            *intersection_point );

public  void   reverse_polygons_vertices(
    polygons_struct  *polygons );

public  void   make_polygons_front_facing(
    polygons_struct  *polygons );

public  BOOLEAN  polygon_is_back_facing(
    polygons_struct   *polygons,
    int               poly );

public  void  compute_points_centroid_and_normal(
    polygons_struct  *polygons,
    int              point_index,
    int              n_neighbours,
    int              neighbours[],
    Point            *centroid,
    Vector           *normal,
    Real             *base_length,
    Real             *curvature );

public  void  compute_polygon_point_centroid(
    polygons_struct  *polygons,
    int              poly,
    int              vertex_index,
    int              point_index,
    Point            *centroid,
    Vector           *normal,
    Real             *base_length,
    Real             *curvature );

public  Real  compute_polygon_vertex_curvature(
    polygons_struct  *polygons,
    int              point_index );

public  Real  get_polygon_edge_angle(
    polygons_struct  *polygons,
    int              poly,
    int              edge );

public  BOOLEAN  polygons_are_same_topology(
    polygons_struct  *p1,
    polygons_struct  *p2 );

public  void  initialize_quadmesh(
    quadmesh_struct   *quadmesh,
    Colour            col,
    Surfprop          *spr,
    int               m,
    int               n );

public  void  delete_quadmesh(
    quadmesh_struct *quadmesh );

public  void  set_quadmesh_point(
    quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    Point            *point,
    Vector           *normal );

public  void  get_quadmesh_n_objects(
    quadmesh_struct  *quadmesh,
    int              *m,
    int              *n );

public  void  compute_quadmesh_normals(
    quadmesh_struct  *quadmesh );

public  void  get_quadmesh_patch_indices(
    quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    int              indices[] );

public  void  get_quadmesh_patch(
    quadmesh_struct  *quadmesh,
    int              i,
    int              j,
    Point            points[] );

public  Colour     *get_8bit_rgb_pixel_lookup( void );

public  int  convert_rgb_pixel_to_8bit_lookup(
    Colour    colour );

public  Status   input_tag_objects_file(
    char           filename[],
    Colour         marker_colour,
    Real           default_size,
    Marker_types   default_type,
    int            *n_objects,
    object_struct  **object_list[] );

public  void   initialize_text(
    text_struct     *text,
    Point           *position,
    Colour          colour,
    Font_types      text_font,
    Real            size );
#endif
