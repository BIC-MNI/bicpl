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

---------------------------------------------------------------------------- */

#include <internal_volume_io.h>
#include <geom.h>
#include <recipes.h>

#ifdef dec
#define  fsin( x )   sinf( x )
#define  fcos( x )   cosf( x )
#define  fasin( x )  asinf( x )
#define  facos( x )  acosf( x )
#endif

#if defined(linux) || defined(sun)
#define  fsin( x )   (float) sin( (double) (x) )
#define  fcos( x )   (float) cos( (double) (x) )
#define  fasin( x )  (float) asin( (double) (x) )
#define  facos( x )  (float) acos( (double) (x) )
#endif

extern char *prog_name;

#define EPS  0.00000000001	/* epsilon, should be calculated */

private void print_rot_error(char *s, char * d1, int d2, int d3, int d4, int d5, int d6, int d7);

public BOOLEAN rotmat_to_ang(float **rot_mat, float *ang)
{

   float 
      rx,ry,rz,
      **t,**s,
      **R,
      **Rx,
      **Ry,
      **Rz,
      len,
      i,j,k;

   int
      m,n;

   t  = matrix(1,4,1,1);	/* make two column vectors */
   s  = matrix(1,4,1,1);

   R  = matrix(1,4,1,4);	/* working space matrices */
   Rx = matrix(1,4,1,4);
   Ry = matrix(1,4,1,4);
   Rz = matrix(1,4,1,4);

   nr_identf(R,1,4,1,4);	/* init R homogeneous matrix */

   for (m=1; m<=3; ++m)		/* copy rot matrix into R */
      for (n=1; n<=3; ++n)
	 R[m][n] = rot_mat[m][n];
   
/* ---------------------------------------------------------------
   step one,  find the RZ rotation reqd to bring 
              the local x into the world XZ plane
*/

   for (m=1; m<=3; ++m)		/* extract local x vector, ie the first column */
      t[m][1] = R[m][1];
   t[4][1] = 1.0;

   i = t[1][1];			/* make local vector componants */
   j = t[2][1]; 
   k = t[3][1];

   if (i<EPS) {			/* if i is not already in the positive X range, */
      print_rot_error("step one: rz not in the range -pi/2..pi/2",__FILE__, __LINE__,0,0,0,0,0);
      return(FALSE);
   }

   len = sqrt(i*i + j*j);	/* length of vect x on XY plane */
   if (ABS(len)<EPS) {
      print_rot_error("step one: length of vect x null.",__FILE__, __LINE__,0,0,0,0,0);
      return(FALSE);
   }

   if (ABS(i)>ABS(j)) {
      rz = ABS(fasin(j/len));
   }
   else {
      rz = ABS(facos(i/len));
   }

   if (j>0)			/* what is the counter clockwise angle */
      rz = -rz;                 /* necessary to bring vect x ont XY plane? */
      
  
/*---------------------------------------------------------------
   step two:  find the RY rotation reqd to align 
              the local x on the world X axis 

  (since i was positive above, RY should already by in range -pi/2..pi/2 
  but we'll check it  anyway)                                             */

   for (m=1; m<=3; ++m)		/* extract local x vector */
      t[m][1] = R[m][1];
   t[4][1] = 1.0;

   nr_rotzf(Rz,rz);             /* create the rotate Z matrix */
 
   nr_multf(Rz,1,4,1,4,  t,1,4,1,1,   s);   /* apply RZ, to get x in XZ plane */

   i = s[1][1];			/* make local vector componants */
   j = s[2][1]; 
   k = s[3][1];

   if (i<EPS) {
      print_rot_error("step two: ry not in the range -pi/2..pi/2",__FILE__, __LINE__,0,0,0,0,0);
      return(FALSE);
   }

   len = sqrt(i*i + k*k);		/* length of vect x in XZ plane, after RZ */

   if (ABS(len)<EPS) {
      print_rot_error("step two: length of vect z null.",__FILE__, __LINE__,0,0,0,0,0);
      return(FALSE);
   }

   if (ABS(i)>ABS(k)) {
      ry = ABS(fasin(k/len));
   }
   else {
      ry = ABS(facos(i/len));
   }

   /*    what is the counter clockwise angle necessary to bring  */
   /*    vect x onto X? */
   if (k < 0) { 
      ry = -ry;
   }

   /*--------------------------------------------------------------- */
   /*   step three,rotate around RX to */
   /*              align the local y with Y and z with Z */

   for (m=1; m<=3; ++m)		/* extract local z vector */
      t[m][1] = R[m][3];
   t[4][1] = 1.0;

   nr_rotyf(Ry,ry);             /* create the rotate Y matrix */

				/* t =  roty(ry*180/pi) *(rotz(rz*180/pi) *r(3,:)); */
   nr_multf(Rz,1,4,1,4,  t,1,4,1,1,  s); /* apply RZ, to get x in XZ plane */
   nr_multf(Ry,1,4,1,4,  s,1,4,1,1,  t); /* apply RY, to get x onto X      */

   i = t[1][1];			/* make local vector componants */
   j = t[2][1]; 
   k = t[3][1];

   len = sqrt(j*j + k*k);	/* length of vect x in Y,Z plane */

   if (ABS(len)<EPS) {
      print_rot_error("step three: length of vect z null.",__FILE__, __LINE__,0,0,0,0,0);
      return(FALSE);
   }

   if (ABS(k)>ABS(j)) {
      rx = ABS(fasin(j/len));
   }
   else {
      rx = ABS(facos(k/len));
   }

   if (j< 0) { 
      rx = -rx;
   }
	
   rx = -rx;  /* these are the required rotations */
   ry = -ry;
   rz = -rz;

   ang[1] = rx;
   ang[2] = ry;
   ang[3] = rz;

   free_matrix(t,1,4,1,1);
   free_matrix(s,1,4,1,1);
   free_matrix(R,1,4,1,4);
   free_matrix(Rx,1,4,1,4);
   free_matrix(Ry,1,4,1,4);
   free_matrix(Rz,1,4,1,4);

   return(TRUE);
}





/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_identd, nr_identf - make identity matrix
@INPUT      : A - pointer to matrix
              m1,m2 - row limits
	      n1,n2 - col limits
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
public void nr_identd(double **A, int m1, int m2, int n1, int n2 )
{

   int i,j;

   for (i=m1; i<=m2; ++i)
      for (j=n1; j<=n2; ++j) {
	 if (i==j) 
	    A[i][j] = 1.0;
	 else
	    A[i][j] = 0.0;
      }
   
}

public void nr_identf(float **A, int m1, int m2, int n1, int n2 )
{

   int i,j;

   for (i=m1; i<=m2; ++i)
      for (j=n1; j<=n2; ++j) {
	 if (i==j) 
	    A[i][j] = 1.0;
	 else
	    A[i][j] = 0.0;
      }
   
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_copyd, nr_copyf - copy matrix
@INPUT      : A - source matrix
              m1,m2 - row limits
	      n1,n2 - col limits
              (matrix in numerical recipes form, allocated by calling routine)
@OUTPUT     : B - copy of A
@RETURNS    : (nothing)
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Tue Jun  1 12:49:21 EST 1993 (Louis Collins)
@MODIFIED   : 

---------------------------------------------------------------------------- */
public void nr_copyd(double **A, int m1, int m2, int n1, int n2, double **B )
{
   int i,j;

   for (i=m1; i<=m2; ++i)
      for (j=n1; j<=n2; ++j)
	 B[i][j] = A[i][j];
}

public void nr_copyf(float  **A, int m1, int m2, int n1, int n2, float **B )
{
   int i,j;

   for (i=m1; i<=m2; ++i)
      for (j=n1; j<=n2; ++j)
	 B[i][j] = A[i][j];
}



/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_rotxd,nr_rotxf - make rot X matrix
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
public void nr_rotxd(double **M, double a)
{
   nr_identd(M,1,4,1,4);

   M[2][2] = cos(a);   M[2][3] = -sin(a);
   M[3][2] = sin(a);   M[3][3] = cos(a);
}


public void nr_rotxf(float **M, float a)
{
   nr_identf(M,1,4,1,4);

   M[2][2] = fcos(a);    M[2][3] = -fsin(a);
   M[3][2] = fsin(a);   M[3][3] = fcos(a);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_rotyd,nr_rotyf - make rot Y matrix
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
public void nr_rotyd(double **M,double a)
{

   nr_identd(M,1,4,1,4);

   M[1][1] = cos(a);   M[1][3] = sin(a);
   M[3][1] = -sin(a);   M[3][3] = cos(a);
}

public void nr_rotyf(float **M, float a)
{

   nr_identf(M,1,4,1,4);

   M[1][1] = fcos(a);   M[1][3] = fsin(a);
   M[3][1] = -fsin(a);   M[3][3] = fcos(a);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_rotzd, nr_rotzf - make rot Z matrix
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
public void nr_rotzd(double **M,double a)
{

   nr_identd(M,1,4,1,4);

   M[1][1] = cos(a);   M[1][2] = -sin(a);
   M[2][1] = sin(a);  M[2][2] = cos(a);
}

public void nr_rotzf(float **M, float a)
{

   nr_identf(M,1,4,1,4);

   M[1][1] = fcos(a);   M[1][2] = -fsin(a);
   M[2][1] = fsin(a);  M[2][2] = fcos(a);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : nr_multd, nr_multf - mult matrix
@INPUT      : A - source matrix
              mA1,mA2 - row limits of A
	      nA1,nA2 - col limits of A
	      B - source matrix
              mB1,mB2 - row limits of B
	      nB1,nB2 - col limits of B
              (matrix in numerical recipes form, allocated by calling routine)
@OUTPUT     : C = A * B
@RETURNS    : (nothing)
@DESCRIPTION: 
   Routine multiplies matrices A*B to give C. A is a mA x nA matrix and
   B is a mB x nB matrix. The result is returned in C which is mA x nB.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Tue Jun  1 12:49:21 EST 1993 (Louis Collins)
@MODIFIED   : 

---------------------------------------------------------------------------- */

/* ARGSUSED */

public void nr_multd(double **A, int mA1, int mA2, int nA1, int nA2, 
	 double **B, int mB1, int mB2, int nB1, int nB2,
	 double **C )
{
   int i, j, k;

   for ( k = mA1; k <= mA2; k++ ) {
      for ( i = nB1; i <= nB2; i++ ) {
         C[k][i] = 0.0;
         for ( j = mB1; j <= mB2; j++ ) {
            C[k][i] += A[k][j] * B[j][i];
         }
      }
   }

   return;
}

/* ARGSUSED */

public void nr_multf(float **A, int mA1, int mA2, int nA1, int nA2, 
	 float **B, int mB1, int mB2, int nB1, int nB2,
	 float **C)
{
   int i, j, k;

   for ( k = mA1; k <= mA2; k++ ) {
      for ( i = nB1; i <= nB2; i++ ) {
         C[k][i] = 0.0;
         for ( j = mB1; j <= mB2; j++ ) {
            C[k][i] += A[k][j] * B[j][i];
         }
      }
   }

   return;
}


private void print_rot_error(char *s, char * d1, int d2, int d3, int d4, int d5, int d6, int d7)
{
  print_error( "Error in file %s, line %d\n   ",d1,d2);
  print_error( s, d3,d4,d5,d6,d7);
  print_error( "\n");
  exit(EXIT_FAILURE);
}
