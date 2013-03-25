/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include "bicpl_internal.h"


int bicpl_dgesvd_(char *jobu, char *jobvt, long int *m, long int *n,
            VIO_Real *a, long int *lda, VIO_Real *s, VIO_Real *u,
            long int * ldu, VIO_Real *vt, long int *ldvt, 
	    VIO_Real *work, long int *lwork, long int *info);


#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Numerical/matrix_svd.c,v 1.9 2005-08-17 22:28:59 bert Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : singular_value_decomposition
@INPUT      : m
              n
              a   - array of size [m] by [n]
@OUTPUT     : w
              v
@RETURNS    : TRUE if successful
@DESCRIPTION: Performs singular value decomposition of a matrix, 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Jul  4, 1995    David MacDonald
@MODIFIED   : 15/04/04 Vivek Singh - changed implementation to use BSD licence/Open Source
	      compatible code, specifically a routine extracted from the clapack library
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  singular_value_decomposition(
    int    m,
    int    n,
    VIO_Real   **a,
    VIO_Real   w[],
    VIO_Real   **v )
{
  
  int i,j;
  VIO_BOOL val;
  char jobu = 'O';
  char jobvt = 'A';
  long int _m = (long int) m;
  long int _n = (long int) n;

  long int lda = _m;  
  long int ldu = _m;
  long int ldvt = _n;
  long int lwork = MAX(3*MIN(_m,_n)+MAX(_m,_n),5*MIN(_m,_n));
  VIO_Real** _a;
  VIO_Real* work;
  VIO_Real** _u;
  VIO_Real** _v;
  long int info;
  VIO_Real temp;

  ALLOC(work,(int) lwork);
  ALLOC2D(_a,n,m);
  ALLOC2D(_u,n,m);
  ALLOC2D(_v,n,n);

  for (j = 0; j < n; j++) {    
    for (i = 0; i < m; i++) {
      _a[j][i] = a[i][j];
    }
  }
  

  val = bicpl_dgesvd_(&jobu, &jobvt, &_m, &_n, (VIO_Real*) *_a, &lda, (VIO_Real*) w, 
		(VIO_Real*) *_u, &ldu, (VIO_Real*) *_v, &ldvt, 
		(VIO_Real*) work, &lwork, &info);
      
  for (j = 0; j < n; j++) {    
    for (i = 0; i < m; i++) {
      a[i][j] = _a[j][i];
    }
  }

  for (j = 0; j < n; j++) {    
    for (i = 0; i < n; i++) {
      v[i][j] = _v[i][j];
    }
  }

  FREE(work);
  FREE2D(_u);
  FREE2D(_v);
  FREE2D(_a);
  
  return val;  
}
