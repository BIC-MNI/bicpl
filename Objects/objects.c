#include  <def_mni.h>

private  void  advance_object_traverse(
    object_traverse_struct  *object_traverse );

public  object_struct  *create_object(
    Object_types   object_type )
{
    object_struct   *object;

    ALLOC( object, 1 );

    object->object_type = object_type;
    object->visibility = TRUE;

    return( object );
}

public  Boolean  get_object_visibility(
    object_struct  *object )
{
    return( object->visibility );
}

public  void  set_object_visibility(
    object_struct  *object,
    Boolean        visibility )
{
    object->visibility = visibility;
}

public  lines_struct  *get_lines_ptr(
    object_struct  *object )
{
    if( object->object_type != LINES )
    {
        HANDLE_INTERNAL_ERROR( "get_lines_ptr" );
        return( (lines_struct *) NULL );
    }
    else
        return( &object->specific.lines );
}

public  marker_struct  *get_marker_ptr(
    object_struct  *object )
{
    if( object->object_type != MARKER )
    {
        HANDLE_INTERNAL_ERROR( "get_marker_ptr" );
        return( (marker_struct *) NULL );
    }
    else
        return( &object->specific.marker );
}

public  model_struct  *get_model_ptr(
    object_struct  *object )
{
    if( object->object_type != MODEL )
    {
        HANDLE_INTERNAL_ERROR( "get_model_ptr" );
        return( (model_struct *) NULL );
    }
    else
        return( &object->specific.model );
}

public  pixels_struct  *get_pixels_ptr(
    object_struct  *object )
{
    if( object->object_type != PIXELS )
    {
        HANDLE_INTERNAL_ERROR( "get_pixels_ptr" );
        return( (pixels_struct *) NULL );
    }
    else
        return( &object->specific.pixels );
}

public  polygons_struct  *get_polygons_ptr(
    object_struct  *object )
{
    if( object->object_type != POLYGONS )
    {
        HANDLE_INTERNAL_ERROR( "get_polygons_ptr" );
        return( (polygons_struct *) NULL );
    }
    else
        return( &object->specific.polygons );
}

public  quadmesh_struct  *get_quadmesh_ptr(
    object_struct  *object )
{
    if( object->object_type != QUADMESH )
    {
        HANDLE_INTERNAL_ERROR( "get_quadmesh_ptr" );
        return( (quadmesh_struct *) NULL );
    }
    else
        return( &object->specific.quadmesh );
}

public  text_struct  *get_text_ptr(
    object_struct  *object )
{
    if( object->object_type != TEXT )
    {
        HANDLE_INTERNAL_ERROR( "get_text_ptr" );
        return( (text_struct *) NULL );
    }
    else
        return( &object->specific.text );
}

/* --------------------- get points ------------------------ */

private  int  get_lines_points(
    object_struct   *object,
    Point           *points[] )
{
    *points = get_lines_ptr(object)->points;

    return( get_lines_ptr(object)->n_points );
}

private  int  get_marker_points(
    object_struct   *object,
    Point           *points[] )
{
    *points = &get_marker_ptr(object)->position;

    return( 1 );
}

private  int  get_object_zero_points(
    object_struct   *object,
    Point           *points[] )       /* ARGSUSED */
{
    return( 0 );
}

private  int  get_polygons_points(
    object_struct   *object,
    Point           *points[] )
{
    *points = get_polygons_ptr(object)->points;

    return( get_polygons_ptr(object)->n_points );
}

private  int  get_quadmesh_points(
    object_struct   *object,
    Point           *points[] )
{
    *points = get_quadmesh_ptr(object)->points;

    return( get_quadmesh_ptr(object)->m * get_quadmesh_ptr(object)->n );
}

private  int  get_text_points(
    object_struct   *object,
    Point           *points[] )
{
    *points = &get_text_ptr(object)->origin;

    return( 1 );
}

/* --------------------- get normals ------------------------ */

private  int  get_object_zero_normals(
    object_struct   *object,
    Vector          *normals[] )        /* ARGSUSED */
{
    return( 0 );
}

private  int  get_polygons_normals(
    object_struct   *object,
    Vector          *normals[] )
{
    *normals = get_polygons_ptr(object)->normals;

    return( get_polygons_ptr(object)->n_points );
}

private  int  get_quadmesh_normals(
    object_struct   *object,
    Vector          *normals[] )
{
    *normals = get_quadmesh_ptr(object)->normals;

    return( get_quadmesh_ptr(object)->m * get_quadmesh_ptr(object)->n );
}

/* --------------------- get colours ------------------------ */

private  Colour_flags  *get_lines_colours(
    object_struct   *object,
    Colour          *colours[] )
{
    *colours = get_lines_ptr(object)->colours;

    return( &get_lines_ptr(object)->colour_flag );
}

private  Colour_flags  *get_marker_colours(
    object_struct    *object,
    Colour           *colours[] )
{
    *colours = &get_marker_ptr(object)->colour;

    return( (Colour_flags *) 0 );
}

private  Colour_flags  *get_object_zero_colours(
    object_struct   *object,
    Colour          *colours[] )       /* ARGSUSED */
{
    *colours = (Colour *) NULL;

    return( (Colour_flags *) 0 );
}

private  Colour_flags  *get_polygons_colours(
    object_struct   *object,
    Colour          *colours[] )
{
    *colours = get_polygons_ptr(object)->colours;

    return( &get_polygons_ptr(object)->colour_flag );
}

private  Colour_flags  *get_quadmesh_colours(
    object_struct   *object,
    Colour          *colours[] )
{
    *colours = get_quadmesh_ptr(object)->colours;

    return( &get_quadmesh_ptr(object)->colour_flag );
}

private  Colour_flags  *get_text_colours(
    object_struct   *object,
    Colour          *colours[] )
{
    *colours = &get_text_ptr(object)->colour;

    return( (Colour_flags *) 0 );
}

/* --------------------- set colours ------------------------ */

private  void  set_lines_colours(
    object_struct   *object,
    Colour          colours[] )
{
    get_lines_ptr(object)->colours = colours;
}

private  void  set_object_no_colours(
    object_struct    *object,
    Colour           colours[] )    /* ARGSUSED */
{
}

private  void  set_polygons_colours(
    object_struct   *object,
    Colour          colours[] )
{
    get_polygons_ptr(object)->colours = colours;
}

private  void  set_quadmesh_colours(
    object_struct   *object,
    Colour          colours[] )
{
    get_quadmesh_ptr(object)->colours = colours;
}

/* --------------------- get surfprop ------------------------ */

private  Surfprop  *get_object_no_surfprop(
    object_struct   *object )                     /* ARGSUSED */
{
    return( (Surfprop *) NULL );
}

private  Surfprop  *get_polygons_surfprop(
    object_struct   *object )
{
    return( &get_polygons_ptr(object)->surfprop );
}

private  Surfprop  *get_quadmesh_surfprop(
    object_struct   *object )
{
    return( &get_quadmesh_ptr(object)->surfprop );
}

/* --------------------- get name functions ------------------------ */

private  void  get_lines_name(
    object_struct   *object,
    char            name[] )
{
    (void) sprintf( name, "Lines (%d:%d)",
                    get_lines_ptr(object)->n_items,
                    get_lines_ptr(object)->n_points );
}

private  void  get_marker_name(
    object_struct   *object,
    char            name[] )
{
    char   *label;

    if( strlen(get_marker_ptr(object)->label) == 0 )
        label = "Marker";
    else
        label = get_marker_ptr(object)->label;

    (void) sprintf( name, "%s:%d:%d: (%g %g %g)",
                    label,
                    get_marker_ptr(object)->patient_id,
                    get_marker_ptr(object)->structure_id,
                    Point_x(get_marker_ptr(object)->position),
                    Point_y(get_marker_ptr(object)->position),
                    Point_z(get_marker_ptr(object)->position) );
}

private  void  get_model_name(
    object_struct   *object,
    char            name[] )
{
    (void) sprintf( name, "Model (%s) ->", get_model_ptr(object)->filename );
}

private  void  get_pixels_name(
    object_struct   *object,
    char            name[] )
{
    (void) sprintf( name, "Pixels (%d by %d)",
                    get_pixels_ptr(object)->x_size,
                    get_pixels_ptr(object)->y_size );
}

private  void  get_polygons_name(
    object_struct   *object,
    char            name[] )
{
    (void) sprintf( name, "Polygons (%d:%d)",
                    get_polygons_ptr(object)->n_items,
                    get_polygons_ptr(object)->n_points );
}

private  void  get_quadmesh_name(
    object_struct   *object,
    char            name[] )
{
    (void) sprintf( name, "Quadmesh [%d][%d]",
                    get_quadmesh_ptr(object)->m, get_quadmesh_ptr(object)->n );
}

private  void  get_text_name(
    object_struct   *object,
    char            name[] )
{
    (void) sprintf( name, "Text (%s)", get_text_ptr(object)->string );
}

/* --------------------- delete functions ------------------------ */

private  void  delete_lines_object(
    object_struct   *object )
{
    delete_lines( get_lines_ptr(object) );
}

private  void  delete_marker_object(
    object_struct   *object )    /* ARGSUSED */
{
}

private  void  delete_model_object(
    object_struct   *object )
{
    delete_model( get_model_ptr(object) );
}

private  void  delete_pixels_object(
    object_struct   *object )
{
    delete_pixels( get_pixels_ptr(object) );
}

private  void  delete_polygons_object(
    object_struct   *object )
{
    delete_polygons( get_polygons_ptr(object) );
}

private  void  delete_quadmesh_object(
    object_struct   *object )       /* ARGSUSED */
{
    delete_quadmesh( get_quadmesh_ptr(object) );
}

private  void  delete_text_object(
    object_struct   *object )       /* ARGSUSED */
{
}

/* -------------------- function lookup table ------------------- */

typedef  struct
{
    int           (*get_points_function)( object_struct *, Point *[] );
    int           (*get_normals_function)( object_struct *, Vector *[] );
    Colour_flags  *(*get_colours_function)( object_struct *, Colour *[] );
    void          (*set_colours_function)( object_struct *, Colour [] );
    Surfprop      *(*get_surfprop_function)( object_struct * );
    void          (*get_name_function)( object_struct *, char [] );
    void          (*delete_function)( object_struct * );
}
object_functions_list;

static  object_functions_list   object_functions[N_OBJECT_TYPES] =
{
    {
        get_lines_points,
        get_object_zero_normals,
        get_lines_colours,
        set_lines_colours,
        get_object_no_surfprop,
        get_lines_name,
        delete_lines_object
    },                              /* LINES */
    {
        get_marker_points,
        get_object_zero_normals,
        get_marker_colours,
        set_object_no_colours,
        get_object_no_surfprop,
        get_marker_name,
        delete_marker_object
    },                              /* MARKER */
    {
        get_object_zero_points,
        get_object_zero_normals,
        get_object_zero_colours,
        set_object_no_colours,
        get_object_no_surfprop,
        get_model_name,
        delete_model_object
    },                              /* MODEL */
    {
        get_object_zero_points,
        get_object_zero_normals,
        get_object_zero_colours,
        set_object_no_colours,
        get_object_no_surfprop,
        get_pixels_name,
        delete_pixels_object
    },                              /* PIXELS */
    {
        get_polygons_points,
        get_polygons_normals,
        get_polygons_colours,
        set_polygons_colours,
        get_polygons_surfprop,
        get_polygons_name,
        delete_polygons_object
    },                              /* POLYGONS */
    {
        get_quadmesh_points,
        get_quadmesh_normals,
        get_quadmesh_colours,
        set_quadmesh_colours,
        get_quadmesh_surfprop,
        get_quadmesh_name,
        delete_quadmesh_object
    },                              /* QUADMESH */
    {
        get_text_points,
        get_object_zero_normals,
        get_text_colours,
        set_object_no_colours,
        get_object_no_surfprop,
        get_text_name,
        delete_text_object
    }                               /* TEXT */
};

public  void  delete_object(
    object_struct  *object )
{
    object_functions[object->object_type].delete_function( object );
    FREE( object );
}

public  int  get_object_points(
    object_struct   *object,
    Point           *points[] )
{
    return( object_functions[object->object_type].get_points_function
                   ( object, points ) );
}

public  int  get_object_normals(
    object_struct   *object,
    Vector          *normals[] )
{
    return( object_functions[object->object_type].get_normals_function
                   ( object, normals ) );
}

public  Colour_flags  *get_object_colours(
    object_struct   *object,
    Colour          *colours[] )
{
    return( object_functions[object->object_type].get_colours_function
                   ( object, colours ) );
}

public  void  set_object_colours(
    object_struct   *object,
    Colour          colours[] )
{
    object_functions[object->object_type].set_colours_function(object,colours);
}

public  Surfprop  *get_object_surfprop(
    object_struct   *object )
{
    return( object_functions[object->object_type].get_surfprop_function
                   ( object ) );
}

public  void  get_object_name(
    object_struct  *object,
    char           name[] )
{
    object_functions[object->object_type].get_name_function( object, name );
}

public  void  initialize_object_traverse(
    object_traverse_struct  *object_traverse,
    int                     n_objects,
    object_struct           *object_list[] )
{
    object_stack_struct   push_entry;

    INITIALIZE_STACK( object_traverse->stack );

    push_entry.index = 0;
    push_entry.n_objects = n_objects;
    push_entry.object_list = object_list;

    PUSH_STACK( object_traverse->stack, push_entry );
}

public  Boolean  get_next_object_traverse(
    object_traverse_struct  *object_traverse,
    object_struct           **object )
{
    Boolean               object_found;
    object_stack_struct   *top_entry;

    if( !IS_STACK_EMPTY(object_traverse->stack) )
    {
        top_entry = &TOP_OF_STACK( object_traverse->stack );
        *object = top_entry->object_list[top_entry->index];

        advance_object_traverse( object_traverse );
        object_found = TRUE;
    }
    else
    {
        terminate_object_traverse( object_traverse );
        object_found = FALSE;
    }

    return( object_found );
}

private  void  advance_object_traverse(
    object_traverse_struct  *object_traverse )
{
    object_stack_struct   *top_entry, push_entry;
    object_struct         *object;
    model_struct          *model;

    top_entry = &TOP_OF_STACK( object_traverse->stack );
    object = top_entry->object_list[top_entry->index];
    ++top_entry->index;

    if( object->object_type == MODEL )
    {
        model = get_model_ptr( object );
        push_entry.index = 0;
        push_entry.n_objects = model->n_objects;
        push_entry.object_list = model->objects;

        PUSH_STACK( object_traverse->stack, push_entry );
    }

    while( TOP_OF_STACK(object_traverse->stack).index >=
           TOP_OF_STACK(object_traverse->stack).n_objects &&
           !IS_STACK_EMPTY(object_traverse->stack) )
    {
        POP_STACK( object_traverse->stack, push_entry );
    }
}

public  void  terminate_object_traverse(
    object_traverse_struct   *object_traverse )
{
    DELETE_STACK( object_traverse->stack );
}

public  Boolean  get_range_of_object(
    object_struct   *object,
    Boolean         visible_ones_only,
    Point           *min_corner,
    Point           *max_corner )
{
    Boolean                 found_flag;
    Point                   min_obj, max_obj, *points;
    int                     n_points;
    object_struct           *current_object;
    object_traverse_struct  object_traverse;

    found_flag = FALSE;

    initialize_object_traverse( &object_traverse, 1, &object );

    while( get_next_object_traverse( &object_traverse, &current_object ) )
    {
        if( !visible_ones_only || current_object->visibility )
        {
            n_points = get_object_points( current_object, &points );

            if( n_points > 0 )
            {
                get_range_points( n_points, points, &min_obj, &max_obj );

                if( !found_flag )
                {
                    found_flag = TRUE;
                    *min_corner = min_obj;
                    *max_corner = max_obj;
                }
                else
                {
                    expand_min_and_max_points( min_corner, max_corner,
                                               &min_obj, &max_obj );
                }
            }
        }
    }

    return( found_flag );
}

public  void  reverse_object_normals(
    object_struct   *object )
{
    int                     n_normals;
    Vector                  *normals;
    object_struct           *current_object;
    object_traverse_struct  object_traverse;

    initialize_object_traverse( &object_traverse, 1, &object );

    while( get_next_object_traverse( &object_traverse, &current_object ) )
    {
        n_normals = get_object_normals( current_object, &normals );
        reverse_vectors( n_normals, normals );
    }
}

/* ------------------ io -------------------- */

public  Status  input_object(
    char           directory[],
    FILE           *file,
    File_formats   *format,
    object_struct  **object,
    Boolean        *eof )
{
    Status         status;
    File_formats   sub_format;
    Object_types   type;
    String         abs_filename;

    status = input_object_type( file, &type, format, eof );

    if( status == OK && !(*eof) )
    {
        *object = create_object( type );

        switch( type )
        {
        case LINES:
            status = io_lines( file, READ_FILE, *format,
                               get_lines_ptr(*object) );
            break;

        case MARKER:
            status = io_marker( file, READ_FILE, *format,
                                get_marker_ptr(*object) );
            break;

        case MODEL:
            status = io_model( file, READ_FILE, *format,
                               get_model_ptr(*object) );

            get_absolute_filename( get_model_ptr(*object)->filename,
                                   directory, abs_filename );

            if( status == OK )
            {
                status = input_graphics_file( abs_filename, &sub_format,
                                          &get_model_ptr(*object)->n_objects,
                                          &get_model_ptr(*object)->objects );
            }
            break;

        case PIXELS:
            status = io_pixels( file, READ_FILE, *format,
                                get_pixels_ptr(*object) );
            break;

        case POLYGONS:
            get_polygons_ptr( *object )->visibilities = (Smallest_int *) NULL;
            get_polygons_ptr( *object )->neighbours = (int *) NULL;
            get_polygons_ptr( *object )->bintree = (bintree_struct *) NULL;

            status = io_polygons( file, READ_FILE, *format,
                                   get_polygons_ptr( *object ) );

            break;

        case QUADMESH:
            status = io_quadmesh( file, READ_FILE, *format,
                                  get_quadmesh_ptr( *object ) );
            break;

        case TEXT:
            status = io_text( file, READ_FILE, *format,
                              get_text_ptr( *object ) );
            break;

        default:
            print( "Unrecognized object type %d\n", type );
            status = ERROR;
        }
    }

    return( status );
}

/* ------------------ output -------------------- */

public  Status  output_object(
    FILE           *file,
    File_formats   format,
    object_struct  *object )
{
    Status         status;

    switch( object->object_type )
    {
    case LINES:
        status = io_lines( file, WRITE_FILE, format, get_lines_ptr(object) );
        break;

    case MARKER:
        status = io_marker( file, WRITE_FILE, format, get_marker_ptr(object) );
        break;

    case MODEL:
        status = io_model( file, WRITE_FILE, format, get_model_ptr(object) );
        break;

    case PIXELS:
        status = io_pixels( file, WRITE_FILE, format, get_pixels_ptr(object) );
        break;

    case POLYGONS:
        status = io_polygons( file, WRITE_FILE, format,
                              get_polygons_ptr(object) );
        break;

    case QUADMESH:
        status = io_quadmesh( file, WRITE_FILE, format,
                              get_quadmesh_ptr(object) );
        break;

    case TEXT:
        status = io_text( file, WRITE_FILE, format, get_text_ptr(object) );
        break;

    default:
        status = ERROR;
    }

    return( status );
}

public  Boolean  get_object_colour(
    object_struct  *object,
    Colour         *colour )
{
    Boolean       has_single_colour;
    Colour_flags  *colour_flag;
    Colour        *colours;

    colour_flag = get_object_colours( object, &colours );

    if( (colour_flag == (Colour_flags *) NULL ||
         *colour_flag == ONE_COLOUR) && colours != (Colour *) NULL )
    {
        has_single_colour = TRUE;
        *colour = colours[0];
    }
    else
    {
        has_single_colour = FALSE;
    }

    return( has_single_colour );
}

public  void  set_object_colour(
    object_struct  *object,
    Colour         col )
{
    Colour_flags  *colour_flag;
    Colour        *colours;

    colour_flag = get_object_colours( object, &colours );

    if( colour_flag != (Colour_flags *) NULL && *colour_flag != ONE_COLOUR )
    {
        *colour_flag = ONE_COLOUR;

        if( colours != (Colour *) NULL )
            FREE( colours );

        ALLOC( colours, 1 );
        set_object_colours( object, colours );
    }

    if( colours != (Colour *) NULL )
        colours[0] = col;
}

public  void  set_object_surfprop(
    object_struct  *object,
    Surfprop       *spr )
{
    Surfprop   *object_spr;

    object_spr = get_object_surfprop( object );

    if( object_spr != (Surfprop *) NULL )
        *object_spr = *spr;
}

public  void  free_colours(
    Colour_flags   colour_flag,
    Colour         colours[],
    int            n_points,
    int            n_items )
{
    Boolean  should_free;

    should_free = FALSE;

    switch( colour_flag )
    {
    case  ONE_COLOUR:
        should_free = TRUE;
        break;

    case  PER_ITEM_COLOURS:
        should_free = (n_items > 0);
        break;

    case  PER_VERTEX_COLOURS:
        should_free = (n_points > 0);
        break;
    }

    if( should_free )
        FREE( colours );
}
