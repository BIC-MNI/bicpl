
#include  <def_string.h>
#include  <def_files.h>
#include  <def_mni.h>
#include  <def_objects.h>
#include  <def_alloc.h>

Status   io_object_type();
static    Status   io_points();
static    Status   io_vectors();
Status   io_surfprop();

public  Status  io_lines( file, io_flag, format, lines )
    FILE                *file;
    IO_types            io_flag;
    File_formats        format;
    lines_struct        *lines;
{
    Status   status;
    Status   io_colours();
    Status   io_newline();
    Status   io_int();
    Status   io_ints();

    status = OK;

    if( io_flag == READ_FILE ||
        (lines->n_points > 0 && lines->n_items > 0) )
    {
        status = io_object_type( file, io_flag, format, LINES );

        if( status == OK )
            status = io_int( file, io_flag, format, &lines->line_thickness );

        if( status == OK )
            status = io_int( file, io_flag, format, &lines->n_points );

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_points( file, io_flag, format,
                                lines->n_points, &lines->points );
        }

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
            status = io_int( file, io_flag, format, &lines->n_items );

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_colours( file, io_flag, format, &lines->colour_flag,
                                 lines->n_items, lines->n_points,
                                 &lines->colours );
        }

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_ints( file, io_flag, format,
                              lines->n_items, &lines->end_indices );
        }

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_ints( file, io_flag, format,
                              NUMBER_INDICES(*lines),
                              &lines->indices );
        }
    }

    return( status );
}

public  Status  io_marker( file, io_flag, format, marker )
    FILE                *file;
    IO_types            io_flag;
    File_formats        format;
    marker_struct       *marker;
{
    Status   status;
    Status   io_object_type();
    Status   io_real();
    Status   io_colour();
    Status   io_newline();
    Status   io_point();
    Status   io_quoted_string();

    status = io_object_type( file, io_flag, format, MARKER );

    if( status == OK )
        status = io_int( file, io_flag, format, (int *) &marker->type );

    if( status == OK )
        status = io_real( file, io_flag, format, &marker->size );

    if( status == OK )
        status = io_colour( file, io_flag, format, &marker->colour );

    if( status == OK )
        status = io_point( file, io_flag, format, &marker->position );

    if( status == OK )
        status = io_int( file, io_flag, format, &marker->structure_id );

    if( status == OK )
        status = io_int( file, io_flag, format, &marker->patient_id );

    if( status == OK )
        status = io_quoted_string( file, io_flag, format, marker->label,
                                   MAX_STRING_LENGTH );

    if( status == OK )
        status = io_newline( file, io_flag, format );

    return( status );
}

public  Status  io_model( file, io_flag, format, model )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    model_struct    *model;
{
    Status   status;
    Status   io_object_type();
    Status   io_quoted_string();
    Status   io_newline();

    status = io_object_type( file, io_flag, format, MODEL );

    if( status == OK )
        status = io_quoted_string( file, io_flag, format, model->filename,
                                   MAX_STRING_LENGTH );

    if( status == OK )
        status = io_newline( file, io_flag, format );

    return( status );
}

public  Status  io_pixels( file, io_flag, format, pixels )
    FILE                *file;
    IO_types            io_flag;
    File_formats        format;
    pixels_struct       *pixels;
{
    Status   status;
    int      n_pixels, x_size, y_size;
    Status   io_object_type();
    Status   io_newline();
    Status   io_int();
    Status   io_unsigned_chars();
    Status   io_pixel_colours();

    status = OK;

    if( io_flag == READ_FILE ||
        (pixels->x_max >= pixels->x_min && pixels->y_max >= pixels->y_min) )
    {
        status = io_object_type( file, io_flag, format, PIXELS );

        if( status == OK )
            status = io_int( file, io_flag, format, (int *)&pixels->pixel_type);

        if( status == OK && io_flag == WRITE_FILE )
        {
            x_size = pixels->x_max - pixels->x_min + 1;
            y_size = pixels->y_max - pixels->y_min + 1;
        }

        if( status == OK )
            status = io_int( file, io_flag, format, &x_size );

        if( status == OK )
            status = io_int( file, io_flag, format, &y_size );

        if( status == OK && io_flag == READ_FILE )
        {
            pixels->x_min = 0;
            pixels->x_max = x_size-1;
            pixels->y_min = 0;
            pixels->y_max = y_size-1;
        }

        if( status == OK )
        {
            n_pixels = x_size * y_size;

            if( pixels->pixel_type == Pixel8 )
            {
                status = io_unsigned_chars( file, io_flag, format, n_pixels,
                                            &pixels->data.pixels8 );
            }
            else
            {
                status = io_pixel_colours( file, io_flag, format, n_pixels,
                                           &pixels->data.pixels24 );
            }
        }
    }

    return( status );
}

public  Status  io_polygons( file, io_flag, format, polygons )
    FILE                *file;
    IO_types            io_flag;
    File_formats        format;
    polygons_struct     *polygons;
{
    Status   status;
    Status   io_object_type();
    Status   io_colours();
    Status   io_surfprop();
    Status   io_newline();
    Status   io_int();
    Status   io_ints();
    Status   io_points();
    Status   io_vectors();

    status = OK;

    if( io_flag == READ_FILE ||
        (polygons->n_points > 0 && polygons->n_items > 0) )
    {
        status = io_object_type( file, io_flag, format, POLYGONS );

        if( status == OK )
            status = io_surfprop( file, io_flag, format, &polygons->surfprop );

        if( status == OK )
            status = io_int( file, io_flag, format, &polygons->n_points );

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_points( file, io_flag, format,
                                polygons->n_points, &polygons->points );
        }

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_vectors( file, io_flag, format,
                                 polygons->n_points, &polygons->normals );
        }

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
            status = io_int( file, io_flag, format, &polygons->n_items );

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_colours( file, io_flag, format, &polygons->colour_flag,
                                 polygons->n_items, polygons->n_points,
                                 &polygons->colours );
        }

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_ints( file, io_flag, format,
                              polygons->n_items,
                              &polygons->end_indices );
        }

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_ints( file, io_flag, format,
                              NUMBER_INDICES(*polygons),
                              &polygons->indices );
        }
    }

    return( status );
}

public  Status  io_quadmesh( file, io_flag, format, quadmesh )
    FILE                *file;
    IO_types            io_flag;
    File_formats        format;
    quadmesh_struct     *quadmesh;
{
    Status   status;
    Status   io_object_type();
    Status   io_colours();
    Status   io_boolean();
    Status   io_surfprop();
    Status   io_newline();
    Status   io_int();
    Status   io_points();
    Status   io_vectors();

    status = OK;

    if( io_flag == READ_FILE ||
        (quadmesh->m > 1 && quadmesh->n > 1) )
    {
        status = io_object_type( file, io_flag, format, QUADMESH );

        if( status == OK )
            status = io_surfprop( file, io_flag, format, &quadmesh->surfprop );

        if( status == OK )
            status = io_int( file, io_flag, format, &quadmesh->m );

        if( status == OK )
            status = io_int( file, io_flag, format, &quadmesh->n );

        if( status == OK )
            status = io_boolean( file, io_flag, format, &quadmesh->m_closed );

        if( status == OK )
            status = io_boolean( file, io_flag, format, &quadmesh->n_closed );

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
            status = io_colours( file, io_flag, format, &quadmesh->colour_flag,
                                 (quadmesh->m-1) * (quadmesh->n-1),
                                 quadmesh->m * quadmesh->n,
                                 &quadmesh->colours );

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_points( file, io_flag, format,
                                quadmesh->m * quadmesh->n, &quadmesh->points );
        }

        if( status == OK )
            status = io_newline( file, io_flag, format );

        if( status == OK )
        {
            status = io_vectors( file, io_flag, format,
                                quadmesh->m * quadmesh->n, &quadmesh->normals );
        }

        if( status == OK )
            status = io_newline( file, io_flag, format );
    }

    return( status );
}

public  Status  io_text( file, io_flag, format, text )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    text_struct     *text;
{
    Status   status;
    Status   io_object_type();
    Status   io_colour();
    Status   io_int();
    Status   io_real();
    Status   io_point();
    Status   io_quoted_string();
    Status   io_newline();

    status = io_object_type( file, io_flag, format, TEXT );

    if( status == OK )
        status = io_int( file, io_flag, format, (int *) &text->font );

    if( status == OK )
        status = io_real( file, io_flag, format, &text->size );

    if( status == OK )
        status = io_colour( file, io_flag, format, &text->colour );

    if( status == OK )
        status = io_point( file, io_flag, format, &text->origin );

    if( status == OK )
        status = io_quoted_string( file, io_flag, format, text->text,
                                   MAX_STRING_LENGTH );

    if( status == OK )
        status = io_newline( file, io_flag, format );

    return( status );
}

public  Status  io_volume( file, io_flag, format, volume )
    FILE                *file;
    IO_types            io_flag;
    File_formats        format;
    volume_struct       *volume;
{
    Status   status;
    Status   io_object_type();
    Status   io_int();
    Status   io_real();
    Status   io_newline();
    Status   io_volume_mapping();
    int      dummy_int = 0;
    Real     dummy_real = 0.0;

    status = OK;

    if( io_flag == READ_FILE ||
        (volume->sizes[X] > 0 &&
         volume->sizes[Y] > 0 &&
         volume->sizes[Z] > 0) )
    {
        status = io_object_type( file, io_flag, format, VOLUME );

        if( status == OK )
        {
            status = io_quoted_string( file, io_flag, format,
                                       volume->filename, MAX_STRING_LENGTH );
        }

        if( status == OK )
        {
            status = io_quoted_string( file, io_flag, format,
                                       volume->activity_filename,
                                       MAX_STRING_LENGTH );
        }

        if( status == OK )
            status = io_int( file, io_flag, format, &volume->sizes[X] );

        if( status == OK )
            status = io_int( file, io_flag, format, &volume->sizes[Y] );

        if( status == OK )
            status = io_int( file, io_flag, format, &volume->sizes[Z] );

        if( status == OK )
        {
            status = io_volume_mapping( file, io_flag, format,
                                        volume->axis_map, volume->axis_flip );
        }

        if( status == OK )
        {
            status = io_real( file, io_flag, format,
                              &volume->slice_thickness[X] );
        }

        if( status == OK )
        {
            status = io_real( file, io_flag, format,
                              &volume->slice_thickness[Y] );
        }

        if( status == OK )
        {
            status = io_real( file, io_flag, format,
                              &volume->slice_thickness[Z] );
        }

        if( status == OK )
            status = io_real( file, io_flag, format,
                              &volume->activity_threshold );

        if( status == OK )
            status = io_real( file, io_flag, format, &dummy_real );

        if( status == OK )
            status = io_newline( file, io_flag, format );
    }

    return( status );
}

public  Status  io_volume_mapping( file, io_flag, format, axis_map, axis_flip )
    FILE                *file;
    IO_types            io_flag;
    File_formats        format;
    int                 axis_map[N_DIMENSIONS];
    Boolean             axis_flip[N_DIMENSIONS];
{
    Status         status;
    int            axis, axis_index;
    char           ch;
    static   char  axis_names[N_DIMENSIONS] = { 'x', 'y', 'z' };
    Status         input_nonwhite_character();
    Status         output_character();

    status = OK;

    if( io_flag == WRITE_FILE )
    {
        for_less( axis, 0, N_DIMENSIONS )
        {
            if( axis_flip[axis] )
                status = output_character( file, '-' );

            if( status == OK )
                status = output_character( file, axis_names[axis_map[axis]] );

            if( status == OK && format == ASCII_FORMAT )
                status = output_character( file, ' ' );
        }
    }
    else
    {
        for_less( axis, 0, N_DIMENSIONS )
        {
            axis_flip[axis] = FALSE;

            status = input_nonwhite_character( file, &ch );

            if( ch == '-' )
            {
                status = input_nonwhite_character( file, &ch );
                axis_flip[axis] = TRUE;
            }

            if( ch >= 'X' && ch <= 'Z' )
                axis_index = ch - 'X';
            else
                axis_index = ch - 'x';

            if( axis_index < 0 || axis_index >= N_DIMENSIONS )
            {
                PRINT_ERROR( "Error in axis name.\n" );
                status = ERROR;
                axis_index = 0;
                break;
            }

            axis_map[axis] = axis_index;
        }
    }

    return( status );
}

public  Status  io_point( file, io_flag, format, point )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    Point           *point;
{
    Status   status;
    Status   io_real();
    Status   io_binary_data();

    status = OK;

    if( format == ASCII_FORMAT )
    {
        status = io_real( file, io_flag, format, &Point_x(*point) );

        if( status == OK )
            status = io_real( file, io_flag, format, &Point_y(*point) );

        if( status == OK )
            status = io_real( file, io_flag, format, &Point_z(*point) );
    }
    else
    {
        status = io_binary_data( file, io_flag, (VOID *) point,
                                 sizeof(*point), 1 );
    }

    return( status );
}

public  Status  io_vector( file, io_flag, format, vector )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    Vector          *vector;
{
    Status   status;

    status = OK;

    if( format == ASCII_FORMAT )
    {
        status = io_real( file, io_flag, format, &Vector_x(*vector) );

        if( status == OK )
            status = io_real( file, io_flag, format, &Vector_y(*vector));

        if( status == OK )
            status = io_real( file, io_flag, format, &Vector_z(*vector));
    }
    else
    {
        status = io_binary_data( file, io_flag, (VOID *) vector,
                                 sizeof(*vector), 1 );
    }

    return( status );
}

public  Status  io_colour( file, io_flag, format, colour )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    Colour          *colour;
{
    Status   status;

    status = OK;

    if( format == ASCII_FORMAT )
    {
        status = io_real( file, io_flag, format, &Colour_r(*colour) );

        if( status == OK )
            status = io_real( file, io_flag, format, &Colour_g(*colour));

        if( status == OK )
            status = io_real( file, io_flag, format, &Colour_b(*colour));
    }
    else
    {
        status = io_binary_data( file, io_flag, (VOID *) colour,
                                 sizeof(*colour), 1 );
    }

    return( status );
}

public  Status  io_colours( file, io_flag, format, colour_flag, n_items,
                            n_points, colours )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    Colour_flags    *colour_flag;
    int             n_items;
    int             n_points;
    Colour          **colours;
{
    int      i, n_colours;
    Status   status;

    status = io_int( file, io_flag, format, (int *) colour_flag );

    if( status == OK )
    {
        switch( *colour_flag )
        {
        case ONE_COLOUR:          n_colours = 1;   break;
        case PER_ITEM_COLOURS:    n_colours = n_items;   break;
        case PER_VERTEX_COLOURS:  n_colours = n_points;   break;
        default:
            PRINT_ERROR( "Error inputting colour flag.\n" );
            status = ERROR;
            break;
        }
    }

    if( status == OK && io_flag == READ_FILE )
        ALLOC( status, *colours, n_colours );

    if( status == OK )
    {
        if( format == ASCII_FORMAT )
        {
            for_less( i, 0, n_colours )
                status = io_colour( file, io_flag, format, &(*colours)[i] );
        }
        else
            status = io_binary_data( file, io_flag, (VOID *) (*colours),
                                 sizeof((*colours)[0]), n_colours );
    }

    return( status );
}

public  Status  io_surfprop( file, io_flag, format, surfprop )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    Surfprop        *surfprop;
{
    Status   status;

    status = OK;

    if( format == ASCII_FORMAT )
    {
        status = io_real( file, io_flag, format,
                          &Surfprop_a(*surfprop) );

        if( status == OK )
            status = io_real( file, io_flag, format, &Surfprop_d(*surfprop) );

        if( status == OK )
            status = io_real( file, io_flag, format, &Surfprop_s(*surfprop) );

        if( status == OK )
            status = io_real( file, io_flag, format, &Surfprop_se(*surfprop) );

        if( status == OK )
            status = io_real( file, io_flag, format, &Surfprop_t(*surfprop) );

    }
    else
    {
        status = io_binary_data( file, io_flag, (VOID *) surfprop,
                                 sizeof(*surfprop), 1 );
    }

    if( io_flag == READ_FILE && Surfprop_t(*surfprop) == 0.0 )
        Surfprop_t(*surfprop) = 1.0;

    return( status );
}

private  Status  io_points( file, io_flag, format, n, points )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    int             n;
    Point           *points[];
{
    Status   status;
    int      i;

    status = OK;

    if( io_flag == READ_FILE )
    {
        ALLOC( status, *points, n );
    }

    if( status == OK )
    {
        if( format == ASCII_FORMAT )
        {
            for_less( i, 0, n )
            {
                status = io_point( file, io_flag, format, &(*points)[i]);

                if( status == OK )
                    status = io_newline( file, io_flag, format );

                if( status == ERROR )
                    break;
            }
        }
        else
        {
            status = io_binary_data( file, io_flag, (VOID *) (*points),
                                     sizeof((*points)[0]), n );
        }
    }

    return( status );
}

private  Status  io_vectors( file, io_flag, format, n, vectors )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    int             n;
    Vector          *vectors[];
{
    Status   status;
    int      i;

    status = OK;

    if( io_flag == READ_FILE )
    {
        ALLOC( status, *vectors, n );
    }

    if( format == ASCII_FORMAT )
    {
        for_less( i, 0, n )
        {
            status = io_vector( file, io_flag, format, &(*vectors)[i] );

            if( status == OK )
                status = io_newline( file, io_flag, format );

            if( status == ERROR )
                break;
        }
    }
    else
    {
        status = io_binary_data( file, io_flag, (VOID *) (*vectors),
                                 sizeof((*vectors)[0]), n );
    }

    return( status );
}

public  Status  io_object_type( file, io_flag, format, type )
    FILE           *file;
    int            io_flag, format;
    object_types   type;
{
    int      ch;
    Status   status;

    status = OK;

    if( io_flag == WRITE_FILE )
    {
        switch( type )
        {
        case  LINES:       ch = 'l';    break;
        case  MARKER:      ch = 'm';    break;
        case  MODEL:       ch = 'f';    break;
        case  PIXELS:      ch = 'x';    break;
        case  POLYGONS:    ch = 'p';    break;
        case  QUADMESH:    ch = 'q';    break;
        case  TEXT:        ch = 't';    break;
        case  VOLUME:      ch = 'v';    break;
        }

        if( format == ASCII_FORMAT )
        {
            ch += 'A' - 'a';
        }

        if( fputc( ch, file ) == EOF )
        {
            PRINT_ERROR( "Error outputting char.\n" );
            status = ERROR;
        }
    }

    return( status );
}

public  Status  input_object_type( file, type, format, eof )
    FILE           *file;
    object_types   *type;
    File_formats   *format;
    Boolean        *eof;
{
    char     ch;
    Status   status;
    Status   input_nonwhite_character();

    status = OK;
    *eof = FALSE;

    status = input_nonwhite_character( file, &ch );

    if( status == OK )
    {
        if( ch >= 'A' && ch <= 'Z' )
        {
            *format = ASCII_FORMAT;
            ch -= 'A' - 'a';
        }
        else
        {
            *format = BINARY_FORMAT;
        }

        switch( ch )
        {
        case  'l':   *type = LINES;        break;
        case  'm':   *type = MARKER;       break;
        case  'f':   *type = MODEL;        break;
        case  'x':   *type = PIXELS;       break;
        case  'p':   *type = POLYGONS;     break;
        case  'q':   *type = QUADMESH;     break;
        case  't':   *type = TEXT;         break;
        case  'v':   *type = VOLUME;       break;

        default:
            PRINT_ERROR( "Unrecognized object type in file.\n" );
            status = ERROR;
        }
    }
    else
    {
        *eof = TRUE;
        status = OK;
    }

    return( status );
}

#define  PIXELS_PER_LINE    1
#define  BUFFER_SIZE        256

public  Status  io_pixel_colours( file, io_flag, format, n, pixel_colours )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    int             n;
    Pixel_colour    *pixel_colours[];
{
    Status         status;
    Status         io_pixel_colour();
    unsigned char  buffer[3*BUFFER_SIZE];
    Pixel_colour   pixel_colour;
    int            i, block, n_blocks, n_to_do, start_pixel_index;

    status = OK;

    if( io_flag == READ_FILE )
        ALLOC( status, *pixel_colours, n );

    if( status == OK )
    {
        if( format == ASCII_FORMAT )
        {
            for_less( i, 0, n )
            {
                status = io_pixel_colour( file, io_flag, format,
                                          &(*pixel_colours)[i] );

                if( status == OK )
                {
                    if( i == n - 1 || (i+1) % PIXELS_PER_LINE == 0 )
                    {
                        status = io_newline( file, io_flag, format );
                    }
                }

                if( status != OK )
                    break;
            }
        }
        else
        {
            n_blocks = n / BUFFER_SIZE;
            if( n % BUFFER_SIZE != 0 )
                ++n_blocks;

            for_less( block, 0, n_blocks )
            {
                start_pixel_index = block * BUFFER_SIZE;
                n_to_do = BUFFER_SIZE;
                if( start_pixel_index + n_to_do > n )
                    n_to_do = n - start_pixel_index;

                if( io_flag == WRITE_FILE )
                {
                    for_less( i, 0, n_to_do )
                    {
                        pixel_colour = (*pixel_colours)[start_pixel_index+i];
                        buffer[3*i+0] = (unsigned char)
                                        Pixel_colour_r(pixel_colour);
                        buffer[3*i+1] = (unsigned char)
                                        Pixel_colour_g(pixel_colour);
                        buffer[3*i+2] = (unsigned char)
                                        Pixel_colour_b(pixel_colour);
                    }
                }

                status = io_binary_data( file, io_flag, (VOID *) buffer,
                                     sizeof(buffer[0]), 3 * n_to_do );

                if( io_flag == READ_FILE )
                {
                    for_less( i, 0, n_to_do )
                    {
                        pixel_colour = RGB_255_TO_PIXEL( buffer[3*i+0],
                                                         buffer[3*i+1],
                                                         buffer[3*i+2] );
                        (*pixel_colours)[start_pixel_index+i] = pixel_colour;
                    }
                }
            }
        }
    }

    return( status );
}

public  Status  io_pixel_colour( file, io_flag, format, pixel_colour )
    FILE            *file;
    IO_types        io_flag;
    File_formats    format;
    Pixel_colour    *pixel_colour;
{
    int      r, g, b;
    Status   status;

    status = OK;

    if( format == ASCII_FORMAT )
    {
        if( io_flag == WRITE_FILE )
        {
            r = Pixel_colour_r( *pixel_colour );
            g = Pixel_colour_g( *pixel_colour );
            b = Pixel_colour_b( *pixel_colour );
        }

        status = io_int( file, io_flag, format, &r );

        if( status == OK )
            status = io_int( file, io_flag, format, &g );

        if( status == OK )
            status = io_int( file, io_flag, format, &b );

        if( io_flag == READ_FILE && status == OK )
            *pixel_colour = RGB_255_TO_PIXEL( r, g, b );
    }
    else
    {
        status = io_binary_data( file, io_flag, (VOID *) pixel_colour,
                                 sizeof(*pixel_colour), 1 );
    }

    return( status );
}

