
#include  <def_mni.h>

static    Status  input_global_variable( int, global_struct [],
                                         FILE *, Boolean * );
static    void    extract_string( char [], char [] );

public  Status  input_globals_file(
    int              n_globals,
    global_struct    globals_lookup[],
    char             filename[] )
{
    Status  status;
    Boolean eof;
    FILE    *file;

    status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        eof = FALSE;

        while( !eof )
            status = input_global_variable( n_globals, globals_lookup,
                                            file, &eof );
    }

    if( status == OK )
        status = close_file( file );

    return( status );
}

private  Status  input_global_variable(
    int            n_globals,
    global_struct  global_lookup[],
    FILE           *file,
    Boolean        *eof )
{
    Status   set_status, status;
    String   variable_name;
    String   value;

    *eof = FALSE;

    status = input_string( file, variable_name, MAX_STRING_LENGTH, '=' );

    if( status == OK )
    {
        status = input_string( file, value, MAX_STRING_LENGTH, ';' );

        if( status == OK )
            set_status = set_global_variable( n_globals, global_lookup,
                                              variable_name, value );
        else
            *eof = TRUE;
    }
    else
    {
        status = OK;
        *eof = TRUE;
    }

    if( set_status != OK || status != OK )
    {
        PRINT1( "Error inputting global.\n" );
        PRINT2( "Variable name is %s\n", variable_name );
    }

    return( status );
}

private  Status  lookup_global(
    int              n_globals,
    global_struct    globals_lookup[],
    char             variable_name[],
    void             **ptr,
    Variable_types   *type )
{
    Status  status;
    String  stripped;
    char    *global_name;
    int     i, s, len;

    status = ERROR;

    strip_blanks( variable_name, stripped );

    for_less( i, 0, n_globals )
    {
        global_name = globals_lookup[i].variable_name;
        len = strlen( global_name );
        s = 0;

        while( s < len && global_name[s] == ' ' )
            ++s;

        if( strcmp( &global_name[s], stripped ) == 0 )
        {
            *ptr = globals_lookup[i].ptr_to_global;
            *type = globals_lookup[i].type;
            status = OK;
            break;
        }
    }

    return( status );
}

public  Status  get_global_variable(
    int              n_globals,
    global_struct    globals_lookup[],
    char             variable_name[],
    char             value[] )
{
    Status             status;
    void               *ptr;
    Surfprop           *surfprop;
    Variable_types     type;

    status = lookup_global( n_globals, globals_lookup, variable_name,
                            &ptr, &type );

    if( status == OK )
    {
        switch( type )
        {
        case Boolean_type:
            if( * ((Boolean *) ptr) )
                (void) strcpy( value, "True" );
            else
                (void) strcpy( value, "False" );
            break;

        case int_type:
            (void) sprintf( value, "%d", * (int *) ptr );
            break;

        case Real_type:
            (void) sprintf( value, "%g", * (Real *) ptr );
            break;

        case String_type:
            (void) strcpy( value, ptr );
            break;

        case Point_type:
            (void) sprintf( value, "%g %g %g",
                            Point_x(* (Point *) ptr),
                            Point_y(* (Point *) ptr),
                            Point_z(* (Point *) ptr) );
            break;

        case Vector_type:
            (void) sprintf( value, "%g %g %g",
                            Vector_x(* (Vector *) ptr),
                            Vector_y(* (Vector *) ptr),
                            Vector_z(* (Vector *) ptr) );
            break;

        case Colour_type:
            convert_colour_to_string( * (Colour *) ptr, value );
            break;

        case Surfprop_type:
            surfprop = (Surfprop *) ptr;
            (void) sprintf( value, "%g %g %g %g %g",
                            Surfprop_a(*surfprop),
                            Surfprop_d(*surfprop),
                            Surfprop_s(*surfprop),
                            Surfprop_se(*surfprop),
                            Surfprop_t(*surfprop) );
            break;

        default:
            HANDLE_INTERNAL_ERROR( "get_global_variable\n" );
            break;
        }
    }

    return( status );
}

public  Status  set_global_variable(
    int              n_globals,
    global_struct    globals_lookup[],
    char             variable_name[],
    char             value_to_set[] )
{
    Status             status;
    String             value;
    void               *ptr;
    Variable_types     type;
    int                tmp_int;
    Real               tmp_real;
    Point              tmp_point;
    Vector             tmp_vector;
    Colour             tmp_colour;
    Surfprop           tmp_surfprop;

    strip_blanks( value_to_set, value );

    status = lookup_global( n_globals, globals_lookup, variable_name,
                            &ptr, &type );

    if( status == OK )
    {
        switch( type )
        {
        case Boolean_type:
            if( (value[0] == 't' || value[0] == 'T') )
            {
                * (Boolean *) ptr = TRUE;
            }
            else if( (value[0] == 'f' || value[0] == 'F') )
            {
                * (Boolean *) ptr = FALSE;
            }
            else
            {
                status = ERROR;
            }
            break;

        case int_type:
            if( sscanf( value, "%d", &tmp_int ) == 1 )
                * (int *) ptr = tmp_int;
            else
                status = ERROR;
            break;

        case Real_type:
            if( real_is_double() )
            {
                if( sscanf( value, "%lf", (double *) &tmp_real ) == 1 )
                    * (Real *) ptr = tmp_real;
                else
                    status = ERROR;
            }
            else
            {
                if( sscanf( value, "%f", (float *) &tmp_real ) == 1 )
                    * (Real *) ptr = tmp_real;
                else
                    status = ERROR;
            }
            break;

        case String_type:
            extract_string( value, (char *) ptr );
            break;

        case Point_type:
            if( sscanf( value, "%f %f %f", &Point_x(tmp_point),
                        &Point_y(tmp_point), &Point_z(tmp_point) ) == 3 )
                * (Point *) ptr = tmp_point;
            else
                status = ERROR;
            break;

        case Vector_type:
            if( sscanf( value, "%f %f %f", &Vector_x(tmp_vector),
                        &Vector_y(tmp_vector), &Vector_z(tmp_vector) ) == 3 )
                * (Vector *) ptr = tmp_vector;
            else
                status = ERROR;
            break;

        case Colour_type:
            status = convert_string_to_colour( value, &tmp_colour );

            if( status == OK )
            {
                * (Colour *) ptr = tmp_colour;
            }
            break;

        case Surfprop_type:
            if( sscanf( value, "%f %f %f %f %f",
                            &Surfprop_a(tmp_surfprop),
                            &Surfprop_d(tmp_surfprop),
                            &Surfprop_s(tmp_surfprop),
                            &Surfprop_se(tmp_surfprop),
                            &Surfprop_t(tmp_surfprop) ) == 5 )
            {
                * (Surfprop *) ptr = tmp_surfprop;
            }
            else
                status = ERROR;
            break;

        default:
            HANDLE_INTERNAL_ERROR( "set_global_variable\n" );
            break;
        }
    }

    return( status );
}

public  Status  set_or_get_global_variable(
    int              n_globals,
    global_struct    globals_lookup[],
    char             input_str[],
    char             variable_name[],
    char             value_string[] )
{
    Status  status;
    String  tmp_var_name, value_to_set;
    int     equal_index;

    status = OK;

    (void) strcpy( tmp_var_name, input_str );

    equal_index = find_character( tmp_var_name, '=' );

    if( equal_index >= 0 )
    {
        (void) strcpy( value_to_set, &tmp_var_name[equal_index+1] );
        tmp_var_name[equal_index] = (char) 0;
        status = set_global_variable( n_globals, globals_lookup,
                                      tmp_var_name, value_to_set );
    }

    strip_blanks( tmp_var_name, variable_name );

    if( status == OK )
    {
        status = get_global_variable( n_globals, globals_lookup,
                                      variable_name, value_string );
    }

    return( status );
}

private  void  extract_string( str, extracted )
    char   str[];
    char   extracted[];
{
    int   len_str, start, end, i;

    start = 0;

    len_str = strlen( str );

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

    if( start <= end )
    {
        for_inclusive( i, start, end )
        {
            extracted[i-start] = str[i];
        }

        extracted[end-start+1] = (char) 0;
    }
    else
    {
        extracted[0] = (char) 0;
    }
}
