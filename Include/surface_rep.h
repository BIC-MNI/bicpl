#ifndef  DEF_SURFACE_REP
#define  DEF_SURFACE_REP

#include  <mni.h>

typedef  enum  { SUPERQUADRIC, SPLINE,
                 N_SURFACE_REPRESENTATIONS }
               Surface_representations;

typedef struct surface_rep_struct
{
    Surface_representations  representation;
    int                      n_descriptors;

    int                       (*get_num_parameters)( double [] );
    void                      (*get_default_descriptors)( double [] );
    BOOLEAN                   (*are_parameters_valid)( double [], double []);
    void                      (*get_parameter_influence)( int, double [],
                                                          double *, double *, 
                                                          double *, double * );
    void                      (*convert_from_representation)( 
                                   struct surface_rep_struct *,
                                   double [], double [], double [], double [] );
    void                      (*evaluate_surface_at_uv)( double, double,
                                   double [], double [],
                                   double *, double *, double *,
                                   double *, double *, double *,
                                   double *, double *, double *,
                                   double *, double *, double *,
                                   double *, double *, double * );
} surface_rep_struct;

#endif
