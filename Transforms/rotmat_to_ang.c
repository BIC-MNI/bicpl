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

private void print_rot_error(
    char *s, char * d1, int d2, int d3, int d4, int d5, int d6, int d7);

public  BOOLEAN  rotmat_to_ang(
     Real  **rot_mat,
     Real  *ang )
{
    Real     rx,ry,rz, **t,**s, **R, **Rx, **Ry, **Rz, len, i,j,k;
    int      m,n;

    ALLOC2D( t, 4, 1 );      /* make two column vectors */
    ALLOC2D( s, 4, 1 );

    ALLOC2D( R, 4, 4 );      /* working space matrices */
    ALLOC2D( Rx, 4, 4 );
    ALLOC2D( Ry, 4, 4 );
    ALLOC2D( Rz, 4, 4 );

    nr_ident( R, 4, 4 );        /* init R homogeneous matrix */

    for_less( m, 0, 3 )         /* copy rot matrix into R */
        for_less( n, 0, 3 )
            R[m][n] = rot_mat[m][n];
   
/* ---------------------------------------------------------------
   step one,  find the RZ rotation reqd to bring 
              the local x into the world XZ plane
*/

    for_less( m, 0, 3 )       /* extract local x vector, ie the first column */
        t[m][0] = R[m][0];
    t[3][0] = 1.0;

    i = t[0][0];                        /* make local vector componants */
    j = t[1][0]; 
    k = t[2][0];

    if( i < EPS )
    {                        /* if i is not already in the positive X range, */
        print_rot_error(
          "step one: rz not in the range -pi/2..pi/2",__FILE__, __LINE__,
             0,0,0,0,0 );
        return(FALSE);
    }

    len = sqrt(i*i + j*j);        /* length of vect x on XY plane */
    if( ABS(len) < EPS )
    {
        print_rot_error(
            "step one: length of vect x null.",__FILE__, __LINE__,0,0,0,0,0 );
        return(FALSE);
    }

    if( ABS(i) > ABS(j) )
        rz = ABS( asin(j/len) );
    else
        rz = ABS( acos(i/len) );

    if( j > 0 )                   /* what is the counter clockwise angle */
        rz = -rz;                 /* necessary to bring vect x ont XY plane? */
      
  
/*---------------------------------------------------------------
   step two:  find the RY rotation reqd to align 
              the local x on the world X axis 

   (since i was positive above, RY should already by in range -pi/2..pi/2 
   but we'll check it  anyway)                                             */

    for_less( m, 0, 3 )                /* extract local x vector */
       t[m][0] = R[m][0];
    t[3][0] = 1.0;

    nr_rotz( Rz, rz );             /* create the rotate Z matrix */
 
    matrix_multiply( 4, 4, 1, Rz, t, s ); /* apply RZ, to get x in XZ plane */

    i = s[0][0];                        /* make local vector componants */
    j = s[1][0]; 
    k = s[2][0];

    if( i < EPS )
    {
        print_rot_error(
              "step two: ry not in the range -pi/2..pi/2",__FILE__, __LINE__,
              0,0,0,0,0);
        return(FALSE);
    }

    len = sqrt(i*i + k*k);        /* length of vect x in XZ plane, after RZ */

    if( ABS(len) < EPS )
    {
        print_rot_error(
           "step two: length of vect z null.",__FILE__, __LINE__,0,0,0,0,0);
        return(FALSE);
    }

    if( ABS(i) > ABS(k) )
        ry = ABS( asin(k/len) );
    else
        ry = ABS( acos(i/len) );

    /*    what is the counter clockwise angle necessary to bring  */
    /*    vect x onto X? */

    if( k < 0.0 )
        ry = -ry;

   /*--------------------------------------------------------------- */
   /*   step three,rotate around RX to */
   /*              align the local y with Y and z with Z */

    for_less( m, 0, 3 )                /* extract local z vector */
        t[m][0] = R[m][2];
    t[3][0] = 1.0;

    nr_roty( Ry, ry );             /* create the rotate Y matrix */

                       /* t =  roty(ry*180/pi) *(rotz(rz*180/pi) *r(3,:)); */
    matrix_multiply( 4, 4, 1, Rz, t, s ); /* apply RZ, to get x in XZ plane */
    matrix_multiply( 4, 4, 1, Ry, s, t ); /* apply RY, to get x onto X      */

    i = t[0][0];                        /* make local vector componants */
    j = t[1][0]; 
    k = t[2][0];

    len = sqrt(j*j + k*k);        /* length of vect x in Y,Z plane */

    if( ABS(len) < EPS )
    {
        print_rot_error(
           "step three: length of vect z null.",__FILE__, __LINE__,0,0,0,0,0);
        return(FALSE);
    }

    if( ABS(k) > ABS(j) )
        rx = ABS( asin(j/len) );
    else
        rx = ABS( acos(k/len) );

    if( j < 0.0 )
        rx = -rx;
        
    rx = -rx;  /* these are the required rotations */
    ry = -ry;
    rz = -rz;

    ang[0] = rx;
    ang[1] = ry;
    ang[2] = rz;

    FREE2D( t );
    FREE2D( s );
    FREE2D( R );
    FREE2D( Rx );
    FREE2D( Ry );
    FREE2D( Rz );

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

private  void  print_rot_error(
    char   *s,
    char   *d1,
    int    d2,
    int    d3,
    int    d4,
    int    d5,
    int    d6,
    int    d7 )
{
    print_error( "Error in file %s, line %d\n   ",d1,d2 );
    print_error( s, d3,d4,d5,d6,d7 );
    print_error( "\n" );
    exit(EXIT_FAILURE);
}
