#ifndef  DEF_DEFORM
#define  DEF_DEFORM

#include  <def_mni.h>

typedef  enum  { TOWARDS_LOWER, TOWARDS_HIGHER, ANY_DIRECTION }
               Normal_directions;

typedef  struct
{
    Real               min_isovalue;
    Real               max_isovalue;
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
    Point                     *model_centroids;
    Vector                    *model_normals;
    Point                     *model_points;
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
    int                           degrees_continuity;
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

typedef struct
{
    int                  axis;
    Point                *save_points;
    Real                 *curvature_factors;
    Point                *equilibrium_points;
    Point                *new_equilibrium_points;
    Point                *boundary_points;
    Point                *new_boundary_points;
    Real                 temperature;
    Real                 temperature_factor;
    int                  temperature_step;
    int                  min_n_to_move;
    int                  max_n_to_move;
    Real                 max_translation;
    Real                 max_angle_rotation;
    Real                 max_scale_offset;
    int                  stop_criteria;
    int                  try;
    int                  max_tries;
    int                  max_successes;
    int                  n_successes;
    int                  n_pos_successes;
    int                  n_no_moves;
    Real                 min_delta_energy;
    Real                 max_delta_energy;
    Real                 energy;
} anneal_struct;

#endif
