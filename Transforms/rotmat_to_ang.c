/* ----------------------------- MNI Header ----------------------------------- @NAME       : rotmat_to_ang.c
@INPUT      : rot      - rotation matrix (3x3 in num recipes form) calculated
                         by the calling program.
@OUTPUT     : ang      - vector giving rx,ry and rz rotation angles (in 
                         radians). This vector must be defined by the 
                         calling routine.
@RETURNS    : TRUE if ok, FALSE if error.
@DESCRIPTION: this routine extracts the rotation angles from the rotation
              matrix.  The rotation matrix is assumed to be a 3x3 matrix in
              numerical recipes form.  It is locally copied into a 
              4x4 homogeneous matrix for manipulation.

              we assume that the matrix rotation center is (0,0,0).
              we assume that the application of this matrix to a vector
                 is done with rot_mat*vec = premultiplication by matrix

              the resulting angles rx=ang[1],ry=ang[2],rz=ang[3], follow
              the following assumptions:

              -all rotations are assumed to be in the range -pi/2..pi/2
               routine returns FALSE is this is found not to hold
              -rotations are applied 1 - rx, 2 - ry and 3 - rz
              -applying these rotations to an identity matrix will
               result in a matrix equal to `rot' (the input matrix)
              -positive rotations are counter-clockwise when looking
               down the axis, from the positive end towards the origin.
              -I assume a coordinate system:
                          ^ Y
                          |
                          |
                          |
                          |
                          +---------> X
                         /
                        /
                       / Z  (towards the viewer).

              -The problem is posed as:  
                 given a rotation matrix ROT, determine the rotations
                 rx,ry,rz applied in order to give ROT
               solution:
                 assume the rot matrix is equivalent to a normalized
                 orthogonal local coord sys.  i.e.  row 1 of ROT is
                 the local x direction, row 2 is the local y and row 3
                 is the local z.
             
                 (note local is lower case, world is UPPER)

                 1- find RZ that brings local x into the XZ plane
                 2- find RY that brings local x*RZ onto X
                 3- find RX that brings local y*RZ*RY onto Y

                 the required rotations are -RX,-RY and -RZ!

@GLOBALS    : 
@CALLS      : mfmult(), rotx(),roty(),rotz(),matrix(),vector()
@CREATED    : Feb 9, 1992 lc
@MODIFIED   : 
Tue Jun  8 08:44:59 EST 1993 LC
   changes all vec*matrix to matrix*vec.  Std is premultiplication by matrix!
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code

---------------------------------------------------------------------------- */

#include <internal_volume_io.h>
#include <geom.h>

#define EPS  0.00000000001        /* epsilon, should be calculated */

public  BOOLEAN  rotmat_to_ang(
     Real  **rot_mat,
     Real  *ang )
{
    Real       rx, ry, rz, vx, vy, vz;
    int        m,n;
    Vector     x_axis, z_axis;
    Transform  rot_trans, z_rot, y_rot;

    make_identity_transform( &rot_trans );

    for_less( m, 0, N_DIMENSIONS )
    {
        for_less( n, 0, N_DIMENSIONS )
            Transform_elem(rot_trans,m,n) = rot_mat[m][n];
    }

    get_transform_x_axis( &rot_trans, &x_axis );
    get_transform_z_axis( &rot_trans, &z_axis );
   
/* ---------------------------------------------------------------
   step one,  find the RZ rotation reqd to bring 
              the local x into the world XZ plane
*/

    rz = compute_clockwise_rotation( Vector_x(x_axis), Vector_y(x_axis) );

    if( rz >= PI )
        rz -= 2.0 * PI;

/*---------------------------------------------------------------
   step two:  find the RY rotation reqd to align 
              the local x on the world X axis 

   (since i was positive above, RY should already by in range -pi/2..pi/2 
   but we'll check it  anyway)                                             */

    make_rotation_transform( -rz, Z, &z_rot );

    transform_vector( &z_rot, Vector_x(x_axis), Vector_y(x_axis),
                      Vector_z(x_axis), &vx, &vy, &vz );

    ry = - compute_clockwise_rotation( vx, vz );

    if( ry <= -PI )
        ry += 2.0 * PI;

   /*--------------------------------------------------------------- */
   /*   step three,rotate around RX to */
   /*              align the local y with Y and z with Z */

    make_rotation_transform( -ry, Y, &y_rot );

    transform_vector( &z_rot, Vector_x(z_axis), Vector_y(z_axis),
                      Vector_z(z_axis), &vx, &vy, &vz );
    transform_vector( &y_rot, vx, vy, vz, &vx, &vy, &vz );

    rx = - compute_clockwise_rotation( vz, vy );

    if( rx <= -PI )
        rx += 2.0 * PI;

    rx = -rx;  /* these are the required rotations */
    ry = -ry;
    rz = -rz;

#ifdef DEBUG
#define  DEBUG
{
    BOOLEAN  error;
    Transform  test, Rx, Ry, Rz;

    make_rotation_transform( -rx, X, &Rx ) ;
    make_rotation_transform( -ry, Y, &Ry ) ;
    make_rotation_transform( -rz, Z, &Rz ) ;

    concat_transforms( &test, &Rx, &Ry );
    concat_transforms( &test, &test, &Rz );


    error = FALSE;
    for_less( m, 0, N_DIMENSIONS )
        for_less( n, 0, N_DIMENSIONS )
            if( !numerically_close( Transform_elem(test,m,n), 
                                    Transform_elem(rot_trans,m,n), 1.0e-3 ) )
            {
                error = TRUE;
            }

    if( error )
    {
        for_less( m, 0, N_DIMENSIONS )
        {
            for_less( n, 0, N_DIMENSIONS )
                print( " %g", Transform_elem(test,m,n) );
            print( "\n" );
        }
        print( "\n" );
        for_less( m, 0, N_DIMENSIONS )
        {
            for_less( n, 0, N_DIMENSIONS )
                print( " %g", Transform_elem(rot_trans,m,n) );
            print( "\n" );
        }

        handle_internal_error( "rotmat_to_ang" );
    }
}
#endif

    ang[0] = rx;
    ang[1] = ry;
    ang[2] = rz;

    return(TRUE);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_ident - make identity matrix
@INPUT      : A - pointer to matrix
              m - row limits
              n - col limits
              (matrix in numerical recipes form, allocated by calling routine)
@OUTPUT     : identiy matrix in A
@RETURNS    : (nothing)
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Tue Jun  1 12:49:21 EST 1993 (Louis Collins)
@MODIFIED   : 

---------------------------------------------------------------------------- */

public  void  nr_ident(
    Real   **A,
    int    m,
    int    n )
{
    int i, j;

    for_less( i, 0, m )
    {
        for_less( j, 0, n )
        {
            if( i == j ) 
                A[i][j] = 1.0;
            else
                A[i][j] = 0.0;
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_rotx - make rot X matrix
@INPUT      : M - 4x4 matrix
              a - rotation angle in radians
              (matrix in numerical recipes form, allocated by calling routine)
@OUTPUT     : modified matrix M
@RETURNS    : (nothing)
@DESCRIPTION: 
@METHOD     : 
   rx =[1   0      0      0 
        0  cos(a)  -sin(a) 0
        0 sin(a)  cos(a) 0
        0   0      0      1];
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Tue Jun  1 12:49:21 EST 1993 (Louis Collins)
@MODIFIED   : Tue Jun  8 08:44:59 EST 1993 (LC) changed to mat*vec format
---------------------------------------------------------------------------- */

public  void  nr_rotx(
    Real   **M,
    Real   a )
{
    nr_ident( M, 4, 4 );

    M[1][1] = cos(a);   M[1][2] = -sin(a);
    M[2][1] = sin(a);   M[2][2] = cos(a);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_roty - make rot Y matrix
@INPUT      : M - 4x4 matrix
              a - rotation angle in radians
              (matrix in numerical recipes form, allocated by calling routine)
@RETURNS    : (nothing)
@DESCRIPTION: 
@METHOD     : 
ry = [  cos(a)   0 sin(a)  0 
        0       1   0       0
        -sin(a)  0  cos(a)   0
        0   0      0      1];
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Tue Jun  1 12:49:21 EST 1993 (Louis Collins)
@MODIFIED   : Tue Jun  8 08:44:59 EST 1993 (LC) changed to mat*vec format
---------------------------------------------------------------------------- */

public  void  nr_roty(
    Real   **M,
    Real   a )
{
    nr_ident( M, 4, 4 );

    M[0][0] =  cos(a);   M[0][2] = sin(a);
    M[2][0] = -sin(a);   M[2][2] = cos(a);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_rotz - make rot Z matrix
@INPUT      : M - 4x4 matrix
              a - rotation angle in radians
              (matrix in numerical recipes form, allocated by calling routine)
@RETURNS    : (nothing)
@DESCRIPTION: 
@METHOD     : 
rz = [cos(a)  -sin(a) 0  0
      sin(a) cos(a) 0  0
        0     0      1  0
        0     0      0  1];
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Tue Jun  1 12:49:21 EST 1993 (Louis Collins)
@MODIFIED   : Tue Jun  8 08:44:59 EST 1993 (LC) changed to mat*vec format
---------------------------------------------------------------------------- */

public  void  nr_rotz(
    Real     **M,
    Real     a )
{
    nr_ident( M, 4, 4 );

    M[0][0] = cos(a);   M[0][1] = -sin(a);
    M[1][0] = sin(a);   M[1][1] = cos(a);
}
