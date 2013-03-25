#ifndef  DEF_vol_prototypes
#define  DEF_vol_prototypes

public  Volume  create_box_filtered_volume(
    Volume   volume,
    nc_type  nc_data_type,
    VIO_BOOL  sign_flag,
    VIO_Real     real_min_value,
    VIO_Real     real_max_value,
    VIO_Real     x_width,
    VIO_Real     y_width,
    VIO_Real     z_width );

public  void  modify_labels_in_range(
    Volume   volume,
    Volume   label_volume,
    int      src_label,
    int      dest_label,
    VIO_Real     min_threshold,
    VIO_Real     max_threshold );

public  void  initialize_colour_coding(
    colour_coding_struct   *colour_coding,
    Colour_coding_types    type,
    Colour                 under_colour,
    Colour                 over_colour,
    VIO_Real                   min_value,
    VIO_Real                   max_value );

public  void  set_colour_coding_type(
    colour_coding_struct   *colour_coding,
    Colour_coding_types    type );

public  void  set_colour_coding_min_max(
    colour_coding_struct   *colour_coding,
    VIO_Real                   min_value,
    VIO_Real                   max_value );

public  void  get_colour_coding_min_max(
    colour_coding_struct   *colour_coding,
    VIO_Real                   *min_value,
    VIO_Real                   *max_value );

public  Colour  get_colour_coding_under_colour(
    colour_coding_struct   *colour_coding );

public  void  set_colour_coding_under_colour(
    colour_coding_struct   *colour_coding,
    Colour                 under_colour );

public  Colour  get_colour_coding_over_colour(
    colour_coding_struct   *colour_coding );

public  void  set_colour_coding_over_colour(
    colour_coding_struct   *colour_coding,
    Colour                 over_colour );

public  Colour  get_colour_code(
    colour_coding_struct  *colour_coding,
    VIO_Real                  value );

public  void  colour_code_object(
    Volume                 volume,
    int                    continuity,
    colour_coding_struct   *colour_coding,
    object_struct          *object );

public  int    get_volume_cross_section(
    Volume   volume,
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    VIO_Real     clipped_voxels[][MAX_DIMENSIONS] );

public  void    get_volume_mapping_range(
    Volume   volume,
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

public  void  create_volume_slice(
    Volume          volume1,
    Filter_types    filter_type1,
    VIO_Real            filter_width1,
    VIO_Real            slice_position1[],
    VIO_Real            x_axis1[],
    VIO_Real            y_axis1[],
    VIO_Real            x_translation1,
    VIO_Real            y_translation1,
    VIO_Real            x_scale1,
    VIO_Real            y_scale1,
    Volume          volume2,
    Filter_types    filter_type2,
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
    int             degrees_continuity,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    void            *render_storage,
    int             *n_pixels_alloced,
    pixels_struct   *pixels );

public  VIO_BOOL  find_volume_crop_bounds(
    Volume          volume,
    VIO_Real            min_crop_threshold,
    VIO_Real            max_crop_threshold,
    int             limits[2][MAX_DIMENSIONS] );

public  Volume  create_cropped_volume(
    Volume          volume,
    int             limits[2][MAX_DIMENSIONS] );

public  void  dilate_labeled_voxels_3d(
    Volume          volume,
    Volume          label_volume,
    int             min_inside_label,
    int             max_inside_label,
    VIO_Real            min_inside_value,
    VIO_Real            max_inside_value,
    int             min_outside_label,
    int             max_outside_label,
    VIO_Real            min_outside_value,
    VIO_Real            max_outside_value,
    int             new_label,
    Neighbour_types connectivity );

public  int  get_slice_weights_for_filter(
    Volume         volume,
    VIO_Real           voxel_position[],
    VIO_Real           voxel_direction[],   /* if filter_type != NEAREST */
    Filter_types   filter_type,
    VIO_Real           full_width_half_max,
    VIO_Real           ***positions,
    VIO_Real           *weights[] );

public  VIO_BOOL  fill_connected_voxels(
    Volume              volume,
    Volume              label_volume,
    Neighbour_types     connectivity,
    int                 voxel[],
    int                 min_label_threshold,
    int                 max_label_threshold,
    int                 desired_label,
    VIO_Real                min_threshold,
    VIO_Real                max_threshold );

public  int  get_neighbour_directions(
    Neighbour_types   connectivity,
    int               *dx[],
    int               *dy[] );

public  int  get_3D_neighbour_directions(
    Neighbour_types   connectivity,
    int               *dx[],
    int               *dy[],
    int               *dz[] );

public  VIO_BOOL  should_change_this_one(
    Volume          volume,
    Volume          label_volume,
    int             voxel[],
    VIO_Real            min_threshold,
    VIO_Real            max_threshold,
    int             label_min_threshold,
    int             label_max_threshold,
    int             desired_label );

public  void  interpolate_volume_to_slice(
    Volume          volume1,
    int             n_dims1,
    int             sizes1[],
    VIO_Real            origin1[],
    VIO_Real            x_axis1[],
    VIO_Real            y_axis1[],
    Volume          volume2,
    int             n_dims2,
    int             sizes2[],
    VIO_Real            origin2[],
    VIO_Real            x_axis2[],
    VIO_Real            y_axis2[],
    int             degrees_continuity,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    pixels_struct   *pixels );

public  void  set_label_volume_real_range(
    Volume  volume );

public  Volume  create_label_volume(
    Volume  volume,
    nc_type type );

public  void  set_all_volume_label_data(
    Volume    volume,
    int       value );

public  void  set_volume_label_data(
    Volume          volume,
    int             voxel[],
    int             value );

public  int  get_volume_label_data(
    Volume          volume,
    int             voxel[] );

public  int  get_3D_volume_label_data(
    Volume          volume,
    int             x,
    int             y,
    int             z );

public  VIO_BOOL  get_voxel_label_bit(
    Volume          volume,
    int             voxel[],
    int             bit );

public  void  set_voxel_label_bit(
    Volume          volume,
    int             voxel[],
    int             bit,
    VIO_BOOL         value );

public  void  set_all_volume_label_data_bit(
    Volume         volume,
    int            bit,
    VIO_BOOL        value );

public  VIO_BOOL  get_volume_voxel_activity(
    Volume     volume,
    VIO_Real       voxel[],
    VIO_BOOL    activity_if_mixed );

public  Status  load_label_volume(
    char     filename[],
    Volume   label_volume );

public  Status  save_label_volume(
    char     filename[],
    char     original_filename[],
    Volume   label_volume );

public  Status  input_tags_as_labels(
    FILE    *file,
    Volume  volume,
    Volume  label_volume );

public  Status  output_labels_as_tags(
    FILE    *file,
    Volume  volume,
    Volume  label_volume,
    int     desired_label,
    VIO_Real    size,
    int     patient_id );

public  Status  input_landmarks_as_labels(
    FILE    *file,
    Volume  volume,
    Volume  label_volume );

public  void  get_mapping(
    Volume          volume,
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

public  void  map_voxel_to_pixel(
    int    n,
    VIO_Real   voxel[],
    VIO_Real   origin[],
    VIO_Real   x_axis[],
    VIO_Real   y_axis[],
    VIO_Real   *x_pixel,
    VIO_Real   *y_pixel );

public  VIO_BOOL  convert_slice_pixel_to_voxel(
    Volume          volume,
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

public  void  convert_voxel_to_slice_pixel(
    Volume          volume,
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

public  void  resize_volume_slice(
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

public  void  fit_volume_slice_to_viewport(
    Volume       volume,
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

public  void   scale_slice_about_viewport_centre(
    VIO_Real        scale_factor,
    int         x_viewport_size,
    int         y_viewport_size,
    VIO_Real        *x_translation,
    VIO_Real        *y_translation,
    VIO_Real        *x_scale,
    VIO_Real        *y_scale );

public  VIO_BOOL  voxel_is_within_volume(
    Volume   volume,
    VIO_Real     voxel_position[] );

public  VIO_BOOL  int_voxel_is_within_volume(
    Volume   volume,
    int      indices[] );

public  void  convert_real_to_int_voxel(
    int      n_dimensions,
    VIO_Real     voxel[],
    int      int_voxel[] );

public  VIO_BOOL  voxel_contains_range(
    Volume   volume,
    int      int_voxel[],
    VIO_Real     min_value,
    VIO_Real     max_value );

public  Status  output_volume_free_format(
    char           prefix[],
    Volume         volume,
    int            axis_ordering[] );

public  void   *initialize_render_storage();

public  void   delete_render_storage(
    void  *ptr );

public  void  render_volume_to_slice(
    int             n_dims1,
    int             sizes1[],
    void            *volume_data1,
    Data_types      volume1_type,
    int             n_slices1,
    VIO_Real            weights1[],
    int             strides1[],
    VIO_Real            **origins1,
    VIO_Real            x_axis1[],
    VIO_Real            y_axis1[],
    int             n_dims2,
    int             sizes2[],
    void            *volume_data2,
    Data_types      volume2_type,
    int             n_slices2,
    VIO_Real            weights2[],
    int             strides2[],
    VIO_Real            **origins2,
    VIO_Real            x_axis2[],
    VIO_Real            y_axis2[],
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    void            *render_storage,
    pixels_struct   *pixels );

public  void  render_the_slice (
    void            *volume_data1,
    Data_types      volume1_type,
    int             y,
    int             start_x,
    int             end_x,
    int             **y_offsets1,
    int             **row_offsets1,
    void            **start_slices1,
    int             n_slices1,
    VIO_Real            weights1[],
    void            *volume_data2,
    Data_types      volume2_type,
    int             **y_offsets2,
    int             **row_offsets2,
    void            **start_slices2,
    int             n_slices2,
    VIO_Real            weights2[],
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    pixels_struct   *pixels );

public  void  initialize_resample_volume(
    resample_struct      *resample,
    Volume               src_volume,
    VIO_General_transform    *dest_to_src_transform,
    Volume               dest_volume );

public  VIO_BOOL  do_more_resampling(
    resample_struct  *resample,
    VIO_Real             max_seconds,
    VIO_Real             *fraction_done );

public  void  resample_volume(
    Volume                   src_volume,
    VIO_General_transform        *dest_to_src_transform,
    Volume                   dest_volume );

public  void  scan_lines_to_voxels(
    lines_struct     *lines,
    Volume           volume,
    Volume           label_volume,
    int              label );

public  void  scan_marker_to_voxels(
    marker_struct    *marker,
    Volume           volume,
    Volume           label_volume,
    int              label );

public  void  scan_object_to_volume(
    object_struct    *object,
    Volume           volume,
    Volume           label_volume,
    int              label,
    VIO_Real             max_distance );

public  void  scan_polygons_to_voxels(
    polygons_struct     *polygons,
    Volume              volume,
    Volume              label_volume,
    int                 label,
    VIO_Real                max_distance );

public  Volume  smooth_resample_volume(
    Volume              volume,
    int                 new_nx,
    int                 new_ny,
    int                 new_nz );

public  void  convert_voxel_to_talairach(
    VIO_Real   x_voxel,
    VIO_Real   y_voxel,
    VIO_Real   z_voxel,
    int    nx_voxels,
    int    ny_voxels,
    int    nz_voxels,
    VIO_Real   *x_tal,
    VIO_Real   *y_tal,
    VIO_Real   *z_tal );

public  void  convert_talairach_to_voxel(
    VIO_Real   x_tal,
    VIO_Real   y_tal,
    VIO_Real   z_tal,
    int    nx_voxels,
    int    ny_voxels,
    int    nz_voxels,
    VIO_Real   *x_voxel,
    VIO_Real   *y_voxel,
    VIO_Real   *z_voxel );

public  void  convert_talairach_to_mm(
    VIO_Real   x_tal,
    VIO_Real   y_tal,
    VIO_Real   z_tal,
    VIO_Real   *x_mm,
    VIO_Real   *y_mm,
    VIO_Real   *z_mm );

public  void  convert_mm_to_talairach(
    VIO_Real   x_mm,
    VIO_Real   y_mm,
    VIO_Real   z_mm,
    VIO_Real   *x_tal,
    VIO_Real   *y_tal,
    VIO_Real   *z_tal );
#endif
