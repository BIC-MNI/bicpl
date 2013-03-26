#include  "bicpl_internal.h"

static void   trilinear_interpolate_volume(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    VIO_BOOL        x_use_higher,
    VIO_BOOL        y_use_higher,
    VIO_BOOL        z_use_higher,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_z );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_3D_volume_in_world
@INPUT      : volume
              x
              y
              z
              degrees_continuity - 0 = linear, 2 = cubic
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : 
@DESCRIPTION: Takes a world space position and evaluates the value within
              the volume.
              If deriv_x is not a null pointer, then the 3 derivatives are
              passed back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void   evaluate_3D_volume_in_world(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    int            degrees_continuity,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_z,
    VIO_Real           *deriv_xx,
    VIO_Real           *deriv_xy,
    VIO_Real           *deriv_xz,
    VIO_Real           *deriv_yy,
    VIO_Real           *deriv_yz,
    VIO_Real           *deriv_zz )
{
    VIO_Real      ignore, dxx, dxy, dxz, dyy, dyz, dzz;
    VIO_Real      voxel[MAX_DIMENSIONS];
    VIO_Real      txx, txy, txz;
    VIO_Real      tyx, tyy, tyz;
    VIO_Real      tzx, tzy, tzz;
    VIO_Real      derivs[MAX_DIMENSIONS];

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error(
                 "evaluate_3D_volume_in_world: volume must be 3D.\n" );
    }

    convert_world_to_voxel( volume, x, y, z, voxel );

    evaluate_3D_volume( volume, voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z],
                        degrees_continuity, value, deriv_x, deriv_y, deriv_z,
                        deriv_xx, deriv_xy, deriv_xz,
                        deriv_yy, deriv_yz, deriv_zz );

    if( deriv_x != (VIO_Real *) 0 )
    {
        derivs[0] = *deriv_x;
        derivs[1] = *deriv_y;
        derivs[2] = *deriv_z;
        convert_voxel_normal_vector_to_world( volume, derivs,
                                              deriv_x, deriv_y, deriv_z );
    }

    if( deriv_xx != (VIO_Real *) 0 )
    {
        dxx = *deriv_xx;
        dxy = *deriv_xy;
        dxz = *deriv_xz;
        dyy = *deriv_yy;
        dyz = *deriv_yz;
        dzz = *deriv_zz;
        convert_voxel_normal_vector_to_world( volume,
                                              dxx, dxy, dxz,
                                              &txx, &txy, &txz );
        convert_voxel_normal_vector_to_world( volume,
                                              dxy, dyy, dyz,
                                              &tyx, &tyy, &tyz );
        convert_voxel_normal_vector_to_world( volume,
                                              dxz, dyz, dzz,
                                              &tzx, &tzy, &tzz );

        convert_voxel_normal_vector_to_world( volume,
                                              txx, tyx, tzx,
                                              deriv_xx, &ignore, &ignore );
        convert_voxel_normal_vector_to_world( volume,
                                              txy, tyy, tzy,
                                              deriv_xy, deriv_yy, &ignore );
        convert_voxel_normal_vector_to_world( volume,
                                              txz, tyz, tzz,
                                              deriv_xz, deriv_yz, deriv_zz );
    }
}

BICAPI void   special_evaluate_3D_volume_in_world(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    VIO_BOOL        x_use_higher,
    VIO_BOOL        y_use_higher,
    VIO_BOOL        z_use_higher,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_z )
{
    VIO_Real      voxel[MAX_DIMENSIONS];
    int       sizes[MAX_DIMENSIONS];

    convert_world_to_voxel( volume, x, y, z, voxel );

    get_volume_sizes( volume, sizes );
    if( x < -0.5 || x > (VIO_Real) sizes[VIO_X] - 0.5 ||
        y < -0.5 || y > (VIO_Real) sizes[VIO_Y] - 0.5 ||
        z < -0.5 || z > (VIO_Real) sizes[VIO_Z] - 0.5 )
    {
        *value = get_volume_voxel_min( volume );
        if( deriv_x != (VIO_Real *) NULL )
        {
            *deriv_x = 0.0;
            *deriv_y = 0.0;
            *deriv_z = 0.0;
        }
        return;
    }

    trilinear_interpolate_volume( volume, voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z],
                                  x_use_higher, y_use_higher, z_use_higher,
                                  value, deriv_x, deriv_y, deriv_z );

    if( deriv_x != (VIO_Real *) 0 )
    {
        convert_voxel_normal_vector_to_world( volume,
                                              *deriv_x, *deriv_y, *deriv_z,
                                              deriv_x, deriv_y, deriv_z );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_3D_slice_in_world
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_xx
              deriv_xy
              deriv_yy
@RETURNS    : 
@DESCRIPTION: Takes a world space position and evaluates the value within
              the volume by bilinear interpolation within the slice.
              If deriv_x is not a null pointer, then the derivatives are passed
              back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void   evaluate_3D_slice_in_world(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_xx,
    VIO_Real           *deriv_xy,
    VIO_Real           *deriv_yy )
{
    VIO_Real      ignore, dxx, dxy, dyy;
    VIO_Real      voxel[MAX_DIMENSIONS];
    VIO_Real      txx, txy, txz;
    VIO_Real      tyx, tyy, tyz;
    VIO_Real      tzx, tzy, tzz;

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error(
                 "evaluate_3D_slice_in_world: volume must be 3D.\n" );
    }

    convert_world_to_voxel( volume, x, y, z, voxel );

    evaluate_3D_slice( volume, voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z],
                       value, deriv_x, deriv_y,
                       deriv_xx, deriv_xy, deriv_yy );

    if( deriv_x != (VIO_Real *) 0 )
    {
        convert_voxel_normal_vector_to_world( volume,
                                              *deriv_x, *deriv_y, 0.0,
                                              deriv_x, deriv_y, &ignore );
    }

    if( deriv_xx != (VIO_Real *) 0 )
    {
        dxx = *deriv_xx;
        dxy = *deriv_xy;
        dyy = *deriv_yy;
        convert_voxel_normal_vector_to_world( volume,
                                              dxx, dxy, 0.0,
                                              &txx, &txy, &txz );
        convert_voxel_normal_vector_to_world( volume,
                                              dxy, dyy, 0.0,
                                              &tyx, &tyy, &tyz );
        convert_voxel_normal_vector_to_world( volume,
                                              0.0, 0.0, 0.0,
                                              &tzx, &tzy, &tzz );

        convert_voxel_normal_vector_to_world( volume,
                                              txx, tyx, tzx,
                                              deriv_xx, &ignore, &ignore );
        convert_voxel_normal_vector_to_world( volume,
                                              txy, tyy, tzy,
                                              deriv_xy, deriv_yy, &ignore );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : triconstant_interpolate_volume
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : 
@DESCRIPTION: Returns the value within the volume, assuming constant voxels,
              (step function).  Derivative is approximated by neighbours.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static void   triconstant_interpolate_volume(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_z )
{
    int                i, j, k, sizes[MAX_DIMENSIONS];
    VIO_Real               prev, next, dx, dy, dz, voxel_value;

    get_volume_sizes( volume, sizes );

    i = VIO_ROUND( x );
    if( i == sizes[VIO_X] )
        i = sizes[VIO_X]-1;
    j = VIO_ROUND( y );
    if( j == sizes[VIO_Y] )
        j = sizes[VIO_Y]-1;
    k = VIO_ROUND( z );
    if( k == sizes[VIO_Z] )
        k = sizes[VIO_Z]-1;

    GET_VALUE_3D( voxel_value, volume, i, j, k );

    if( value != (VIO_Real *) 0 )
        *value = voxel_value;

    if( deriv_x != (VIO_Real *) NULL )
    {
        /* --- get derivative wrt x */

        dx = 0;
        if( i == 0 )
            prev = voxel_value;
        else
        {
            GET_VALUE_3D( prev, volume, i-1, j, k );
            ++dx;
        }

        if( i == sizes[VIO_X]-1 )
            next = voxel_value;
        else
        {
            GET_VALUE_3D( next, volume, i+1, j, k );
            ++dx;
        }

        if( dx == 0 )
            *deriv_x = 0.0;
        else
            *deriv_x = (next - prev) / (VIO_Real) dx;

        /* --- get derivative wrt y */

        dy = 0;
        if( j == 0 )
            prev = voxel_value;
        else
        {
            GET_VALUE_3D( prev, volume, i, j-1, k );
            ++dy;
        }

        if( j == sizes[VIO_Y]-1 )
            next = voxel_value;
        else
        {
            GET_VALUE_3D( next, volume, i, j+1, k );
            ++dy;
        }

        if( dy == 0 )
            *deriv_y = 0.0;
        else
            *deriv_y = (next - prev) / (VIO_Real) dy;

        /* --- get derivative wrt z */

        dz = 0;
        if( k == 0 )
            prev = voxel_value;
        else
        {
            GET_VALUE_3D( prev, volume, i, j, k-1 );
            ++dz;
        }

        if( k == sizes[VIO_Z]-1 )
            next = voxel_value;
        else
        {
            GET_VALUE_3D( next, volume, i, j, k+1 );
            ++dz;
        }

        if( dz == 0 )
            *deriv_z = 0.0;
        else
            *deriv_z = (next - prev) / (VIO_Real) dz;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : trilinear_interpolate_volume
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : 
@DESCRIPTION: Returns the value within the volume, assuming trilinear
              interpolation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static void   trilinear_interpolate_volume(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    VIO_BOOL        x_use_higher,
    VIO_BOOL        y_use_higher,
    VIO_BOOL        z_use_higher,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_z )
{
    int                i, j, k, sizes[MAX_DIMENSIONS];
    VIO_Real               u, v, w;
    VIO_Real               c000, c001, c010, c011, c100, c101, c110, c111;
    VIO_Real               c00, c01, c10, c11;
    VIO_Real               c0, c1;
    VIO_Real               du00, du01, du10, du11, du0, du1;
    VIO_Real               dv0, dv1;

    get_volume_sizes( volume, sizes );

    if( x == (VIO_Real) (sizes[VIO_X]-1) )
    {
        i = sizes[VIO_X]-2;
        u = 1.0;
    }
    else
    {
        i = (int) x;
        u = VIO_FRACTION( x );
        if( !x_use_higher && u == 0.0 && i > 0 )
        {
            --i;
            u = 1.0;
        }
    }

    if( y == (VIO_Real) (sizes[VIO_Y]-1) )
    {
        j = sizes[VIO_Y]-2;
        v = 1.0;
    }
    else
    {
        j = (int) y;
        v = VIO_FRACTION( y );
        if( !y_use_higher && v == 0.0 && j > 0 )
        {
            --j;
            v = 1.0;
        }
    }

    if( z == (VIO_Real) (sizes[VIO_Z]-1) )
    {
        k = sizes[VIO_Z]-2;
        w = 1.0;
    }
    else
    {
        k = (int) z;
        w = VIO_FRACTION( z );
        if( !z_use_higher && w == 0.0 && k > 0 )
        {
            --k;
            w = 1.0;
        }
    }

    GET_VALUE_3D( c000, volume, i,   j,   k );
    GET_VALUE_3D( c001, volume, i,   j,   k+1 );
    GET_VALUE_3D( c010, volume, i,   j+1, k );
    GET_VALUE_3D( c011, volume, i,   j+1, k+1 );
    GET_VALUE_3D( c100, volume, i+1, j,   k );
    GET_VALUE_3D( c101, volume, i+1, j,   k+1 );
    GET_VALUE_3D( c110, volume, i+1, j+1, k );
    GET_VALUE_3D( c111, volume, i+1, j+1, k+1 );

    du00 = c100 - c000;
    du01 = c101 - c001;
    du10 = c110 - c010;
    du11 = c111 - c011;

    c00 = c000 + u * du00;
    c01 = c001 + u * du01;
    c10 = c010 + u * du10;
    c11 = c011 + u * du11;

    dv0 = c10 - c00;
    dv1 = c11 - c01;

    c0 = c00 + v * dv0;
    c1 = c01 + v * dv1;

    if( value != (VIO_Real *) 0 )
        *value = VIO_INTERPOLATE( w, c0, c1 );

    if( deriv_x != (VIO_Real *) 0 )
    {
        du0 = VIO_INTERPOLATE( v, du00, du10 );
        du1 = VIO_INTERPOLATE( v, du01, du11 );

        *deriv_x = VIO_INTERPOLATE( w, du0, du1 );
        *deriv_y = VIO_INTERPOLATE( w, dv0, dv1 );
        *deriv_z = (c1 - c0);
    }
}

static void   trivar_interpolate_volume(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    int            degree,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_z,
    VIO_Real           *deriv_xx,
    VIO_Real           *deriv_xy,
    VIO_Real           *deriv_xz,
    VIO_Real           *deriv_yy,
    VIO_Real           *deriv_yz,
    VIO_Real           *deriv_zz )
{
    int                i, j, k, tu, tv, tw, ind, n_derivs;
    VIO_Real               u, v, w, bound;
    VIO_Real               coefs[4*4*4];
    VIO_Real               derivs[3*3*3];
    int                sizes[MAX_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    bound = (degree - 2) / 2.0;

    if( x >= (VIO_Real) sizes[VIO_X] - 1.0 - bound )
    {
        i = sizes[VIO_X] - degree;
        u = 1.0;
    }
    else
    {
        if( x < bound )
            x = bound;

        i = (int) ( x - bound );
        u = VIO_FRACTION( x - bound );
    }

    if( y >= (VIO_Real) sizes[VIO_Y] - 1.0 - bound )
    {
        j = sizes[VIO_Y] - degree;
        v = 1.0;
    }
    else
    {
        if( y < bound )
            y = bound;

        j = (int) ( y - bound );
        v = VIO_FRACTION( y - bound );
    }

    if( z >= (VIO_Real) sizes[VIO_Z] - 1.0 - bound )
    {
        k = sizes[VIO_Z] - degree;
        w = 1.0;
    }
    else
    {
        if( z < bound )
            z = bound;

        k = (int) ( z - bound );
        w = VIO_FRACTION( z - bound );
    }

    ind = 0;
    for_less( tu, 0, degree )
    for_less( tv, 0, degree )
    for_less( tw, 0, degree )
    {
        GET_VALUE_3D( coefs[ind], volume, i+tu, j+tv, k+tw );
        ++ind;
    }

    if( deriv_xx != NULL )
        n_derivs = 2;
    else if( deriv_x != NULL )
        n_derivs = 1;
    else
        n_derivs = 0;

    evaluate_trivariate_catmull_spline( u, v, w, degree, coefs, n_derivs,
                                        derivs );

    if( deriv_xx != NULL )
    {
        *deriv_xx = derivs[VIO_IJK(2,0,0,n_derivs+1,n_derivs+1)];
        *deriv_xy = derivs[VIO_IJK(1,1,0,n_derivs+1,n_derivs+1)];
        *deriv_xz = derivs[VIO_IJK(1,0,1,n_derivs+1,n_derivs+1)];
        *deriv_yy = derivs[VIO_IJK(0,2,0,n_derivs+1,n_derivs+1)];
        *deriv_yz = derivs[VIO_IJK(0,1,1,n_derivs+1,n_derivs+1)];
        *deriv_zz = derivs[VIO_IJK(0,0,2,n_derivs+1,n_derivs+1)];
    }

    if( deriv_x != NULL )
    {
        *deriv_x = derivs[VIO_IJK(1,0,0,n_derivs+1,n_derivs+1)];
        *deriv_y = derivs[VIO_IJK(0,1,0,n_derivs+1,n_derivs+1)];
        *deriv_z = derivs[VIO_IJK(0,0,1,n_derivs+1,n_derivs+1)];
    }

    if( value != NULL )
        *value = derivs[VIO_IJK(0,0,0,n_derivs+1,n_derivs+1)];
}

static void   bicubic_interpolate_volume(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_xx,
    VIO_Real           *deriv_xy,
    VIO_Real           *deriv_yy )
{
    int                i, j, k, tu, tv, ind, n_derivs;
    VIO_Real               u, v;
    VIO_Real               coefs[4*4];
    VIO_Real               derivs[3*3];
    int                sizes[MAX_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    if( x >= (VIO_Real) sizes[VIO_X] - 1.5 )
    {
        i = sizes[VIO_X]-2;
        u = 1.0;
    }
    else
    {
        i = (int) x;
        u = VIO_FRACTION( x );
    }

    if( y >= (VIO_Real) sizes[VIO_Y] - 1.5 )
    {
        j = sizes[VIO_Y]-2;
        v = 1.0;
    }
    else
    {
        j = (int) y;
        v = VIO_FRACTION( y );
    }

    if( z == (VIO_Real) sizes[VIO_Z] - 1.5 )
    {
        k = sizes[VIO_Z]-2;
    }
    else
    {
        k = (int) z;
    }

    ind = 0;
    for_less( tu, 0, 4 )
    for_less( tv, 0, 4 )
    {
        GET_VALUE_3D( coefs[ind], volume, i-1+tu, j-1+tv, k );
        ++ind;
    }

    if( deriv_xx != NULL )
        n_derivs = 2;
    else if( deriv_x != NULL )
        n_derivs = 1;
    else
        n_derivs = 0;

    evaluate_bivariate_catmull_spline( u, v, 4, coefs, n_derivs, derivs );

    if( deriv_xx != NULL )
    {
        *deriv_xx = derivs[VIO_IJ(2,0,n_derivs+1)];
        *deriv_xy = derivs[VIO_IJ(1,1,n_derivs+1)];
        *deriv_yy = derivs[VIO_IJ(0,2,n_derivs+1)];
    }

    if( deriv_x != NULL )
    {
        *deriv_x = derivs[VIO_IJ(1,0,n_derivs+1)];
        *deriv_y = derivs[VIO_IJ(0,1,n_derivs+1)];
    }

    if( value != NULL )
        *value = derivs[VIO_IJ(0,0,n_derivs+1)];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_3D_volume
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_z
@RETURNS    : 
@DESCRIPTION: Takes a voxel space position and evaluates the value within
              the volume by trilinear interpolation.
              If deriv_x is not a null pointer, then the 3 derivatives are passed
              back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void   evaluate_3D_volume(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    int            degrees_continuity,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_z,
    VIO_Real           *deriv_xx,
    VIO_Real           *deriv_xy,
    VIO_Real           *deriv_xz,
    VIO_Real           *deriv_yy,
    VIO_Real           *deriv_yz,
    VIO_Real           *deriv_zz )
{
    int         sizes[MAX_DIMENSIONS];

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error( "evaluate_3D_volume: volume must be 3D.\n" );
    }

    get_volume_sizes( volume, sizes );

    if( x < -0.5 || x > (VIO_Real) sizes[VIO_X] - 0.5 ||
        y < -0.5 || y > (VIO_Real) sizes[VIO_Y] - 0.5 ||
        z < -0.5 || z > (VIO_Real) sizes[VIO_Z] - 0.5 )
    {
        *value = get_volume_voxel_min( volume );
        if( deriv_x != (VIO_Real *) NULL )
        {
            *deriv_x = 0.0;
            *deriv_y = 0.0;
            *deriv_z = 0.0;
        }
        if( deriv_xx != (VIO_Real *) NULL )
        {
            *deriv_xx = 0.0;
            *deriv_xy = 0.0;
            *deriv_xz = 0.0;
            *deriv_yy = 0.0;
            *deriv_yz = 0.0;
            *deriv_zz = 0.0;
        }
        return;
    }

    if( x < (VIO_Real) degrees_continuity * 0.5 ||
        x > (VIO_Real) (sizes[VIO_X]-1) - (VIO_Real) degrees_continuity * 0.5 ||
        y < (VIO_Real) degrees_continuity * 0.5 ||
        y > (VIO_Real) (sizes[VIO_Y]-1) - (VIO_Real) degrees_continuity * 0.5 ||
        z < (VIO_Real) degrees_continuity * 0.5 ||
        z > (VIO_Real) (sizes[VIO_Z]-1) - (VIO_Real) degrees_continuity * 0.5 )
    {
        degrees_continuity = -1;
    }

    if( degrees_continuity < 1 && deriv_xx != (VIO_Real *) NULL )
    {
        *deriv_xx = 0.0;
        *deriv_xy = 0.0;
        *deriv_xz = 0.0;
        *deriv_yy = 0.0;
        *deriv_yz = 0.0;
        *deriv_zz = 0.0;
    }

    switch( degrees_continuity )
    {
    case -1:
        triconstant_interpolate_volume( volume, x, y, z, value,
                                        deriv_x, deriv_y, deriv_z );
        break;

    case 0:
        trilinear_interpolate_volume( volume, x, y, z,
                                      TRUE, TRUE, TRUE, value,
                                      deriv_x, deriv_y, deriv_z );
        break;

    case 1:
    case 2:
        trivar_interpolate_volume( volume, x, y, z, degrees_continuity+2,
                                   value,
                                   deriv_x, deriv_y, deriv_z,
                                   deriv_xx, deriv_xy, deriv_xz,
                                   deriv_yy, deriv_yz, deriv_zz );
        break;

    default:
        handle_internal_error( "evaluate_3D_volume: invalid continuity" );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_3D_slice
@INPUT      : volume
              x
              y
              z
@OUTPUT     : value
              deriv_x
              deriv_y
              deriv_xx
              deriv_xy
              deriv_yy
@RETURNS    : 
@DESCRIPTION: Takes a voxel position and evaluates the value within
              the volume by bilinear interpolation within the slice.
              If deriv_x is not a null pointer, then the
              derivatives are passed back.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void   evaluate_3D_slice(
    Volume         volume,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           z,
    VIO_Real           *value,
    VIO_Real           *deriv_x,
    VIO_Real           *deriv_y,
    VIO_Real           *deriv_xx,
    VIO_Real           *deriv_xy,
    VIO_Real           *deriv_yy )
{
    int           sizes[MAX_DIMENSIONS];

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error( "evaluate_3D_slice: volume must be 3D.\n" );
    }

    get_volume_sizes( volume, sizes );

    if( x < 1.0 || x > (VIO_Real) sizes[VIO_X] - 2.0 ||
        y < 1.0 || y > (VIO_Real) sizes[VIO_Y] - 2.0 ||
        z < 1.0 || z > (VIO_Real) sizes[VIO_Z] - 2.0 )
    {
        *value = get_volume_voxel_min( volume );
        if( deriv_x != (VIO_Real *) NULL )
        {
            *deriv_x = 0.0;
            *deriv_y = 0.0;
        }
        if( deriv_xx != (VIO_Real *) NULL )
        {
            *deriv_xx = 0.0;
            *deriv_xy = 0.0;
            *deriv_yy = 0.0;
        }
        return;
    }

    bicubic_interpolate_volume( volume, x, y, z, value,
                                deriv_x, deriv_y,
                                deriv_xx, deriv_xy, deriv_yy );
}
