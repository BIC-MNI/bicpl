#ifndef  DEF_vol_prototypes
#define  DEF_vol_prototypes

#ifdef __cplusplus
extern "C" {
#endif

BICAPI  VIO_Volume  create_box_filtered_volume(
    VIO_Volume   volume,
    nc_type  nc_data_type,
    VIO_BOOL  sign_flag,
    VIO_Real     real_min_value,
    VIO_Real     real_max_value,
    VIO_Real     x_width,
    VIO_Real     y_width,
    VIO_Real     z_width );

BICAPI  void  modify_labels_in_range(
    VIO_Volume   volume,
    VIO_Volume   label_volume,
    int      src_min,
    int      src_max,
    int      dest_label,
    VIO_Real     min_threshold,
    VIO_Real     max_threshold,
    int      range_changed[2][VIO_N_DIMENSIONS] );

BICAPI  void  initialize_colour_coding(
    colour_coding_struct   *colour_coding,
    Colour_coding_types    type,
    VIO_Colour                 under_colour,
    VIO_Colour                 over_colour,
    VIO_Real                   min_value,
    VIO_Real                   max_value );

BICAPI  void  delete_colour_coding(
    colour_coding_struct   *colour_coding );

BICAPI  void  set_colour_coding_type(
    colour_coding_struct   *colour_coding,
    Colour_coding_types    type );

BICAPI  Colour_coding_types  get_colour_coding_type(
    colour_coding_struct   *colour_coding );

BICAPI  void  set_colour_coding_min_max(
    colour_coding_struct   *colour_coding,
    VIO_Real                   min_value,
    VIO_Real                   max_value );

BICAPI  void  get_colour_coding_min_max(
    colour_coding_struct   *colour_coding,
    VIO_Real                   *min_value,
    VIO_Real                   *max_value );

BICAPI  VIO_Colour  get_colour_coding_under_colour(
    colour_coding_struct   *colour_coding );

BICAPI  void  set_colour_coding_under_colour(
    colour_coding_struct   *colour_coding,
    VIO_Colour                 under_colour );

BICAPI  VIO_Colour  get_colour_coding_over_colour(
    colour_coding_struct   *colour_coding );

BICAPI  void  set_colour_coding_over_colour(
    colour_coding_struct   *colour_coding,
    VIO_Colour                 over_colour );

BICAPI  VIO_BOOL  define_colour_coding_user_defined(
    colour_coding_struct  *colour_code,
    int                   n_colours,
    VIO_Colour                colours[],
    VIO_Real                  positions[],
    Colour_spaces         interpolation_space );

BICAPI  VIO_Colour  get_colour_code(
    colour_coding_struct  *colour_coding,
    VIO_Real                  value );

BICAPI  void  colour_code_object(
    VIO_Volume                 volume,
    int                    continuity,
    colour_coding_struct   *colour_coding,
    object_struct          *object );

BICAPI  VIO_STR    get_default_user_def_colour_code_suffix( void );

BICAPI  VIO_Status  input_user_defined_colour_coding(
    colour_coding_struct   *colour_coding,
    VIO_STR                 filename );

BICAPI  void  create_volume_slice(
    VIO_Volume          volume1,
    VIO_Filter_types    filter_type1,
    VIO_Real            filter_width1,
    VIO_Real            slice_position1[],
    VIO_Real            x_axis1[],
    VIO_Real            y_axis1[],
    VIO_Real            x_translation1,
    VIO_Real            y_translation1,
    VIO_Real            x_scale1,
    VIO_Real            y_scale1,
    VIO_Volume          volume2,
    VIO_Filter_types    filter_type2,
    VIO_Real            filter_width2,
    VIO_Real            slice_position2[],
    VIO_Real            x_axis2[],
    VIO_Real            y_axis2[],
    VIO_Real            x_translation2,
    VIO_Real            y_translation2,
    VIO_Real            x_scale2,
    VIO_Real            y_scale2,
    int             x_viewport_size,
    int             y_viewport_size,
    int             x_pixel_start,
    int             x_pixel_end,
    int             y_pixel_start,
    int             y_pixel_end,
    Pixel_types     pixel_type,
    int             degrees_continuity,
    unsigned short  **cmode_colour_map,
    VIO_Colour          **rgb_colour_map,
    VIO_Colour          empty_colour,
    void            *render_storage,
    VIO_BOOL         clip_pixels_flag,
    int             *n_pixels_alloced,
    pixels_struct   *pixels );

BICAPI  void  set_volume_slice_pixel_range(
    VIO_Volume          volume1,
    VIO_Filter_types    filter_type1,
    VIO_Real            filter_width1,
    VIO_Real            slice_position1[],
    VIO_Real            x_axis1[],
    VIO_Real            y_axis1[],
    VIO_Real            x_translation1,
    VIO_Real            y_translation1,
    VIO_Real            x_scale1,
    VIO_Real            y_scale1,
    VIO_Volume          volume2,
    VIO_Filter_types    filter_type2,
    VIO_Real            filter_width2,
    VIO_Real            slice_position2[],
    VIO_Real            x_axis2[],
    VIO_Real            y_axis2[],
    VIO_Real            x_translation2,
    VIO_Real            y_translation2,
    VIO_Real            x_scale2,
    VIO_Real            y_scale2,
    int             x_viewport_size,
    int             y_viewport_size,
    Pixel_types     pixel_type,
    int             *n_pixels_alloced,
    pixels_struct   *pixels );

BICAPI  VIO_BOOL  find_volume_crop_bounds(
    VIO_Volume          volume,
    VIO_Real            min_crop_threshold,
    VIO_Real            max_crop_threshold,
    int             limits[2][VIO_MAX_DIMENSIONS] );

BICAPI  VIO_Volume  create_cropped_volume(
    VIO_Volume          volume,
    int             limits[2][VIO_MAX_DIMENSIONS] );

BICAPI  VIO_Volume  autocrop_volume(
    VIO_Volume    volume );

BICAPI  int  dilate_voxels_3d(
    VIO_Volume          volume,
    VIO_Volume          label_volume,
    VIO_Real            min_inside_label,
    VIO_Real            max_inside_label,
    VIO_Real            min_inside_value,
    VIO_Real            max_inside_value,
    VIO_Real            min_outside_label,
    VIO_Real            max_outside_label,
    VIO_Real            min_outside_value,
    VIO_Real            max_outside_value,
    VIO_Real            new_label,
    Neighbour_types connectivity,
    int             range_changed[2][VIO_N_DIMENSIONS] );

BICAPI  int  get_slice_weights_for_filter(
    VIO_Volume         volume,
    VIO_Real           voxel_position[],
    VIO_Real           voxel_direction[],   /* if filter_type != NEAREST */
    VIO_Filter_types   filter_type,
    VIO_Real           full_width_half_max,
    VIO_Real           ***positions,
    VIO_Real           *weights[] );

BICAPI  VIO_BOOL  fill_connected_voxels(
    VIO_Volume              volume,
    VIO_Volume              label_volume,
    Neighbour_types     connectivity,
    int                 voxel[],
    int                 min_label_threshold,
    int                 max_label_threshold,
    int                 desired_label,
    VIO_Real                min_threshold,
    VIO_Real                max_threshold,
    int                 range_changed[2][VIO_N_DIMENSIONS] );

BICAPI  int  get_neighbour_directions(
    Neighbour_types   connectivity,
    int               *dx[],
    int               *dy[] );

BICAPI  int  get_3D_neighbour_directions(
    Neighbour_types   connectivity,
    int               *dx[],
    int               *dy[],
    int               *dz[] );

BICAPI  VIO_BOOL  should_change_this_one(
    VIO_Volume          volume,
    VIO_Volume          label_volume,
    int             voxel[],
    VIO_Real            min_threshold,
    VIO_Real            max_threshold,
    int             label_min_threshold,
    int             label_max_threshold,
    int             desired_label );

BICAPI  void  interpolate_volume_to_slice(
    VIO_Volume          volume1,
    int             n_dims1,
    VIO_Real            origin1[],
    VIO_Real            x_axis1[],
    VIO_Real            y_axis1[],
    VIO_Volume          volume2,
    int             n_dims2,
    VIO_Real            origin2[],
    VIO_Real            x_axis2[],
    VIO_Real            y_axis2[],
    int             x_pixel_start,
    int             x_pixel_end,
    int             y_pixel_start,
    int             y_pixel_end,
    int             degrees_continuity,
    unsigned short  **cmode_colour_map,
    VIO_Colour          **rgb_colour_map,
    VIO_Colour          empty_colour,
    pixels_struct   *pixels );

BICAPI  VIO_Status  input_volume_header_only(
    VIO_STR               filename,
    int                  n_dimensions,
    VIO_STR               dim_names[],
    VIO_Volume               *volume,
    minc_input_options   *options );

BICAPI  void  set_label_volume_real_range(
    VIO_Volume  volume );

BICAPI  VIO_Volume  create_label_volume(
    VIO_Volume  volume,
    nc_type type );

BICAPI  VIO_BOOL  is_label_volume_initialized(
    VIO_Volume  volume );

BICAPI  void  set_all_volume_label_data(
    VIO_Volume    volume,
    int       value );

BICAPI  void  set_volume_label_data_5d(
    VIO_Volume          volume,
    int             v0,
    int             v1,
    int             v2,
    int             v3,
    int             v4,
    int             value );

BICAPI  void  set_volume_label_data(
    VIO_Volume          volume,
    int             voxel[],
    int             value );

BICAPI  int  get_volume_label_data(
    VIO_Volume          volume,
    int             voxel[] );

BICAPI  int  get_volume_label_data_5d(
    VIO_Volume          volume,
    int             v0,
    int             v1,
    int             v2,
    int             v3,
    int             v4 );

BICAPI  int  get_3D_volume_label_data(
    VIO_Volume          volume,
    int             x,
    int             y,
    int             z );

BICAPI  VIO_BOOL  get_voxel_label_bit(
    VIO_Volume          volume,
    int             voxel[],
    int             bit );

BICAPI  void  set_voxel_label_bit(
    VIO_Volume          volume,
    int             voxel[],
    int             bit,
    VIO_BOOL         value );

BICAPI  void  set_all_volume_label_data_bit(
    VIO_Volume         volume,
    int            bit,
    VIO_BOOL        value );

BICAPI  VIO_BOOL  get_volume_voxel_activity(
    VIO_Volume     volume,
    VIO_Real       voxel[],
    VIO_BOOL    activity_if_mixed );

BICAPI  VIO_Status  load_label_volume(
    VIO_STR   filename,
    VIO_Volume   label_volume );

BICAPI  VIO_Status  save_label_volume(
    VIO_STR   filename,
    VIO_STR   original_filename,
    VIO_Volume   label_volume,
    VIO_Real     crop_threshold );

BICAPI  VIO_Status  input_tags_as_labels(
    FILE    *file,
    VIO_Volume  volume,
    VIO_Volume  label_volume );

BICAPI  VIO_Status  create_label_volume_from_file(
    VIO_STR   filename,
    VIO_Volume   volume,
    VIO_Volume   *label_volume );

BICAPI  VIO_Status  output_labels_as_tags(
    FILE    *file,
    VIO_Volume  volume,
    VIO_Volume  label_volume,
    int     desired_label,
    VIO_Real    size,
    int     patient_id );

BICAPI  VIO_Status  input_landmarks_as_labels(
    FILE    *file,
    VIO_Volume  volume,
    VIO_Volume  label_volume );

BICAPI  int    get_volume_cross_section(
    VIO_Volume   volume,
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    VIO_Real     clipped_voxels[][VIO_MAX_DIMENSIONS] );

BICAPI  void    get_volume_mapping_range(
    VIO_Volume   volume,
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    VIO_Real     x_trans,
    VIO_Real     y_trans,
    VIO_Real     x_scale,
    VIO_Real     y_scale,
    VIO_Real     *x_pixel_start,
    VIO_Real     *x_pixel_end,
    VIO_Real     *y_pixel_start,
    VIO_Real     *y_pixel_end );

BICAPI  void    clip_viewport_to_volume(
    VIO_Volume   volume,
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    int      *x_pixel_start,
    int      *x_pixel_end,
    int      *y_pixel_start,
    int      *y_pixel_end );

BICAPI  void  get_mapping(
    VIO_Volume          volume,
    VIO_Real            origin[],
    VIO_Real            x_axis[],
    VIO_Real            y_axis[],
    VIO_Real            x_translation,
    VIO_Real            y_translation,
    VIO_Real            x_scale,
    VIO_Real            y_scale,
    VIO_Real            pix_origin[],
    VIO_Real            pix_x_axis[],
    VIO_Real            pix_y_axis[] );

BICAPI  void  map_voxel_to_pixel(
    int    n,
    VIO_Real   voxel[],
    VIO_Real   origin[],
    VIO_Real   x_axis[],
    VIO_Real   y_axis[],
    VIO_Real   *x_pixel,
    VIO_Real   *y_pixel );

BICAPI  VIO_BOOL  convert_slice_pixel_to_voxel(
    VIO_Volume          volume,
    VIO_Real            x_pixel,
    VIO_Real            y_pixel,
    VIO_Real            origin[],
    VIO_Real            x_axis[],
    VIO_Real            y_axis[],
    VIO_Real            x_translation,
    VIO_Real            y_translation,
    VIO_Real            x_scale,
    VIO_Real            y_scale,
    VIO_Real            voxel[] );

BICAPI  void  convert_voxel_to_slice_pixel(
    VIO_Volume          volume,
    VIO_Real            voxel[],
    VIO_Real            origin[],
    VIO_Real            x_axis[],
    VIO_Real            y_axis[],
    VIO_Real            x_translation,
    VIO_Real            y_translation,
    VIO_Real            x_scale,
    VIO_Real            y_scale,
    VIO_Real            *x_pixel,
    VIO_Real            *y_pixel );

BICAPI  void  resize_volume_slice(
    int          old_x_viewport_size,
    int          old_y_viewport_size,
    int          old_used_x_viewport_size,
    int          old_used_y_viewport_size,
    int          new_x_viewport_size,
    int          new_y_viewport_size,
    VIO_Real         *x_translation,
    VIO_Real         *y_translation,
    VIO_Real         *x_scale,
    VIO_Real         *y_scale,
    int          *used_x_viewport_size,
    int          *used_y_viewport_size );

BICAPI  void  fit_volume_slice_to_viewport(
    VIO_Volume       volume,
    VIO_Real         origin[],
    VIO_Real         x_axis[],
    VIO_Real         y_axis[],
    int          x_viewport_size,
    int          y_viewport_size,
    VIO_Real         fraction_oversize,
    VIO_Real         *x_translation,
    VIO_Real         *y_translation,
    VIO_Real         *x_scale,
    VIO_Real         *y_scale,
    int          *used_x_viewport_size,
    int          *used_y_viewport_size );

BICAPI  void   scale_slice_about_viewport_centre(
    VIO_Real        scale_factor,
    int         x_viewport_size,
    int         y_viewport_size,
    VIO_Real        *x_translation,
    VIO_Real        *y_translation,
    VIO_Real        *x_scale,
    VIO_Real        *y_scale );

BICAPI  VIO_BOOL  voxel_is_within_volume(
    VIO_Volume   volume,
    VIO_Real     voxel_position[] );

BICAPI  VIO_BOOL  int_voxel_is_within_volume(
    VIO_Volume   volume,
    int      indices[] );

BICAPI  void  convert_real_to_int_voxel(
    int      n_dimensions,
    VIO_Real     voxel[],
    int      int_voxel[] );

BICAPI  void  convert_int_to_real_voxel(
    int      n_dimensions,
    int      int_voxel[],
    VIO_Real     voxel[] );

BICAPI  VIO_BOOL  voxel_contains_range(
    VIO_Volume   volume,
    int      int_voxel[],
    VIO_Real     min_value,
    VIO_Real     max_value );

BICAPI  VIO_BOOL  volumes_are_same_grid(
    VIO_Volume    volume1,
    VIO_Volume    volume2 );

BICAPI  VIO_Status  output_volume_free_format(
    VIO_STR         prefix,
    VIO_Volume         volume,
    int            axis_ordering[] );

BICAPI  void   *initialize_render_storage( void );

BICAPI  void   delete_render_storage(
    void  *ptr );

BICAPI  void  render_volume_to_slice(
    int             n_dims1,
    int             sizes1[],
    void            *volume_data1,
    VIO_Data_types      volume1_type,
    int             n_slices1,
    VIO_Real            weights1[],
    int             strides1[],
    VIO_Real            **origins1,
    VIO_Real            x_axis1[],
    VIO_Real            y_axis1[],
    int             n_dims2,
    int             sizes2[],
    void            *volume_data2,
    VIO_Data_types      volume2_type,
    int             n_slices2,
    VIO_Real            weights2[],
    int             strides2[],
    VIO_Real            **origins2,
    VIO_Real            x_axis2[],
    VIO_Real            y_axis2[],
    int             x_pixel_start,
    int             x_pixel_end,
    int             y_pixel_start,
    int             y_pixel_end,
    unsigned short  **cmode_colour_map,
    VIO_Colour          **rgb_colour_map,
    VIO_Colour          empty_colour,
    void            *render_storage,
    pixels_struct   *pixels );

BICAPI  void  render_one_row (
    void            *volume_data1,
    VIO_Data_types      volume1_type,
    int             y,
    int             start_x,
    int             end_x,
    size_t          **y_offsets1,
    size_t          **row_offsets1,
    void            **start_slices1,
    int             n_slices1,
    VIO_Real            weights1[],
    void            *volume_data2,
    VIO_Data_types      volume2_type,
    size_t          **y_offsets2,
    size_t          **row_offsets2,
    void            **start_slices2,
    int             n_slices2,
    VIO_Real            weights2[],
    unsigned short  **cmode_colour_map,
    VIO_Colour          **rgb_colour_map,
    pixels_struct   *pixels );

BICAPI  void  initialize_resample_volume(
    resample_struct      *resample,
    VIO_Volume               src_volume,
    VIO_General_transform    *dest_to_src_transform,
    VIO_Volume               dest_volume );

BICAPI  VIO_BOOL  do_more_resampling(
    resample_struct  *resample,
    VIO_Real             max_seconds,
    VIO_Real             *fraction_done );

BICAPI  void  resample_volume(
    VIO_Volume                   src_volume,
    VIO_General_transform        *dest_to_src_transform,
    VIO_Volume                   dest_volume );

BICAPI  void  scan_lines_to_voxels(
    lines_struct     *lines,
    VIO_Volume           volume,
    VIO_Volume           label_volume,
    int              label );

BICAPI  void  scan_marker_to_voxels(
    marker_struct    *marker,
    VIO_Volume           volume,
    VIO_Volume           label_volume,
    int              label );

BICAPI  void  scan_object_to_volume(
    object_struct    *object,
    VIO_Volume           volume,
    VIO_Volume           label_volume,
    int              label,
    VIO_Real             max_distance );

BICAPI  void  scan_polygons_to_voxels(
    polygons_struct     *polygons,
    VIO_Volume              volume,
    VIO_Volume              label_volume,
    int                 label,
    VIO_Real                max_distance );

BICAPI  void  scan_a_polygon(
    int                 size,
    VIO_Point               vertices[],
    VIO_Point               voxels[],
    int                 n_output_vertices,
    VIO_Point               output_vertices[],
    VIO_Volume              volume,
    VIO_Volume              label_volume,
    int                 sizes[],
    int                 label );

BICAPI  void  scan_quadmesh_to_voxels(
    quadmesh_struct     *quadmesh,
    VIO_Volume              volume,
    VIO_Volume              label_volume,
    int                 label,
    VIO_Real                max_distance );

BICAPI  VIO_Volume  smooth_resample_volume(
    VIO_Volume              volume,
    int                 new_nx,
    int                 new_ny,
    int                 new_nz );

BICAPI  void  convert_voxel_to_talairach(
    VIO_Real   x_voxel,
    VIO_Real   y_voxel,
    VIO_Real   z_voxel,
    int    nx_voxels,
    int    ny_voxels,
    int    nz_voxels,
    VIO_Real   *x_tal,
    VIO_Real   *y_tal,
    VIO_Real   *z_tal );

BICAPI  void  convert_talairach_to_voxel(
    VIO_Real   x_tal,
    VIO_Real   y_tal,
    VIO_Real   z_tal,
    int    nx_voxels,
    int    ny_voxels,
    int    nz_voxels,
    VIO_Real   *x_voxel,
    VIO_Real   *y_voxel,
    VIO_Real   *z_voxel );

BICAPI  void  convert_talairach_to_mm(
    VIO_Real   x_tal,
    VIO_Real   y_tal,
    VIO_Real   z_tal,
    VIO_Real   *x_mm,
    VIO_Real   *y_mm,
    VIO_Real   *z_mm );

BICAPI  void  convert_mm_to_talairach(
    VIO_Real   x_mm,
    VIO_Real   y_mm,
    VIO_Real   z_mm,
    VIO_Real   *x_tal,
    VIO_Real   *y_tal,
    VIO_Real   *z_tal );

#ifdef __cplusplus
}
#endif

#endif
