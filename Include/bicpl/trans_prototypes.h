#ifndef  DEF_trans_prototypes
#define  DEF_trans_prototypes

#ifdef __cplusplus
extern "C" {
#endif

BICAPI  void  get_nonlinear_warp(
   VIO_Real     **positions,   /* n_points x n_dims */
   VIO_Real     **values,   /* n_points x n_values */
   VIO_Real     **INVMLY,   /* n_points+1+n_dims x n_values */
   int      n_points,
   int      n_dims,
   int      n_values );

BICAPI  void  compute_transform_from_tags(
    int                 npoints, 
    VIO_Real                **tag_list1, 
    VIO_Real                **tag_list2, 
    Trans_type          trans_type,
    VIO_General_transform   *transform);

BICAPI  void  build_transformation_matrix(
    Transform   *lt, 
    VIO_Real        center[],
    VIO_Real        translations[],
    VIO_Real        scales[],
    VIO_Real        shears[],
    VIO_Real        rotations[] );

BICAPI  void  transpose(
    int     rows,
    int     cols,
    VIO_Real    **mat,
    VIO_Real    **mat_transpose );

BICAPI  void  matrix_multiply(
    int    ldim,
    int    mdim,
    int    ndim, 
    VIO_Real   **Amat,
    VIO_Real   **Bmat,
    VIO_Real   **Cmat );

BICAPI  VIO_BOOL  optimize_simplex(
    VIO_Real           **pts1,
    VIO_Real           **pts2, 
    int            npoints, 
    Trans_type     trans_type,
    VIO_Real           center[],
    VIO_Real           translations[],
    VIO_Real           scales[],
    VIO_Real           shears[],
    VIO_Real           rotations[] );

BICAPI  void  procrustes(
    int         npoints,
    int         ndim, 
    VIO_Real        **Apoints,
    VIO_Real        **Bpoints,
    VIO_Real        translation[],
    VIO_Real        centre_of_rotation[],
    Transform   *rotation_transform,
    VIO_Real        *scale_ptr );

BICAPI  VIO_BOOL  rotmat_to_ang(
    Transform   *rot_trans,
    VIO_Real        *ang );

BICAPI void safe_compute_transform_from_tags(
    int                 npoints, 
    VIO_Real                **tag_list1, 
    VIO_Real                **tag_list2, 
    Trans_type          trans_type,
    VIO_General_transform   *transform );

BICAPI  void  make_scale_transform( 
    VIO_Real        sx,
    VIO_Real        sy,
    VIO_Real        sz,
    Transform   *transform );

BICAPI  void  set_transform_x_and_z_axes(
    Transform   *transform,
    Vector      *x_axis,
    Vector      *z_axis );

BICAPI  void  make_translation_transform(
    VIO_Real        x_trans,
    VIO_Real        y_trans,
    VIO_Real        z_trans,
    Transform   *transform );

BICAPI  void  make_origin_transform(
    Point       *origin,
    Transform   *transform );

BICAPI  void  make_rotation_transform(
    VIO_Real       radians,
    int        axis,
    Transform  *transform );

BICAPI  void  make_transform_relative_to_point(
    Point      *point,
    Transform  *transform,
    Transform  *rel_transform );

BICAPI  void  make_transform_in_coordinate_system(
    Point      *origin,
    Vector     *x_axis,
    Vector     *y_axis,
    Vector     *z_axis,
    Transform  *transform,
    Transform  *rel_transform );

BICAPI  void  make_rotation_about_axis(
    Vector     *axis,
    VIO_Real       angle,
    Transform  *transform );

BICAPI  void  convert_2d_transform_to_rotation_translation(
    Transform  *transform,
    VIO_Real       *degrees_clockwise,
    VIO_Real       *x_trans,
    VIO_Real       *y_trans );

BICAPI  VIO_Real  compute_clockwise_rotation( VIO_Real x, VIO_Real y );

BICAPI  VIO_BOOL  is_transform_right_handed(
    Transform   *transform );

BICAPI  void  make_identity_transform_2d(
    Transform_2d *transform );

BICAPI  void  get_inverse_transform_2d(
    Transform_2d   *transform,
    Transform_2d   *inverse );

BICAPI  void  transform_point_2d(
    Transform_2d   *transform,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           *x_trans,
    VIO_Real           *y_trans );

BICAPI  void  get_least_squares_transform_2d(
    int           n_points,
    VIO_Real          x[],
    VIO_Real          y[],
    VIO_Real          x_trans[],
    VIO_Real          y_trans[],
    Transform_2d  *transform_2d );

BICAPI  Status  write_transform_file(
    STRING       filename,
    STRING       comments,
    Transform    *transform );

BICAPI  Status  read_transform_file(
    STRING       filename,
    Transform    *transform );

#ifdef __cplusplus
}
#endif

#endif
