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

static    VIO_Status  input_global_variable( int, global_struct [],
                                         FILE *, VIO_BOOL * );
static    VIO_STR    extract_string( VIO_STR );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_globals_file
@INPUT      : n_globals_lookup
              globals_lookup
              filename
@OUTPUT     : 
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Reads the global variable values from filename, using the
              globals lookup to find the relevant globals.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status  input_globals_file(
    int             n_globals_lookup,
    global_struct   globals_lookup[],
    VIO_STR          filename )
{
    VIO_Status  status;
    VIO_BOOL eof;
    FILE    *file;

    status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( status == VIO_OK )
    {
        eof = FALSE;

        while( !eof )
            status = input_global_variable( n_globals_lookup,
                                            globals_lookup, file, &eof );
    }

    if( status == VIO_OK )
        status = close_file( file );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_global_variable
@INPUT      : n_globals_lookup
              globals_lookup
              file
@OUTPUT     : eof  - TRUE if end of file
@RETURNS    : 
@DESCRIPTION: Inputs a global variable value from the file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  VIO_Status  input_global_variable(
    int             n_globals_lookup,
    global_struct   globals_lookup[],
    FILE            *file,
    VIO_BOOL         *eof )
{
    VIO_Status   set_status, status;
    VIO_STR   variable_name;
    VIO_STR   value;

    *eof = FALSE;

    value = NULL;

    status = input_string( file, &variable_name, (char) '=' );

    /* Ignore comments.
     */
    if (variable_name != NULL && variable_name[0] == '#')
    {
       input_newline( file );
       delete_string( variable_name );
       return status;
    }

    if( status == VIO_OK )
    {
        status = input_string( file, &value, (char) ';' );

        if( status == VIO_OK )
            set_status = set_global_variable( n_globals_lookup, globals_lookup,
                                              variable_name, value );
        else
            *eof = TRUE;
    }
    else
    {
        status = VIO_OK;
        set_status = VIO_OK;
        *eof = TRUE;
    }

    if( set_status != VIO_OK || status != VIO_OK )
    {
        print_error( "Error inputting global.\n" );
        print_error( "Variable name is %s\n", variable_name );
    }

    delete_string( variable_name );
    delete_string( value );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : lookup_global
@INPUT      : n_globals
              global_lookup
              variable_name
@OUTPUT     : ptr    - pointer to global variable
              type
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Given a name string, looks up the corresponding global variable.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  VIO_Status  lookup_global(
    int              n_globals,
    global_struct    global_lookup[],
    VIO_STR           variable_name,
    void             **ptr,
    Variable_types   *type,
    VIO_SCHAR     **set_flag )
{
    VIO_Status  status;
    VIO_STR  stripped;
    char    *global_name;
    int     i, s, len;

    status = VIO_ERROR;

    stripped = strip_outer_blanks( variable_name );

    for_less( i, 0, n_globals )
    {
        global_name = global_lookup[i].variable_name;
        len = string_length( global_name );
        s = 0;

        while( s < len && global_name[s] == ' ' )
            ++s;

        if( equal_strings( &global_name[s], stripped ) )
        {
            *ptr = global_lookup[i].ptr_to_global;
            *type = global_lookup[i].type;
            if( set_flag != NULL )
                *set_flag = &global_lookup[i].set_flag;
            status = VIO_OK;
            break;
        }
    }

    delete_string( stripped );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_global_variable
@INPUT      : n_globals_lookup
              globals_lookup
              variable_name
@OUTPUT     : value
@RETURNS    : 
@DESCRIPTION: Looks up the given variable name and places its value in string
              form in the 'value' parameter.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status  get_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    VIO_STR           variable_name,
    VIO_STR           *value )
{
    VIO_Status             status;
    void               *ptr;
    VIO_Surfprop           *surfprop;
    Variable_types     type;
    char               buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_STR             *string_ptr;
    VIO_STR             tmp_str;

    status = lookup_global( n_globals_lookup, globals_lookup, variable_name,
                            &ptr, &type, NULL );

    if( status == VIO_OK )
    {
        switch( type )
        {
        case VIO_BOOL_type:
            if( * ((VIO_BOOL *) ptr) )
                (void) strcpy( buffer, "True" );
            else
                (void) strcpy( buffer, "False" );
            break;

        case int_type:
            (void) sprintf( buffer, "%d", * (int *) ptr );
            break;

        case VIO_Real_type:
            (void) sprintf( buffer, "%g", * (VIO_Real *) ptr );
            break;

        case VIO_STR_type:
            string_ptr = (VIO_STR *) ptr;
            (void) strcpy( buffer, *string_ptr );
            break;

        case VIO_Point_type:
            (void) sprintf( buffer, "%g %g %g",
                            Point_x(* (VIO_Point *) ptr),
                            Point_y(* (VIO_Point *) ptr),
                            Point_z(* (VIO_Point *) ptr) );
            break;

        case VIO_Vector_type:
            (void) sprintf( buffer, "%g %g %g",
                            Vector_x(* (VIO_Vector *) ptr),
                            Vector_y(* (VIO_Vector *) ptr),
                            Vector_z(* (VIO_Vector *) ptr) );
            break;

        case VIO_Colour_type:
            tmp_str = convert_colour_to_string( * (VIO_Colour *) ptr );
            (void) strcpy( buffer, tmp_str );
            delete_string( tmp_str );
            break;

        case VIO_Surfprop_type:
            surfprop = (VIO_Surfprop *) ptr;
            (void) sprintf( buffer, "%g %g %g %g %g",
                            Surfprop_a(*surfprop),
                            Surfprop_d(*surfprop),
                            Surfprop_s(*surfprop),
                            Surfprop_se(*surfprop),
                            Surfprop_t(*surfprop) );
            break;

        default:
            handle_internal_error( "get_global_variable\n" );
            buffer[0] = VIO_END_OF_STRING;
            break;
        }
    }

    *value = create_string( buffer );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_global_variable
@INPUT      : n_globals_lookup
              globals_lookup
              variable_name
              value_to_set
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Looks up the given variable name, converts the value_to_set
              string to the correct type, and sets the global variable.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status  set_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    VIO_STR           variable_name,
    VIO_STR           value_to_set )
{
    VIO_Status             status;
    VIO_STR             value, *string_ptr;
    void               *ptr;
    Variable_types     type;
    int                tmp_int;
    VIO_Real               tmp_real;
    VIO_Point              tmp_point;
    VIO_Vector             tmp_vector;
    VIO_Colour             tmp_colour;
    VIO_Surfprop           tmp_surfprop;
    VIO_SCHAR       *set_flag;

    value = strip_outer_blanks( value_to_set );

    status = lookup_global( n_globals_lookup, globals_lookup, variable_name,
                            &ptr, &type, &set_flag );

    if( status == VIO_OK )
    {
        switch( type )
        {
        case VIO_BOOL_type:
            if( (value[0] == 't' || value[0] == 'T') )
            {
                * (VIO_BOOL *) ptr = TRUE;
            }
            else if( (value[0] == 'f' || value[0] == 'F') )
            {
                * (VIO_BOOL *) ptr = FALSE;
            }
            else
            {
                status = VIO_ERROR;
            }
            break;

        case int_type:
            if( sscanf( value, "%d", &tmp_int ) == 1 )
                * (int *) ptr = tmp_int;
            else
                status = VIO_ERROR;
            break;

        case VIO_Real_type:
            if( real_is_double() )
            {
                tmp_real = 0.0;
                if( sscanf( value, "%lf", (double *) &tmp_real ) == 1 )
                    * (VIO_Real *) ptr = tmp_real;
                else
                    status = VIO_ERROR;
            }
            else
            {
                if( sscanf( value, "%f", (float *) &tmp_real ) == 1 )
                    * (VIO_Real *) ptr = tmp_real;
                else
                    status = VIO_ERROR;
            }
            break;

        case VIO_STR_type:
            string_ptr = (VIO_STR *) ptr;
            if( *set_flag )
                delete_string( *string_ptr );
            *string_ptr = extract_string( value );
            *set_flag = TRUE;
            break;

        case VIO_Point_type:
            if( sscanf( value, "%f %f %f", &Point_x(tmp_point),
                        &Point_y(tmp_point), &Point_z(tmp_point) ) == 3 )
                * (VIO_Point *) ptr = tmp_point;
            else
                status = VIO_ERROR;
            break;

        case VIO_Vector_type:
            if( sscanf( value, "%f %f %f", &Vector_x(tmp_vector),
                        &Vector_y(tmp_vector), &Vector_z(tmp_vector) ) == 3 )
                * (VIO_Vector *) ptr = tmp_vector;
            else
                status = VIO_ERROR;
            break;

        case VIO_Colour_type:
            tmp_colour = convert_string_to_colour( value );

            if( status == VIO_OK )
            {
                * (VIO_Colour *) ptr = tmp_colour;
            }
            break;

        case VIO_Surfprop_type:
            if( sscanf( value, "%f %f %f %f %f",
                            &Surfprop_a(tmp_surfprop),
                            &Surfprop_d(tmp_surfprop),
                            &Surfprop_s(tmp_surfprop),
                            &Surfprop_se(tmp_surfprop),
                            &Surfprop_t(tmp_surfprop) ) == 5 )
            {
                * (VIO_Surfprop *) ptr = tmp_surfprop;
            }
            else
                status = VIO_ERROR;
            break;

        default:
            handle_internal_error( "set_global_variable\n" );
            break;
        }
    }

    delete_string( value );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_or_get_global_variable
@INPUT      : n_globals_lookup
              globals_lookup
              input_str
@OUTPUT     : variable_name
              value_string
@RETURNS    : 
@DESCRIPTION: input_str is of the form "variable_name" or 
              "variable_name = value".  In both cases the variable name and
              value of the variable are passed back.  In the second case,
              the variable is first assigned the value.  This routine is
              usually used by programs to allow the user to type in input
              strings to query or modify the values of global variables.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status  set_or_get_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    VIO_STR           input_str,
    VIO_STR           *variable_name,
    VIO_STR           *value_string )
{
    VIO_Status  status;
    VIO_STR  tmp_var_name, value_to_set;
    int     equal_index;

    status = VIO_OK;

    tmp_var_name = create_string( input_str );

    equal_index = find_character( tmp_var_name, (char) '=' );

    if( equal_index >= 0 )
    {
        value_to_set = create_string( &tmp_var_name[equal_index+1] );
        tmp_var_name[equal_index] = VIO_END_OF_STRING;
        status = set_global_variable( n_globals_lookup, globals_lookup,
                                      tmp_var_name, value_to_set );
        delete_string( value_to_set );
    }

    *variable_name = strip_outer_blanks( tmp_var_name );

    if( status == VIO_OK )
    {
        status = get_global_variable( n_globals_lookup, globals_lookup,
                                      *variable_name, value_string );
    }

    delete_string( tmp_var_name );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : extract_string
@INPUT      : str
@OUTPUT     : extracted
@RETURNS    : 
@DESCRIPTION: Extracts a string from str.  Skips leading and trailing
              white space.  String can be a quoted string or not.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  VIO_STR  extract_string(
    VIO_STR   str )
{
    int     len_str, start, end, i;
    VIO_STR  extracted;

    start = 0;

    len_str = string_length( str );

    while( start < len_str && (str[start] == ' ' || str[start] == '\t') )
    {
        ++start;
    }

    end = len_str - 1;

    while( end >= 0 && (str[end] == ' ' || str[end] == '\t') )
    {
        --end;
    }

    if( start <= end && str[start] == '"' && str[end] == '"' )
    {
        ++start;
        --end;
    }

    extracted = create_string( NULL );

    if( start <= end )
    {
        for_inclusive( i, start, end )
            concat_char_to_string( &extracted, str[i] );
    }

    return( extracted );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_global_variables
@INPUT      : n_globals_lookup
              globals_lookup
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Sep. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  delete_global_variables(
    int             n_globals_lookup,
    global_struct   globals_lookup[] )
{
    int     i;
    VIO_STR  *ptr;

    for_less( i, 0, n_globals_lookup )
    {
        if( globals_lookup[i].type == VIO_STR_type &&
            globals_lookup[i].set_flag )
        {
            ptr = (VIO_STR *) globals_lookup[i].ptr_to_global;
            delete_string( *ptr );
        }
    }
}
