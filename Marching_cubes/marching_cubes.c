#include  <def_mni.h>
#include  <def_marching_cube_data.h>
#include  <def_splines.h>

static  int  get_voxel_case_struct();

public  int  compute_isotriangles_in_voxel( corner_values, target_value,
                                            points_list_ptr )
    Real                corner_values[2][2][2];
    Real                target_value;
    voxel_point_type    *points_list_ptr[];
{
    Boolean   above_flags[2][2][2];

    above_flags[0][0][0] = (corner_values[0][0][0] >= target_value);
    above_flags[0][0][1] = (corner_values[0][0][1] >= target_value);
    above_flags[0][1][0] = (corner_values[0][1][0] >= target_value);
    above_flags[0][1][1] = (corner_values[0][1][1] >= target_value);
    above_flags[1][0][0] = (corner_values[1][0][0] >= target_value);
    above_flags[1][0][1] = (corner_values[1][0][1] >= target_value);
    above_flags[1][1][0] = (corner_values[1][1][0] >= target_value);
    above_flags[1][1][1] = (corner_values[1][1][1] >= target_value);

    return( get_voxel_case_struct( above_flags, points_list_ptr ) );
}

private  int  get_voxel_case_struct( above_flags, points_list_ptr )
    Boolean             above_flags[2][2][2];
    voxel_point_type    *points_list_ptr[];
{
    int                         case_index;
    marching_cube_data_struct   *case_struct;

    case_index =  (above_flags[0][0][0] != FALSE) +
                 ((above_flags[1][0][0] != FALSE) << 1) +
                 ((above_flags[0][1][0] != FALSE) << 2) +
                 ((above_flags[1][1][0] != FALSE) << 3) +
                 ((above_flags[0][0][1] != FALSE) << 4) +
                 ((above_flags[1][0][1] != FALSE) << 5) +
                 ((above_flags[0][1][1] != FALSE) << 6) +
                 ((above_flags[1][1][1] != FALSE) << 7);

    case_struct = &marching_cube_cases[case_index];

    *points_list_ptr = case_struct->triangle_points;

    return( case_struct->n_triangles );
}

public  void   create_point( data, slice_thickness, isovalue, x, y, z,
                             edge_intersected, normal_towards_lower,
                             max_surface_refinements,
                             max_surface_error, point, normal, pt_class )
    short               ***data;
    Real                slice_thickness[N_DIMENSIONS];
    Real                isovalue;
    int                 x, y, z;
    int                 edge_intersected;
    Boolean             normal_towards_lower;
    int                 max_surface_refinements;
    Real                max_surface_error;
    Point               *point;
    Vector              *normal;
    Point_classes       *pt_class;
{
    int       i;
    Real      u_bar[N_DIMENSIONS], dx, dy, dz;
    Real      alpha1, alpha2, val1, val2, val, alpha;
    int       corner[N_DIMENSIONS];
    Real      c000, c001, c002, c003, c010, c011, c012, c013;
    Real      c020, c021, c022, c023, c030, c031, c032, c033;
    Real      c100, c101, c102, c103, c110, c111, c112, c113;
    Real      c120, c121, c122, c123, c130, c131, c132, c133;
    Real      c200, c201, c202, c203, c210, c211, c212, c213;
    Real      c220, c221, c222, c223, c230, c231, c232, c233;
    Real      c300, c301, c302, c303, c310, c311, c312, c313;
    Real      c320, c321, c322, c323, c330, c331, c332, c333;

    corner[X] = x;
    corner[Y] = y;
    corner[Z] = z;

    val1 = isovalue - (Real) data[corner[Z]]
                                 [corner[X]]
                                 [corner[Y]];

    ++corner[edge_intersected];

    val2 = isovalue - (Real) data[corner[Z]]
                                 [corner[X]]
                                 [corner[Y]];

    if( val1 == 0.0 )
    {
        *pt_class = ON_FIRST_CORNER;
        alpha = 0.0;
    }
    else if( val2 == 0.0 )
    {
        *pt_class = ON_SECOND_CORNER;
        alpha = 1.0;
    }
    else
    {
        *pt_class = ON_EDGE;
        alpha = val1 / (val1 - val2);
    }

    c000 = (Real) data[z-1][x-1][y-1];
    c001 = (Real) data[z+0][x-1][y-1];
    c002 = (Real) data[z+1][x-1][y-1];
    c003 = (Real) data[z+2][x-1][y-1];

    c010 = (Real) data[z-1][x-1][y+0];
    c011 = (Real) data[z+0][x-1][y+0];
    c012 = (Real) data[z+1][x-1][y+0];
    c013 = (Real) data[z+2][x-1][y+0];

    c020 = (Real) data[z-1][x-1][y+1];
    c021 = (Real) data[z+0][x-1][y+1];
    c022 = (Real) data[z+1][x-1][y+1];
    c023 = (Real) data[z+2][x-1][y+1];

    c030 = (Real) data[z-1][x-1][y+2];
    c031 = (Real) data[z+0][x-1][y+2];
    c032 = (Real) data[z+1][x-1][y+2];
    c033 = (Real) data[z+2][x-1][y+2];


    c100 = (Real) data[z-1][x+0][y-1];
    c101 = (Real) data[z+0][x+0][y-1];
    c102 = (Real) data[z+1][x+0][y-1];
    c103 = (Real) data[z+2][x+0][y-1];

    c110 = (Real) data[z-1][x+0][y+0];
    c111 = (Real) data[z+0][x+0][y+0];
    c112 = (Real) data[z+1][x+0][y+0];
    c113 = (Real) data[z+2][x+0][y+0];

    c120 = (Real) data[z-1][x+0][y+1];
    c121 = (Real) data[z+0][x+0][y+1];
    c122 = (Real) data[z+1][x+0][y+1];
    c123 = (Real) data[z+2][x+0][y+1];

    c130 = (Real) data[z-1][x+0][y+2];
    c131 = (Real) data[z+0][x+0][y+2];
    c132 = (Real) data[z+1][x+0][y+2];
    c133 = (Real) data[z+2][x+0][y+2];


    c200 = (Real) data[z-1][x+1][y-1];
    c201 = (Real) data[z+0][x+1][y-1];
    c202 = (Real) data[z+1][x+1][y-1];
    c203 = (Real) data[z+2][x+1][y-1];

    c210 = (Real) data[z-1][x+1][y+0];
    c211 = (Real) data[z+0][x+1][y+0];
    c212 = (Real) data[z+1][x+1][y+0];
    c213 = (Real) data[z+2][x+1][y+0];

    c220 = (Real) data[z-1][x+1][y+1];
    c221 = (Real) data[z+0][x+1][y+1];
    c222 = (Real) data[z+1][x+1][y+1];
    c223 = (Real) data[z+2][x+1][y+1];

    c230 = (Real) data[z-1][x+1][y+2];
    c231 = (Real) data[z+0][x+1][y+2];
    c232 = (Real) data[z+1][x+1][y+2];
    c233 = (Real) data[z+2][x+1][y+2];


    c300 = (Real) data[z-1][x+2][y-1];
    c301 = (Real) data[z+0][x+2][y-1];
    c302 = (Real) data[z+1][x+2][y-1];
    c303 = (Real) data[z+2][x+2][y-1];

    c310 = (Real) data[z-1][x+2][y+0];
    c311 = (Real) data[z+0][x+2][y+0];
    c312 = (Real) data[z+1][x+2][y+0];
    c313 = (Real) data[z+2][x+2][y+0];

    c320 = (Real) data[z-1][x+2][y+1];
    c321 = (Real) data[z+0][x+2][y+1];
    c322 = (Real) data[z+1][x+2][y+1];
    c323 = (Real) data[z+2][x+2][y+1];

    c330 = (Real) data[z-1][x+2][y+2];
    c331 = (Real) data[z+0][x+2][y+2];
    c332 = (Real) data[z+1][x+2][y+2];
    c333 = (Real) data[z+2][x+2][y+2];

    u_bar[X] = 0.0;
    u_bar[Y] = 0.0;
    u_bar[Z] = 0.0;

    alpha1 = 0.0;
    alpha2 = 1.0;

    for_less( i, 0, max_surface_refinements )
    {
        u_bar[edge_intersected] = alpha;

        CUBIC_TRIVAR( c, u_bar[X], u_bar[Y], u_bar[Z], val );

        val = isovalue - val;

        if( (alpha == 0.0 && val != val1) ||
            (alpha == 1.0 && val != val2) )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement val\n" );
        }

        if( (val1 <= 0.0 && val <= 0.0) ||
            (val1 >= 0.0 && val >= 0.0) )
        {
            val1 = val;
            alpha1 = alpha;
        }
        else
        {
            val2 = val;
            alpha2 = alpha;
        }

        if( val1 != val2 )
        {
            alpha = alpha1 + val1 / (val1 - val2) * (alpha2 - alpha1);
        }
        else if( val != 0.0 )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement\n" );
        }

        if( alpha < 0.0 || alpha > 1.0 )
        {
            HANDLE_INTERNAL_ERROR( "Surface refinement alpha\n" );
        }

        if( ABS(val) < max_surface_error )
        {
            break;
        }
    }

    fill_Point( *point, (Real) x, (Real) y, (Real) z )

    Point_coord( *point, edge_intersected ) += alpha;

    u_bar[X] = FRACTION( Point_x(*point) );
    u_bar[Y] = FRACTION( Point_y(*point) );
    u_bar[Z] = FRACTION( Point_z(*point) );
  
    CUBIC_TRIVAR_DERIV( c, u_bar[X], u_bar[Y], u_bar[Z],
                        dx, dy, dz );

    fill_Vector( *normal, dx, dy, dz );

    Point_x(*point) *= slice_thickness[X];
    Point_y(*point) *= slice_thickness[Y];
    Point_z(*point) *= slice_thickness[Z];

    Vector_x(*normal) /= slice_thickness[X];
    Vector_y(*normal) /= slice_thickness[Y];
    Vector_z(*normal) /= slice_thickness[Z];

    NORMALIZE_VECTOR( *normal, *normal );

    if( normal_towards_lower )
    {
        SCALE_VECTOR( *normal, *normal, -1.0 );
    }
}
