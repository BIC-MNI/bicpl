/* ----------------------------- MNI Header -----------------------------------
@NAME       : matrix_basics.c
@DESCRIPTION: File containing routines for doing basic matrix calculations
@METHOD     : Contains routines :
                 transpose
                 matrix_multiply
@CALLS      : 
@CREATED    : January 31, 1992 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <internal_volume_io.h>
#include  <trans.h>



/* ----------------------------- MNI Header -----------------------------------
@NAME       : transpose
@INPUT      : rows    - number of rows
              cols    - number of columns
              mat     - original matrix (in numerical recipes form).
                 The dimensions of this matrix should be defined to be 
                 1 to rows and 1 to cols (when calling the numerical 
                 recipes routine matrix).
@OUTPUT     : mat_transpose  - transposed matrix (in numerical recipes form,
                 with dimensions 1 to cols and 1 to rows). Matrix 
                 mat_transpose cannot be the original matrix mat.
@RETURNS    : (nothing)
@DESCRIPTION: Transposes a matrix.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Feb. 26, 1990 (Weiqian Dai)
@MODIFIED   : January 31, 1992 (Peter Neelin)
                 - change to roughly NIL-abiding code and modified calling
                 sequence.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

public  void  transpose(
    int     rows,
    int     cols,
    Real    **mat,
    Real    **mat_transpose )
{
    int i, j;

    for_less( i, 0, rows )
        for_less( j, 0, cols )
            mat_transpose[j][i] = mat[i][j];
}


/* ----------------------------- mni Header -----------------------------------
@NAME       : raw_matrix_multiply
@INPUT      : ldim, mdim, ndim - dimensions of matrices. Matrix Amat has
                 dimensions (ldim x mdim), matrix Bmat has dimension
                 (mdim x ndim) and resultant matrix has dimension
                 (ldim x ndim).
              Amat - First matrix of multiply (in numerical recipes form).
                 Dimensions are 1 to ldim and 1 to mdim.
              Bmat - Second matrix of multiply (in numerical recipes form).
                 Dimensions are 1 to mdim and 1 to ndim.
@OUTPUT     : Cmat - Resulting matrix (in numerical recipes form).
                 Dimensions are 1 to ldim and 1 to ndim. This matrix cannot
                 be either Amat or Bmat.
@RETURNS    : (nothing)
@DESCRIPTION: Multiplies two matrices.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Feb. 26, 1990 (Weiqian Dai)
@MODIFIED   : January 31, 1992 (Peter Neelin)
                 - change to roughly NIL-abiding code and modified calling
                 sequence.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

private  void  raw_matrix_multiply(
    int     ldim,
    int     mdim,
    int     ndim, 
    Real    **Amat,
    Real    **Bmat,
    Real    **Cmat )
{
    Real   sum;
    int    i, j, k;

    /* Calculate the product */

    for_less( i, 0, ldim )
    {
        for_less( j, 0, ndim )
        {
            sum = 0.0;
            for_less( k, 0, mdim )
                sum += Amat[i][k] * Bmat[k][j];

            Cmat[i][j] = sum;
        }
    }
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : matrix_multiply
@INPUT      : ldim, mdim, ndim - dimensions of matrices. Matrix Amat has
                 dimensions (ldim x mdim), matrix Bmat has dimension
                 (mdim x ndim) and resultant matrix has dimension
                 (ldim x ndim).
              Amat - First matrix of multiply (in numerical recipes form).
                 Dimensions are 1 to ldim and 1 to mdim.
              Bmat - Second matrix of multiply (in numerical recipes form).
                 Dimensions are 1 to mdim and 1 to ndim.
@OUTPUT     : Cmat - Resulting matrix (in numerical recipes form).
                 Dimensions are 1 to ldim and 1 to ndim. This matrix can
                 be either matrix Amat or Bmat.
@RETURNS    : (nothing)
@DESCRIPTION: Multiplies two matrices.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : March 2, 1992 (Peter Neelin)
@MODIFIED   : March 2, 1992 (P.N.)
                 - Changed so that calling program can use an input matrix for
                 output.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

public  void  matrix_multiply(
    int    ldim,
    int    mdim,
    int    ndim, 
    Real   **Amat,
    Real   **Bmat,
    Real   **Cmat )
{
    int     i, j;
    Real    **Ctemp;

    /* Allocate a temporary matrix */

    ALLOC2D( Ctemp, ldim, ndim );

    /* Do the multiplication */

    raw_matrix_multiply( ldim, mdim, ndim, Amat, Bmat, Ctemp );

    /* Copy the result */

    for_less( i, 0, ldim )
        for_less( j, 0, ndim )
            Cmat[i][j] = Ctemp[i][j];

    /* Free the matrix */

    FREE2D( Ctemp );
}
