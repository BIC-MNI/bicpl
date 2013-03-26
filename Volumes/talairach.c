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

#include  "bicpl_internal.h"

#ifndef lint
static char rcsid[] = "$Header: /static-cvsroot/libraries/bicpl/Volumes/talairach.c,v 1.15 2005-08-17 22:26:19 bert Exp $";
#endif

#define  TALAIRACH_OFFSET   16.0

#define  FALSE  0
#define  TRUE   1

static int   nx = 128;
static int   ny = 128;
static int   nz = 80;

static VIO_Real  x_low  = -1.28;
static VIO_Real  x_high =  1.28;
static VIO_Real  y_low  = -1.28;
static VIO_Real  y_high =  1.28;
static VIO_Real  z_low  = -0.50;
static VIO_Real  z_high =  1.10;

static VIO_Real  x_dist_minus_1 = -67.0;
static VIO_Real  x_dist_1       =  67.0;
static VIO_Real  y_dist_minus_1 = -86.0;
static VIO_Real  y_dist_1       =  86.0;
static VIO_Real  z_dist_0       =   0.0;
static VIO_Real  z_dist_1       =  75.0;

static VIO_BOOL   initialized = FALSE;

static void  read_talairach_coordinate_system( void );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_initialized
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Checks if the Talairach system has been read from the standard
              place.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static void  check_initialized( void )
{
    if( !initialized )
    {
        read_talairach_coordinate_system();
        initialized = TRUE;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_talairach_coordinate_system
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Reads the Talairach system coordinates from the standard
              place.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static void  read_talairach_coordinate_system( void )
{
    int     i, n_lines, ch;
    FILE    *file;
    VIO_STR  name = "brain_size.dat";
    VIO_STR  filename;
    int     okay;
    int     tmp_nx, tmp_ny, tmp_nz;
    VIO_Real    tmp_x_low, tmp_x_high, tmp_y_low, tmp_y_high, tmp_z_low;
    VIO_Real    tmp_z_high, tmp_x_dist_minus_1, tmp_x_dist_1, tmp_y_dist_minus_1;
    VIO_Real    tmp_y_dist_1, tmp_z_dist_0, tmp_z_dist_1;

    okay = TRUE;

    if( file_exists(name) )
    {
        filename = create_string( name );
    }
    else
    {
        if( getenv( "PET_ROI" ) == NULL )
        {
            print_error( "You have to setenv PET_ROI.\n" );
            filename = NULL;
            okay = FALSE;
        }
        else
        {
            filename = get_absolute_filename( name, getenv("PET_ROI") );
        }
    }

    if( okay )
    {
        file = fopen( filename, "r" );

        if( file == (FILE *) 0 )
        {
            print_error( "Cannot open %s\n", filename );
            okay = FALSE;
        }
    }

    if( okay )
    {
        (void) input_int( file, &n_lines );

        for( i = 0;  i < n_lines+1;  ++i )
        {
            while( (ch = fgetc(file)) != EOF && ch != '\n' )
            {}
        }

        if( input_int( file, &tmp_nx ) != VIO_OK ||
            input_int( file, &tmp_ny ) != VIO_OK ||
            input_int( file, &tmp_nz ) != VIO_OK ||
            input_real( file, &tmp_x_low ) != VIO_OK ||
            input_real( file, &tmp_x_high ) != VIO_OK ||
            input_real( file, &tmp_y_low ) != VIO_OK ||
            input_real( file, &tmp_y_high ) != VIO_OK ||
            input_real( file, &tmp_z_low ) != VIO_OK ||
            input_real( file, &tmp_z_high ) != VIO_OK ||
            input_real( file, &tmp_x_dist_minus_1 ) != VIO_OK ||
            input_real( file, &tmp_x_dist_1 ) != VIO_OK ||
            input_real( file, &tmp_y_dist_minus_1 ) != VIO_OK ||
            input_real( file, &tmp_y_dist_1 ) != VIO_OK ||
            input_real( file, &tmp_z_dist_0 ) != VIO_OK ||
            input_real( file, &tmp_z_dist_1 ) != VIO_OK )
        {
            print_error( "Error reading info from %s\n", filename );
            okay = FALSE;
        }
    }

    if( okay )
    {
        nx = tmp_nx;
        ny = tmp_ny;
        nz = tmp_nz;

        x_low = tmp_x_low;
        x_high = tmp_x_high;
        y_low = tmp_y_low;
        y_high = tmp_y_high;
        z_low = tmp_z_low;
        z_high = tmp_z_high;

        x_dist_minus_1 = tmp_x_dist_minus_1;
        x_dist_1 = tmp_x_dist_1;
        y_dist_minus_1 = tmp_y_dist_minus_1;
        y_dist_1 = tmp_y_dist_1;
        z_dist_0 = tmp_z_dist_0;
        z_dist_1 = tmp_z_dist_1;

        (void) fclose( file );
    }

    delete_string( filename );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_voxel_to_talairach
@INPUT      : x_voxel
              y_voxel
              z_voxel
              nx_voxels
              ny_voxels
              nz_voxels
@OUTPUT     : x_tal
              y_tal
              z_tal
@RETURNS    : 
@DESCRIPTION: Converts a voxel to a Talairach voxel.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  convert_voxel_to_talairach(
    VIO_Real   x_voxel,
    VIO_Real   y_voxel,
    VIO_Real   z_voxel,
    int    nx_voxels,
    int    ny_voxels,
    int    nz_voxels,
    VIO_Real   *x_tal,
    VIO_Real   *y_tal,
    VIO_Real   *z_tal )
{
    check_initialized();

    *x_tal = x_voxel / (VIO_Real) nx_voxels * (VIO_Real) nx;
    *y_tal = y_voxel / (VIO_Real) ny_voxels * (VIO_Real) ny;
    *z_tal = z_voxel / (VIO_Real) nz_voxels * (VIO_Real) nz;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_talairach_to_voxel
@INPUT      : x_tal
              y_tal
              z_tal
              nx_voxels
              ny_voxels
              nz_voxels
@OUTPUT     : x_voxel
              y_voxel
              z_voxel
@RETURNS    : 
@DESCRIPTION: Converts a Talairach voxel to a volume voxel.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  convert_talairach_to_voxel(
    VIO_Real   x_tal,
    VIO_Real   y_tal,
    VIO_Real   z_tal,
    int    nx_voxels,
    int    ny_voxels,
    int    nz_voxels,
    VIO_Real   *x_voxel,
    VIO_Real   *y_voxel,
    VIO_Real   *z_voxel )
{
    check_initialized();

    *x_voxel = x_tal / (VIO_Real) nx * (VIO_Real) nx_voxels;
    *y_voxel = y_tal / (VIO_Real) ny * (VIO_Real) ny_voxels;
    *z_voxel = z_tal / (VIO_Real) nz * (VIO_Real) nz_voxels;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_to_mm
@INPUT      : tal
              n_planes
              limit_low
              limit_high
              brain_dist_low
              brain_dist_high
              brain_limit_low
              brain_limit_high
@OUTPUT     : 
@RETURNS    : mm distance
@DESCRIPTION: Converts to Talairach mm in one axis.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static VIO_Real  convert_to_mm(
    VIO_Real    tal,
    int     n_planes,
    VIO_Real    limit_low,
    VIO_Real    limit_high,
    VIO_Real    brain_dist_low,
    VIO_Real    brain_dist_high,
    VIO_Real    brain_limit_low,
    VIO_Real    brain_limit_high )
{
    VIO_Real   mm;
    VIO_Real   stereotactic;

    stereotactic = limit_low + (limit_high - limit_low) * tal / (VIO_Real) n_planes;

    mm = brain_dist_low + (brain_dist_high - brain_dist_low) *
      (stereotactic - brain_limit_low) / (brain_limit_high - brain_limit_low);

    return( mm );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_talairach_to_mm
@INPUT      : x_tal
              y_tal
              z_tal
@OUTPUT     : x_mm
              y_mm
              z_mm
@RETURNS    : 
@DESCRIPTION: Converts a Talairach voxel to a Talairach mm coordinate.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  convert_talairach_to_mm(
    VIO_Real   x_tal,
    VIO_Real   y_tal,
    VIO_Real   z_tal,
    VIO_Real   *x_mm,
    VIO_Real   *y_mm,
    VIO_Real   *z_mm )
{
    check_initialized();

    *x_mm = convert_to_mm( x_tal, nx, x_low, x_high, x_dist_minus_1, x_dist_1,
                           -1.0, 1.0 );
    *y_mm = convert_to_mm( y_tal, ny, y_low, y_high, y_dist_minus_1, y_dist_1,
                           -1.0, 1.0 );
    *y_mm -= TALAIRACH_OFFSET;
    *z_mm = convert_to_mm( z_tal, nz, z_low, z_high, z_dist_0, z_dist_1,
                           0.0, 1.0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_from_mm
@INPUT      : mm
              n_planes
              limit_low
              limit_high
              brain_dist_low
              brain_dist_high
              brain_limit_low
              brain_limit_high
@OUTPUT     : 
@RETURNS    : Talairach voxel
@DESCRIPTION: Converts from Talairach mm space to Talairach voxel space,
              along one axis.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static VIO_Real  convert_from_mm(
    VIO_Real    mm,
    int     n_planes,
    VIO_Real    limit_low,
    VIO_Real    limit_high,
    VIO_Real    brain_dist_low,
    VIO_Real    brain_dist_high,
    VIO_Real    brain_limit_low,
    VIO_Real    brain_limit_high )
{
    VIO_Real  stereotactic;

    stereotactic = brain_limit_low +
                   (mm - brain_dist_low) * (brain_limit_high - brain_limit_low)
                   / (brain_dist_high - brain_dist_low);

    return( (stereotactic - limit_low) * (VIO_Real) n_planes /
            (limit_high - limit_low) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_mm_to_talairach
@INPUT      : x_mm
              y_mm
              z_mm
@OUTPUT     : x_tal
              y_tal
              z_tal
@RETURNS    : 
@DESCRIPTION: Converts Talairach mm to Talairach voxels.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  convert_mm_to_talairach(
    VIO_Real   x_mm,
    VIO_Real   y_mm,
    VIO_Real   z_mm,
    VIO_Real   *x_tal,
    VIO_Real   *y_tal,
    VIO_Real   *z_tal )
{
    check_initialized();

    *x_tal = convert_from_mm( x_mm, nx, x_low, x_high, x_dist_minus_1, x_dist_1,
                              -1.0, 1.0 );
    *y_tal = convert_from_mm( y_mm + TALAIRACH_OFFSET, ny, y_low, y_high,
                              y_dist_minus_1, y_dist_1, -1.0, 1.0 );
    *z_tal = convert_from_mm( z_mm, nz, z_low, z_high, z_dist_0, z_dist_1,
                              0.0, 1.0 );
}
