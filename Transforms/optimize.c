#include <internal_volume_io.h>
#include <geom.h>
#include <recipes.h>
#include <limits.h>


/* global parameters */

double   ftol = 0.001;
double   simplex_size = 3;

/* globals for this file, to communication between optimization and obj function */
static int    Gndim, Gnpoints;
static float *Gcenter;
static Real  **Gpt1;
static Real  **Gpt2;

/* internal prototypes */

private float lsq_objective(double lt[3][4], 
			     double **pts1, double **pts2, 
			     int npoints, int ndim);

extern  void     amoeba();

private float fit_function(float *params);

/* external prototype */


/* ----------------------------- MNI Header -----------------------------------
@NAME       : lsq_objective
@INPUT      :  lt - a transformation matrix (with translation on the last column)
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: 	apply matrix lt to pts 2 and measure distance from pts1.              
@GLOBALS    : 
@CALLS      : 
@CREATED    : 
@MODIFIED   : 

---------------------------------------------------------------------------- */
private  float lsq_objective(double lt[3][4], 
			     Real **pts1, Real **pts2, 
			     int npoints, int ndim)
{

  int i,j,k;
  float sum, error2;
  double newpt[4],t;
  double max[3], min[3], minimum_error, range;
  
  sum = 0.0;

  for (j=0; j<ndim; j++) {
     max[j] = -DBL_MAX;
     min[j] = DBL_MAX;
  }

  for (i=0; i<npoints; ++i) {	/* for all of the points */

    for (j=0; j< ndim; ++j) {	/* multiply lt*pts1  */
      t = 0.0;
      for (k=0; k<ndim; k++) {
         t += lt[j][k]*pts2[i][k];
      }
      newpt[j] = t + lt[j][ndim];
    }

    error2 = 0.0;		/* compare it to pts2, summing the squared error */
    
    for (j=0; j< ndim; ++j) {
      if (pts1[i][j]>max[j]) max[j] = pts1[i][j];
      if (pts1[i][j]<min[j]) min[j] = pts1[i][j];
      error2 += (newpt[j]-pts1[i][j])*(newpt[j]-pts1[i][j]);
   }

    sum += error2;
    
  }

  /* Add in a constant error so that amoeba doesn't blow up when there is no
     error on the points. This is equivalent to having an rms error of 
     1/FUDGE_FACTOR of the length of the long diagonal of the tag point 
     bounding box. A FUDGE_FACTOR of 250 was chosen to given a typical 
     rms error of around 1mm */
#define FUDGE_FACTOR 250.0
  minimum_error = 0.0;
  for (j=0; j<ndim; j++) {
     range = (max[j] - min[j])/FUDGE_FACTOR;
     minimum_error += range*range;
  }
  if (minimum_error==0.0) minimum_error = 1.0;
  sum += npoints * minimum_error;


  return(sum);

}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : fit_function
@INPUT      : params - a variable length array of floats
@OUTPUT     :               
@RETURNS    : a float value of the user requested objective function,
              measuring the similarity between two data sets.
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */

private float fit_function(float *params) 
{

  double mat[3][4];
  int i,dim;
  float r;

  double trans[3];
  double cent[3];
  double rots[3];
  double scale[3];
  double shear[3];

  for_less( i, 0, 3 ) {
    trans[i] = params[i+1];	/* set translations */
    rots[i]  = params[i+4];	/* set rotations */
    scale[i] = params[i+7];	/* set scales */
  }

  for_less( i, 0, 3 )		/* set shears */
    shear[i] = 0.0;

  if (Gndim==10) 
    shear[1] = params[10]; 
    
  for_less( i, 0, 3 )
    cent[i] = Gcenter[i+1];	/* global CENTER used here */


  build_transformation_matrix(mat, cent, trans, scale, shear, rots);

				/* call the needed objective function */
  dim = 3;
  r = lsq_objective(mat,Gpt1,Gpt2,Gnpoints,dim);

  return(r);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : optimize_simplex
                get the parameters necessary to map point set 1 to point set 2
		using the simplex optimization algorithm.
@INPUT      : pts1,pts2 - two sets of 3D coordinates
              npoints - number of points in the two lists
	      trans_type = TRANS_LSQ9 or TRANS_LSQ10
	      center - array of center of rotation/scaling
@OUTPUT     : translation
              scales
	      shears
	      rotations
@RETURNS    : TRUE if ok, FALSE if error.
@DESCRIPTION: 
@METHOD     : uses the ameoba simplex algorithm from numerical recipes. 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 2, 1993 Louis
@MODIFIED   : 
---------------------------------------------------------------------------- */
public BOOLEAN optimize_simplex(Real **pts1, Real **pts2, 
				int npoints, 
				Trans_type trans_type,
				float *center,
				float *translations,
				float *scales,
				float *shears,
				float *rotations)
{
  BOOLEAN 
    stat;
  float 
    local_ftol,
    **p, *y;
  int 
    i,j, 
    ndim, nfunk;

  
  stat = TRUE;
  local_ftol = ftol;

  switch (trans_type) {		/* check to see if trans_type is OK */
  case TRANS_LSQ9: 
    ndim = 9;
    break;
  case TRANS_LSQ10: 
    ndim = 10;
    break;
  default:
    print_error( "Unknown type of transformation requested (%d)\n",
		 trans_type);
    print_error( "Error in line %d, file %s\n",__LINE__, __FILE__);
    stat = FALSE;
  }


  if (stat) {

    Gpt1 = pts1;		/* set up globals to communicate to obj */
    Gpt2 = pts2;		/* function. */
    Gnpoints = npoints;
    Gcenter = center;
    Gndim   = ndim;

    p = matrix(1,ndim+1,1,ndim); /* simplex */
    y = vector(1,ndim+1);        /* value of correlation at simplex vertices */
    
    p[1][1]=translations[1];
    p[1][2]=translations[2];
    p[1][3]=translations[3];
    
    p[1][4]=rotations[1]; 
    p[1][5]=rotations[2]; 
    p[1][6]=rotations[3];
    
    p[1][7]=scales[1];
    p[1][8]=scales[2];
    p[1][9]=scales[3];
    
    if (ndim==10){
      	p[1][10] = shears[1];  /* one rotation about the x-axis (LR-axis) */
    }


    for (i=2; i<=(ndim+1); ++i)	/* copy initial guess to all points of simplex */
      for (j=1; j<=ndim; ++j)
	p[i][j] = p[1][j];
    
    p[2][1]=p[1][1]+simplex_size;		/* set up all vertices of simplex */
    p[3][2]=p[1][2]+simplex_size;
    p[4][3]=p[1][3]+simplex_size;
    
    p[5][4]=p[1][4] + (simplex_size*DEG_TO_RAD);
    p[6][5]=p[1][5] + (simplex_size*DEG_TO_RAD);
    p[7][6]=p[1][6] + (simplex_size*DEG_TO_RAD);
    
    p[8][7]=p[1][7] + simplex_size/30;	
    p[9][8]=p[1][8] + simplex_size/30;
    p[10][9]=p[1][9]+ simplex_size/30;


    if (ndim==10) 
      p[11][10]=p[1][10] + simplex_size*DEG_TO_RAD;


    for (i=1; i<=(ndim+1); ++i)	{   /* set up value of correlation at all points of simplex */
      
      y[i] = fit_function(p[i]);

#ifdef PRINT_DEBUG
      print("corr = %6.4f",y[i]);
      for (j=1; j<=3; ++j)  {
         print(" %7.2f",p[i][j]);
      }
      for (j=4; j<=6; ++j)  {
         print(" %7.3f",p[i][j]*RAD_TO_DEG);
      }
      for (j=7; j<=ndim; ++j)  {
         print(" %6.4f",p[i][j]);
      }
      print("\n");
#endif
      
    }
	
    amoeba(p,y,ndim,local_ftol,fit_function,&nfunk);
    
#ifdef PRINT_DEBUG
    print("after %d iterations\n",nfunk);
    
    for (i=1; i<=(ndim+1); ++i)	{   /* print out value of correlation at all points of simplex */
      if (i==1) {
         print("end corr = %f",y[i]);
	for (j=1; j<=3; ++j)  {
      print(" %f",p[i][j]);
	}
	for (j=4; j<=6; ++j)  {
      print(" %f",p[i][j]*RAD_TO_DEG);
	}
	for (j=7; j<=ndim; ++j)  {
      print(" %f",p[i][j]);
	}
         print("\n");
      }
    }   
#endif
    
				/* copy optimized results */
    for_less ( i, 0, 3 ) {
      translations[i+1] = p[1][i+1];
      rotations[i+1]    = p[1][i+4];
      scales[i+1]       = p[1][i+7];
    }
    if (ndim==10)
      shears[1] = p[1][10];

    free_matrix(p,1,ndim+1,1,ndim); /* simplex */
    free_vector(y,1,ndim+1);        /* value of correlation at simplex vertices */

  }

  return( stat );
}

