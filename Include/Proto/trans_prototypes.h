#ifndef  DEF_trans_prototypes
#define  DEF_trans_prototypes

public  void  get_nonlinear_warp(
   VIO_Real     **positions,   /* n_points x n_dims */
   VIO_Real     **values,   /* n_points x n_values */
   VIO_Real     **INVMLY,   /* n_points+1+n_dims x n_values */
   int      n_points,
   int      n_dims,
   int      n_values );

public  void  compute_transform_from_tags(
    int                 npoints, 
    VIO_Real                **tag_list1, 
    VIO_Real                **tag_list2, 
    Trans_type          trans_type,
    VIO_General_transform   *transform);

public  void  build_transformation_matrix(
    Transform   *lt, 
    VIO_Real        center[],
    VIO_Real        translations[],
    VIO_Real        scales[],
    VIO_Real        shears[],
    VIO_Real        rotations[] );

public  void  transpose(
    int     rows,
    int     cols,
    VIO_Real    **mat,
    VIO_Real    **mat_transpose );

public  void  matrix_multiply(
    int    ldim,
    int    mdim,
    int    ndim, 
    VIO_Real   **Amat,
    VIO_Real   **Bmat,
    VIO_Real   **Cmat );

public  VIO_BOOL  optimize_simplex(
    VIO_Real           **pts1,
    VIO_Real           **pts2, 
    int            npoints, 
    Trans_type     trans_type,
    VIO_Real           center[],
    VIO_Real           translations[],
    VIO_Real           scales[],
    VIO_Real           shears[],
    VIO_Real           rotations[] );

public  void  procrustes(
    int         npoints,
    int         ndim, 
    VIO_Real        **Apoints,
    VIO_Real        **Bpoints,
    VIO_Real        translation[],
    VIO_Real        centre_of_rotation[],
    Transform   *rotation_transform,
    VIO_Real        *scale_ptr );

public  VIO_BOOL  rotmat_to_ang(
    Transform   *rot_trans,
    VIO_Real        *ang );

public void safe_compute_transform_from_tags(
    int                 npoints, 
    VIO_Real                **tag_list1, 
    VIO_Real                **tag_list2, 
    Trans_type          trans_type,
    VIO_General_transform   *transform );

public  void  make_scale_transform( 
    VIO_Real        sx,
    VIO_Real        sy,
    VIO_Real        sz,
    Transform   *transform );

public  void  set_transform_x_and_z_axes(
    Transform   *transform,
    Vector      *x_axis,
    Vector      *z_axis );

public  void  make_translation_transform(
    VIO_Real        x_trans,
    VIO_Real        y_trans,
    VIO_Real        z_trans,
    Transform   *transform );

public  void  make_origin_transform(
    Point       *origin,
    Transform   *transform );

public  void  make_rotation_transform(
    VIO_Real       radians,
    int        axis,
    Transform  *transform );

public  void  make_transform_relative_to_point(
    Point      *point,
    Transform  *transform,
    Transform  *rel_transform );

public  void  make_transform_in_coordinate_system(
    Point      *origin,
    Vector     *x_axis,
    Vector     *y_axis,
    Vector     *z_axis,
    Transform  *transform,
    Transform  *rel_transform );

public  void  make_rotation_about_axis(
    Vector     *axis,
    VIO_Real       angle,
    Transform  *transform );

public  void  convert_2d_transform_to_rotation_translation(
    Transform  *transform,
    VIO_Real       *degrees_clockwise,
    VIO_Real       *x_trans,
    VIO_Real       *y_trans );

public  VIO_Real  compute_clockwise_rotation( VIO_Real x, VIO_Real y );

public  void  make_identity_transform_2d(
    Transform_2d *transform );

public  void  get_inverse_transform_2d(
    Transform_2d   *transform,
    Transform_2d   *inverse );

public  void  transform_point_2d(
    Transform_2d   *transform,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           *x_trans,
    VIO_Real           *y_trans );

public  void  get_least_squares_transform_2d(
    int           n_points,
    VIO_Real          x[],
    VIO_Real          y[],
    VIO_Real          x_trans[],
    VIO_Real          y_trans[],
    Transform_2d  *transform_2d );

public  Status  write_transform_file(
    char       filename[],
    char       comments[],
    Transform  *transform );

public  Status  read_transform_file(
    char       filename[],
    Transform  *transform );
#endif
