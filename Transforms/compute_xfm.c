/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_xfm.c
@DESCRIPTION: Routine to calculate a General_transform from a pair of tag
              point lists.
@METHOD     : 
@GLOBALS    : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : $Log: compute_xfm.c,v $
@MODIFIED   : Revision 1.7  1995-06-23 14:24:37  david
@MODIFIED   : check_in_all
@MODIFIED   :
 * Revision 1.6  1995/05/19  21:51:21  david
 * *** empty log message ***
 *
 * Revision 1.5  1995/04/04  03:42:05  david
 * check_in_all
 *
 * Revision 1.4  1995/03/07  18:54:49  david
 * check_in_all
 *
 * Revision 1.3  95/02/27  17:20:20  david
 * *** empty log message ***
 * 
 * Revision 1.2  94/11/25  14:23:13  david
 * check_in_all
 * 
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
#include <numerical.h>
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
      print_error(
         "Invalid transform type in compute_transform_from tags\n" );
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
    print_error( "Error in compute_arb_param_transform().\n" );
    print_error( "Trans_type (=%d) requested is not LSQ9 or LSQ10.\n", 
                 trans_type );
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
    print_error( "Error in compute_arb_param_transform().\n");
    print_error( "Cannot extract angles from rotation matrix.\n");
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
    print_error( "Error in compute_arb_param_transform(),\n");
    print_error( "in call to optimize_simplex().\n");
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

public void build_transformation_matrix(Real lt[3][4], 
					Real *center,
					Real *translations,
					Real *scales,
					Real *shears,
					Real *rotations)
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
  Real cent[3];
  Real trans[3];
  Real sc[3];
  Real sh[3];
  Real ang[3];
  Real mat[3][4];

  int i,j;
  
  for(i=0; i<ndim; i++) {
    cent[i]  = (Real)centre[i+1];
    trans[i] = (Real)translation[i+1];
    sc[i]    = (Real)scales[i+1];
    sh[i]    = (Real)shears[i+i];
    ang[i]   = (Real)angles[i+1];
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
   Real       *x, *solution;
   int        ndims, d, dim;
   int        point;
   Transform  linear_transform;

   /* Check transformation type */

   if (trans_type != TRANS_LSQ12) {
      print_error( "Internal error in compute_12param_transform!\n");
      exit(EXIT_FAILURE);
   }

   ndims = NUMBER_OF_DIMENSIONS;

   make_identity_transform( &linear_transform );

   ALLOC( x, npoints );
   ALLOC( solution, ndims+1 );

   for_less( dim, 0, ndims )
   {
       /* Copy the data points */

       for_less( point, 0, npoints )
           x[point] = tag_list1[point][dim];

       /*--- find the solution */

       least_squares( npoints, ndims, tag_list2, x, solution );

       Transform_elem( linear_transform, dim, ndims ) = solution[0];
       for_less( d, 0, ndims )
           Transform_elem( linear_transform, dim, d ) = solution[1+d];
   }

   /* Create general transform */

   create_linear_transform(transform, &linear_transform);

   /* Free matrices and vectors */

   FREE( x );
   FREE( solution );
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
private  void  compute_tps_transform(int npoints, 
                                     Real **tag_list1, 
                                     Real **tag_list2, 
                                     Trans_type trans_type,
                                     General_transform *transform)
{
   Real               **displacements;
   int                ndim;
   General_transform  inv_transform;

   /* Check trans_type */

   if (trans_type != TRANS_TPS) {
      print_error( "Wrong trans type in compute_tps_transform\n");
      exit(EXIT_FAILURE);
   }

   /* Allocate matrices */

   ndim = NUMBER_OF_DIMENSIONS;

   ALLOC2D( displacements, npoints+1+ndim, ndim );

   get_nonlinear_warp( tag_list1, tag_list2, displacements, npoints,
                       ndim, ndim );

   /* ---- Create general transform */

   create_thin_plate_transform_real( &inv_transform, ndim, npoints, 
                                     tag_list1, displacements);

   /* ---- Invert general transform */

   create_inverse_general_transform( &inv_transform, transform );

   /* ---- Free inverse transform */

   delete_general_transform(&inv_transform);

   /* ---- Free displacements */

   FREE2D( displacements );
}
