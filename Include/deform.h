#ifndef  DEF_DEFORM
#define  DEF_DEFORM

#include  <def_mni.h>

typedef  enum  { TOWARDS_LOWER, TOWARDS_HIGHER, ANY_DIRECTION }
               Normal_directions;

typedef  struct
{
    Real               min_isovalue;
    Real               max_isovalue;
    Boolean            gradient_flag;
    Real               gradient_threshold_squared;
    Boolean            variable_threshold_flag;
    Real               variable_threshold;
    Normal_directions  normal_direction;
} boundary_definition_struct;

typedef  enum  { SPHERE_DATA, VOLUME_DATA }  Deform_data_types;

typedef  struct
{
    Deform_data_types   type;
    Point               sphere_centre;
    Real                sphere_x_size;
    Real                sphere_y_size;
    Real                sphere_z_size;
    volume_struct       *volume;
} deform_data_struct;

typedef  enum { POINT_SPHERE_MODEL, SPHERICAL_MODEL, AVERAGE_MODEL,
                GENERAL_MODEL }
              Deformation_model_types;

typedef  struct
{
    Deformation_model_types   model_type;
    Real                      *curvatures;
    Real                      min_curvature_offset;
    Real                      max_curvature_offset;
    Boolean                   position_constrained;
    Boolean                   max_position_offset;
    Point                     *original_positions;
} deformation_model_struct;

typedef  struct
{
    deform_data_struct            deform_data;
    Real                          model_weight;
    deformation_model_struct      deformation_model;
    Real                          fractional_step;
    Real                          max_step;
    Real                          max_search_distance;
    Real                          search_increment;
    Real                          min_size;
    boundary_definition_struct    boundary_definition;
    int                           max_iterations;
    Real                          stop_threshold;
} deform_struct;

#define  N_DEFORM_HISTOGRAM   7

typedef struct
{
    Real    average;
    Real    maximum;
    int     n_below[N_DEFORM_HISTOGRAM];
} deform_stats;

#endif
