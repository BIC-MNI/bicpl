/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_xfm.c
@DESCRIPTION: Routine to calculate a General_transform from a pair of tag
              point lists.
@METHOD     : 
@GLOBALS    : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : $Log: compute_xfm.c,v $
@MODIFIED   : Revision 1.2  1994-11-25 14:23:13  david
@MODIFIED   : check_in_all
@MODIFIED   :
 * Revision 1.1  94/11/04  14:45:47  david
 * Initial revision
 * 
 * Revision 1.4  93/09/10  11:52:58  neelin
 * Fixed 9 and 10 parameter fitting.
 * 
 * Revision 1.3  93/09/08  15:45:00  neelin
 * Added 9 and 10 parameter linear fitting.
 * 
 * Revision 1.2  93/09/01  15:35:17  neelin
 * Changed calls to lfit to lfit_vector.
 * 
 * Revision 1.1  93/09/01  15:25:25  neelin
 * Initial revision
 * 
 * Revision 1.1  93/09/01  13:21:30  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

#include <internal_volume_io.h>
#include <geom.h>
#include <recipes.h>
#include <compute_xfm.h>

/* Constants */
#define NUMBER_OF_DIMENSIONS 3

/* Function declarations */
private void compute_procrustes_transform(int npoints, 
                                          Real **tag_list1, 
                                          Real **tag_list2, 
                                          Trans_type trans_type,
                                          General_transform *transform);
private void compute_arb_param_transform(int npoints, 
                                         Real **tag_list1, 
                                         Real **tag_list2, 
                                         Trans_type trans_type,
                                         General_transform *transform);
private void   make_rots(float **xmat, float rot_x, float rot_y, float rot_z);
private void  build_homogeneous_from_parameters(float **calc_transformation,
						int ndim, 
						float *centre, 
						float *translation,
						float *scales,
						float *shears,
						float *angles);
private void compute_12param_transform(int npoints, 
                                       Real **tag_list1, 
                                       Real **tag_list2, 
                                       Trans_type trans_type,
                                       General_transform *transform);
private void lfit_func(float *point, float *afunc, int npars);
private void compute_tps_transform(int npoints, 
                                   Real **tag_list1, 
                                   Real **tag_list2, 
                                   Trans_type trans_type,
                                   General_transform *transform);


/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_transform_from_tags
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void compute_transform_from_tags(int npoints, 
                                        Real **tag_list1, 
                                        Real **tag_list2, 
                                        Trans_type trans_type,
                                        General_transform *transform)
{

   /* Check number of points for linear transformation */
   if (((trans_type == TRANS_LSQ6) ||
        (trans_type == TRANS_LSQ7) ||
        (trans_type == TRANS_LSQ9) ||
        (trans_type == TRANS_LSQ10) ||
        (trans_type == TRANS_LSQ12)) &&
       (npoints < MIN_POINTS_LINEAR) ) {
      create_linear_transform(transform, NULL);
      return;
   }

   /* Check number of points for thin-plate spline transformation */
   if ((trans_type == TRANS_TPS) && (npoints < MIN_POINTS_TPS) ) {
      create_linear_transform(transform, NULL);
      return;
   }

   switch (trans_type) {
   case TRANS_LSQ6:
   case TRANS_LSQ7:
      compute_procrustes_transform(npoints, tag_list1, tag_list2,
                                   trans_type, transform);
      break;
   case TRANS_LSQ9:
   case TRANS_LSQ10:
      compute_arb_param_transform(npoints, tag_list1, tag_list2,
                                  trans_type, transform);
      break;
   case TRANS_LSQ12:
      compute_12param_transform(npoints, tag_list1, tag_list2,
                                trans_type, transform);
      break;
   case TRANS_TPS:
      compute_tps_transform(npoints, tag_list1, tag_list2,
                            trans_type, transform);
      break;
   default:
      (void) fprintf(stderr, 
         "Invalid transform type in compute_transform_from tags\n");
      exit(EXIT_FAILURE);
   }

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_procrustes_transform
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
              Uses 6 or 7 parameter procrustes transformation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void compute_procrustes_transform(int npoints, 
                                          Real **tag_list1, 
                                          Real **tag_list2, 
                                          Trans_type trans_type,
                                          General_transform *transform)
{
   float **points1, **points2;
   float *the_centre;
   float *translation, *centre_of_rotation, **rotation, scale;
   float **calc_transformation;
   int i, j, ndim, size;
   Transform linear_transform;

   /* Create needed matrices and vectors */
   ndim = NUMBER_OF_DIMENSIONS;
   size = ndim+1;
   translation = vector(1,ndim);
   centre_of_rotation = vector(1,ndim);
   the_centre = vector(1,ndim);
   points2 = matrix(1,npoints,1,size);
   points1 = matrix(1,npoints,1,size);
   rotation = matrix(1,ndim,1,ndim);
   calc_transformation = matrix(1,size,1,size);

   /* Copy points */
   for (i=0; i<npoints; i++) {
      for (j=0; j<ndim; j++) {
         points1[i+1][j+1] = tag_list1[i][j];
         points2[i+1][j+1] = tag_list2[i][j];
      }
   }

   /* Do procrustes fit */
   procrustes(npoints, ndim, points1, points2, translation, 
              centre_of_rotation, rotation, &scale);

   /* Set scale appropriately */
   if (trans_type == TRANS_LSQ6)
      scale = 1.0;

   /* Calculate matrix in homogeneous coordinates */
   transformations_to_homogeneous(ndim, translation, centre_of_rotation,
                                  rotation, scale, calc_transformation);

   /* Save the transform */
   for (i=0; i<size; i++) {
      for (j=0; j<size; j++) {
         Transform_elem(linear_transform, i, j) =
            calc_transformation[j+1][i+1];
      }
   }
   create_linear_transform(transform, &linear_transform);

   /* Free the matrices and vectors */
   free_vector(translation,1,ndim);
   free_vector(centre_of_rotation,1,ndim);
   free_vector(the_centre,1,ndim);
   free_matrix(points2,1,npoints,1,size);
   free_matrix(points1,1,npoints,1,size);
   free_matrix(calc_transformation,1,size,1,size);

   return;
}
/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_arb_param_transform
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
              Computes a linear transform using arbitrary parameters.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Louis Collins and Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void compute_arb_param_transform(int npoints, 
                                         Real **tag_list1, 
                                         Real **tag_list2, 
                                         Trans_type trans_type,
                                         General_transform *transform)
{
  
  float 
    **points1, 
    **points2,
    *the_centre,
    *translation, 
    *centre_of_rotation, 
    **rotation, 
    scale,
    *scales,
    *shears,
    *angles,
    **calc_transformation;
  int 
    i, j, ndim, size;
  Transform 
    linear_transform;
  
  if (trans_type!=TRANS_LSQ9 && trans_type!=TRANS_LSQ10) {
    (void) fprintf(stderr, "Error in compute_arb_param_transform().\n");
    (void) fprintf(stderr, 
                   "Trans_type (=%d) requested is not LSQ9 or LSQ10.\n", 
                   trans_type);
    exit(EXIT_FAILURE);
  }
				/* Create needed matrices and vectors */
  ndim = NUMBER_OF_DIMENSIONS;
  size = ndim+1;
  translation = vector(1,ndim);
  centre_of_rotation = vector(1,ndim);
  angles = vector(1,ndim);
  shears = vector(1,ndim);
  scales = vector(1,ndim);
  the_centre = vector(1,ndim);
  points2 = matrix(1,npoints,1,size);
  points1 = matrix(1,npoints,1,size);
  rotation = matrix(1,ndim,1,ndim);
  calc_transformation = matrix(1,size,1,size);
  
				/* Copy points */
  for (i=0; i<npoints; i++) {
    for (j=0; j<ndim; j++) {
      points1[i+1][j+1] = tag_list1[i][j];
      points2[i+1][j+1] = tag_list2[i][j];
    }
  }

				/* Do procrustes fit */
  procrustes(npoints, ndim, points1, points2, translation, 
	     centre_of_rotation, rotation, &scale);
  
  if (!rotmat_to_ang(rotation, angles)) {
    (void) fprintf(stderr, "Error in compute_arb_param_transform().\n");
    (void) fprintf(stderr, "Cannot extract angles from rotation matrix.\n");
    exit(EXIT_FAILURE);
  }
  
  for (i=1; i<=ndim; ++i) {
    shears[i] = 0.0;
    scales[i] = scale;
  }

  if (!optimize_simplex(tag_list1, tag_list2,
			npoints, trans_type,
			centre_of_rotation, 
			translation,
			scales,
			shears,
			angles)) {
    (void) fprintf(stderr, "Error in compute_arb_param_transform(),\n");
    (void) fprintf(stderr, "in call to optimize_simplex().\n");
    exit(EXIT_FAILURE);
  }

  /* Calculate matrix in homogeneous coordinates */

  build_homogeneous_from_parameters(calc_transformation,
				    ndim, 
				    centre_of_rotation, 
				    translation,
				    scales,
				    shears,
				    angles);


  /* Save the transform */
  for (i=0; i<size; i++) {
    for (j=0; j<size; j++) {
      Transform_elem(linear_transform, i, j) =
	calc_transformation[j+1][i+1];
    }
  }
  create_linear_transform(transform, &linear_transform);
  
  /* Free the matrices and vectors */
  free_vector(angles,1,ndim);
  free_vector(shears,1,ndim);
  free_vector(scales,1,ndim);
  free_vector(translation,1,ndim);
  free_vector(centre_of_rotation,1,ndim);
  free_vector(the_centre,1,ndim);
  free_matrix(points2,1,npoints,1,size);
  free_matrix(points1,1,npoints,1,size);
  free_matrix(calc_transformation,1,size,1,size);
  
   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_rots
@INPUT      : rot_x, rot_y, rot_z - three rotation angles, in radians.
@OUTPUT     : xmat, a numerical recipes matrix for homogeous transformations
@RETURNS    : nothing
@DESCRIPTION: to be applied by premultiplication, ie rot*vec = newvec
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Tue Jun  8 08:44:59 EST 1993 LC
@MODIFIED   : 
---------------------------------------------------------------------------- */

private void   make_rots(float **xmat, float rot_x, float rot_y, float rot_z)
{
   float
      **TRX,
      **TRY,
      **TRZ,
      **T1,
      **T2;
   
   TRX  = matrix(1,4,1,4);
   TRY  = matrix(1,4,1,4);
   TRZ  = matrix(1,4,1,4);
   T1   = matrix(1,4,1,4);
   T2   = matrix(1,4,1,4);

   nr_rotxf(TRX, rot_x);             /* create the rotate X matrix */
   nr_rotyf(TRY, rot_y);             /* create the rotate Y matrix */
   nr_rotzf(TRZ, rot_z);             /* create the rotate Z matrix */
   
   nr_multf(TRY,1,4,1,4,  TRX,1,4,1,4,  T1); /* apply rx and ry */
   nr_multf(TRZ,1,4,1,4,  T1,1,4,1,4,  T2); /* apply rz */

   transpose(4,4,T2,xmat);
   
   free_matrix(TRX,1,4,1,4);
   free_matrix(TRY,1,4,1,4);
   free_matrix(TRZ,1,4,1,4);
   free_matrix(T1 ,1,4,1,4);
   free_matrix(T2 ,1,4,1,4);

}




/* ----------------------------- MNI Header -----------------------------------
@NAME       : build_transformation_matrix
@INPUT      : center, translations, scales, rotations
@OUTPUT     : lt  - a linear transformation matrix
@RETURNS    : nothing
@DESCRIPTION: mat = (c)(s*r)(-c)(t),
               the matrix is to be  PREmultiplied with a vector (mat*vec)
	       when used in the application
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Thu Jun  3 09:37:56 EST 1993 lc
@MODIFIED   : 
---------------------------------------------------------------------------- */

public void build_transformation_matrix(double lt[3][4], 
					double *center,
					double *translations,
					double *scales,
					double *shears,
					double *rotations)
{
  
  float
    **T,
    **SH,
    **S,
    **R,
    **C,
    **T1,
    **T2,
    **T3,
    **T4;
  int
    i,j;
  
  T      = matrix(1,4,1,4);
  SH     = matrix(1,4,1,4);
  S      = matrix(1,4,1,4);
  R      = matrix(1,4,1,4);
  C   = matrix(1,4,1,4);
  T1     = matrix(1,4,1,4);
  T2     = matrix(1,4,1,4);
  T3     = matrix(1,4,1,4);
  T4     = matrix(1,4,1,4);
  
				             /* mat = (C)(SH)(S)(R)(-C)(T) */

  nr_identf(T,1,4,1,4);	             /* make (T)(-C) */
  for_less( i, 0, 3 ) {
    T[1+i][4] = translations[i] - center[i];		
  }
                                /* make rotation matix */
  make_rots(R,
	    (float)(rotations[0]),
	    (float)(rotations[1]),
	    (float)(rotations[2])); 

                                /* make shear rotation matrix */
  make_rots(SH,
	    (float)(shears[0]),
	    (float)(shears[1]),      
	    (float)(shears[2])); 

                                /* make scaling matrix */
  nr_identf(S,1,4,1,4);	           
  for_less( i, 0, 3 ) {
    S[1+i][1+i] = scales[i];
  }

  nr_identf(C,1,4,1,4);      /* make center          */
  for_less( i, 0, 3 ) {
    C[1+i][4] = center[i];		
  }

  nr_multf(C,1,4,1,4,    SH  ,1,4,1,4, T1 );  
  nr_multf(T1,1,4,1,4,   S ,1,4,1,4, T2 );  
  nr_multf(T2,1,4,1,4,   R   ,1,4,1,4, T3 );  
  nr_multf(T3,1,4,1,4,   T,1,4,1,4, T4 );  

  for (i=1; i<=3; ++i)
    for (j=1; j<=4; ++j)
      lt[i-1][j-1] = T4[i][j];

  free_matrix(T    ,1,4,1,4);
  free_matrix(SH   ,1,4,1,4);
  free_matrix(S    ,1,4,1,4);
  free_matrix(R    ,1,4,1,4);
  free_matrix(C ,1,4,1,4);
  free_matrix(T1   ,1,4,1,4);
  free_matrix(T2   ,1,4,1,4);
  free_matrix(T3   ,1,4,1,4);
  free_matrix(T4   ,1,4,1,4);
}



private void  build_homogeneous_from_parameters(float **calc_transformation,
						int ndim, 
						float *centre, 
						float *translation,
						float *scales,
						float *shears,
						float *angles)
{
  double cent[3];
  double trans[3];
  double sc[3];
  double sh[3];
  double ang[3];
  double mat[3][4];

  int i,j;
  
  for(i=0; i<ndim; i++) {
    cent[i]  = (double)centre[i+1];
    trans[i] = (double)translation[i+1];
    sc[i]    = (double)scales[i+1];
    sh[i]    = (double)shears[i+i];
    ang[i]   = (double)angles[i+1];
  }

  build_transformation_matrix(mat,cent, trans, sc, sh, ang);

  for (i=0; i<ndim; i++) 
    for (j=0; j<=ndim; j++)
      calc_transformation[j+1][i+1] = mat[i][j];

  for (i=0; i<ndim; i++) 
    calc_transformation[i+1][ndim+1] = 0;

  calc_transformation[ndim+1][ndim+1] = 1.0;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_12param_transform
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
              Uses 12 parameter linear transformation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void compute_12param_transform(int npoints, 
                                       Real **tag_list1, 
                                       Real **tag_list2, 
                                       Trans_type trans_type,
                                       General_transform *transform)
{
   float **points2, *x1, *y1, *z1, *sig, **soln, **covar;
   float chisq;
   int *lista;
   int ndims;
   int ipoint, idim, irow, icol;
   Transform linear_transform;

   /* Check transformation type */
   if (trans_type != TRANS_LSQ12) {
      (void) fprintf(stderr, "Internal error in compute_12param_transform!\n");
      exit(EXIT_FAILURE);
   }

   /* Allocate matrices and vectors */
   ndims = NUMBER_OF_DIMENSIONS;
   points2 = matrix(1,npoints,1,ndims);
   x1 = vector(1,npoints);
   y1 = vector(1,npoints);
   z1 = vector(1,npoints);
   sig = vector(1,npoints);
   soln = matrix(1,ndims,1,ndims+1);
   lista = ivector(1,ndims+1);
   covar = matrix(1,ndims+1,1,ndims+1);

   /* Copy the data points and set up sig and lista */
   for (ipoint=1; ipoint<=npoints; ipoint++) {
      for (idim=1; idim<=ndims; idim++) {
         points2[ipoint][idim] = tag_list2[ipoint-1][idim-1];
      }
      x1[ipoint] = tag_list1[ipoint-1][0];
      y1[ipoint] = tag_list1[ipoint-1][1];
      z1[ipoint] = tag_list1[ipoint-1][2];
      sig[ipoint] = 1.0;
   }
   for (idim=1; idim<=ndims+1; idim++) {
      lista[idim] = idim;
   }

   /* Do the fit for each dimension */
   lfit_vector(points2, x1, sig, npoints, soln[1], ndims+1, lista, ndims+1, 
               covar, &chisq, lfit_func);
   lfit_vector(points2, y1, sig, npoints, soln[2], ndims+1, lista, ndims+1, 
               covar, &chisq, lfit_func);
   lfit_vector(points2, z1, sig, npoints, soln[3], ndims+1, lista, ndims+1, 
               covar, &chisq, lfit_func);

   /* Copy data into the linear_transform */
   make_identity_transform(&linear_transform);
   for (irow=0; irow<ndims; irow++) {
      for (icol=0; icol<ndims+1; icol++) {
         Transform_elem(linear_transform, irow, icol) =
            soln[irow+1][icol+1];
      }
   }

   /* Create general transform */
   create_linear_transform(transform, &linear_transform);

   /* Free matrices and vectors */
   free_matrix(points2,1,npoints,1,ndims);
   free_vector(x1,1,npoints);
   free_vector(y1,1,npoints);
   free_vector(z1,1,npoints);
   free_vector(sig,1,npoints);
   free_matrix(soln,1,ndims,1,ndims+1);
   free_ivector(lista,1,ndims+1);
   free_matrix(covar,1,ndims+1,1,ndims+1);

   return;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : lfit_func
@INPUT      : point  - point before transformation
              npars  - number of fit parameters
@OUTPUT     : afunc  - stuff needed by lfit_vector
@RETURNS    : (nothing)
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void lfit_func(float *point, float *afunc, int npars)
{
   int ipar;

   /* Check npars */
   if (npars != NUMBER_OF_DIMENSIONS + 1) {
      (void) fprintf(stderr, "Wrong number of pars in lfit_func\n");
      exit(EXIT_FAILURE);
   }

   for (ipar=1; ipar<=NUMBER_OF_DIMENSIONS; ipar++) {
      afunc[ipar] = point[ipar];
   }
   afunc[NUMBER_OF_DIMENSIONS+1] = 1.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_tps_transform
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
              Uses thin-plate spline transform.
@METHOD     : We calculate the tranformation from tag_list1 to tag_list2
              and then invert it so that resampling volumes is faster
              (doing a forward transformation, rather than a iterative
              inverse).
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
private void compute_tps_transform(int npoints, 
                                   Real **tag_list1, 
                                   Real **tag_list2, 
                                   Trans_type trans_type,
                                   General_transform *transform)
{
   float **afterdef, **beforedef, **invmly, **points, **displacements;
   int ndim, i, j;
   General_transform inv_transform;

   /* Check trans_type */
   if (trans_type != TRANS_TPS) {
      (void) fprintf(stderr, "Wrong trans type in compute_tps_transform\n");
      exit(EXIT_FAILURE);
   }

   /* Allocate matrices */
   ndim = NUMBER_OF_DIMENSIONS;
   afterdef = matrix(1,npoints,1,ndim);
   beforedef = matrix(1,npoints,1,ndim);
   invmly = matrix(1,npoints+1+ndim,1,ndim);

   /* Copy points (we calculate transformation from vol 1 to vol 2 and
      then invert it so that resampling is faster) */
   for (i=0; i<npoints; i++) {
      for (j=0; j<ndim; j++) {
         afterdef[i+1][j+1] = tag_list2[i][j];
         beforedef[i+1][j+1] = tag_list1[i][j];
      }
   }

   get_nonlinear_warp(beforedef, afterdef, invmly, npoints, ndim);

   for (i=0; i<npoints+1+ndim; i++) {
      for (j=0; j<ndim; j++) {
      }
   }

   /* Allocate and copy matrices for gen transform */
   points = matrix(0, npoints-1, 0, ndim-1);
   displacements = matrix(0, npoints+1+ndim-1, 0, ndim-1);

   for (i=0; i<npoints; i++) {
      for (j=0; j<ndim; j++) {
         points[i][j] = beforedef[i+1][j+1];
      }
   }

   for (i=0; i<npoints+1+ndim; i++) {
      for (j=0; j<ndim; j++) {
         displacements[i][j] = invmly[i+1][j+1];
      }
   }

   /* Create general transform */
   create_thin_plate_transform(&inv_transform, ndim, npoints, 
                               points, displacements);

   /* Invert general transform */
   create_inverse_general_transform(&inv_transform, transform);

   /* Free inverse transform */
   delete_general_transform(&inv_transform);

   /* Free points and displacements */
   free_matrix(points, 0, npoints-1, 0, ndim-1);
   free_matrix(displacements, 0, npoints+1+ndim-1, 0, ndim-1);

   /* Free matrices */
   free_matrix(afterdef,1,npoints,1,ndim);
   free_matrix(beforedef,1,npoints,1,ndim);
   free_matrix(invmly,1,npoints+1+ndim,1,ndim);

   return;
}
