
#include  <def_mni.h>

private  Status  io_points(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    int             n,
    Point           *points[] );
private  Status  io_vectors(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    int             n,
    Vector          *vectors[] );

public  Status  io_lines(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    lines_struct        *lines )
{
    Status   status;

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

public  Status  io_marker(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    marker_struct       *marker )
{
    Status   status;

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

public  Status  io_model(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    model_struct    *model )
{
    Status   status;

    status = io_object_type( file, io_flag, format, MODEL );

    if( status == OK )
        status = io_quoted_string( file, io_flag, format, model->filename,
                                   MAX_STRING_LENGTH );

    if( status == OK )
        status = io_newline( file, io_flag, format );

    return( status );
}

public  Status  io_pixels(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    pixels_struct       *pixels )
{
    Status   status;
    int      n_pixels;

    status = OK;

    if( io_flag == READ_FILE || (pixels->x_size > 0 && pixels->y_size > 0) )
    {
        status = io_object_type( file, io_flag, format, PIXELS );

        if( status == OK )
            status = io_int( file, io_flag, format, (int *)&pixels->pixel_type);

        if( status == OK )
            status = io_int( file, io_flag, format, &pixels->x_size );

        if( status == OK )
            status = io_int( file, io_flag, format, &pixels->y_size );

        if( status == OK && io_flag == READ_FILE )
        {
            pixels->x_position = 0;
            pixels->y_position = 0;
        }

        if( status == OK )
        {
            n_pixels = pixels->x_size * pixels->y_size;

            switch( pixels->pixel_type )
            {
            case COLOUR_INDEX_8BIT_PIXEL:
                status = io_unsigned_chars( file, io_flag, format, n_pixels,
                                  &pixels->data.pixels_8bit_colour_index );
                break;

            case COLOUR_INDEX_16BIT_PIXEL:
                if( io_flag == READ_FILE )
                    ALLOC( pixels->data.pixels_16bit_colour_index, n_pixels );

                status = io_binary_data( file, io_flag,
                          (void *) (&pixels->data.pixels_16bit_colour_index),
                          sizeof(pixels->data.pixels_16bit_colour_index[0]),
                          n_pixels );
                break;

            case RGB_PIXEL:
                status = io_pixel_colours( file, io_flag, format, n_pixels,
                                           &pixels->data.pixels_rgb );
                break;

            default:
                print( "Error, unrecognized pixel type %d.\n",
                       pixels->pixel_type );
                status = ERROR;
            }
        }
    }

    return( status );
}

public  Status  io_polygons(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    polygons_struct     *polygons )
{
    Status   status;

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

public  Status  io_quadmesh(
    FILE                *file,
    IO_types            io_flag,
    File_formats        format,
    quadmesh_struct     *quadmesh )
{
    Status   status;

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

public  Status  io_text(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    text_struct     *text )
{
    Status   status;

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
        status = io_quoted_string( file, io_flag, format, text->string,
                                   MAX_STRING_LENGTH );

    if( status == OK )
        status = io_newline( file, io_flag, format );

    return( status );
}

public  Status  io_point(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Point           *point )
{
    Status   status;

    status = OK;

    if( format == ASCII_FORMAT )
    {
        status = io_float( file, io_flag, format, &Point_x(*point) );

        if( status == OK )
            status = io_float( file, io_flag, format, &Point_y(*point) );

        if( status == OK )
            status = io_float( file, io_flag, format, &Point_z(*point) );
    }
    else
    {
        status = io_binary_data( file, io_flag, (void *) point,
                                 sizeof(*point), 1 );
    }

    return( status );
}

public  Status  io_vector(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Vector          *vector )
{
    Status   status;

    status = OK;

    if( format == ASCII_FORMAT )
    {
        status = io_float( file, io_flag, format, &Vector_x(*vector) );

        if( status == OK )
            status = io_float( file, io_flag, format, &Vector_y(*vector));

        if( status == OK )
            status = io_float( file, io_flag, format, &Vector_z(*vector));
    }
    else
    {
        status = io_binary_data( file, io_flag, (void *) vector,
                                 sizeof(*vector), 1 );
    }

    return( status );
}

public  Status  io_colour(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Colour          *colour )
{
    float    r, g, b;
    Status   status;

    status = OK;

    if( io_flag == WRITE_FILE )
    {
        r = get_Colour_r_0_1( *colour );
        g = get_Colour_g_0_1( *colour );
        b = get_Colour_b_0_1( *colour );
    }

    status = io_float( file, io_flag, format, &r );
    if( status == OK )
        status = io_float( file, io_flag, format, &g );
    if( status == OK )
        status = io_float( file, io_flag, format, &b );

    if( io_flag == READ_FILE )
    {
        *colour = make_Colour_0_1( r, g, b );
    }

    return( status );
}

public  Status  io_colours(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Colour_flags    *colour_flag,
    int             n_items,
    int             n_points,
    Colour          **colours )
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
            print( "Error inputting colour flag.\n" );
            status = ERROR;
            break;
        }
    }

    if( status == OK && io_flag == READ_FILE )
        ALLOC( *colours, n_colours );

    if( status == OK )
    {
        if( TRUE || format == ASCII_FORMAT )
        {
            for_less( i, 0, n_colours )
                status = io_colour( file, io_flag, format, &(*colours)[i] );
        }
        else
            status = io_binary_data( file, io_flag, (void *) (*colours),
                                 sizeof((*colours)[0]), n_colours );
    }

    return( status );
}

public  Status  io_surfprop(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Surfprop        *surfprop )
{
    Status   status;

    status = OK;

    if( format == ASCII_FORMAT )
    {
        status = io_float( file, io_flag, format,
                          &Surfprop_a(*surfprop) );

        if( status == OK )
            status = io_float( file, io_flag, format, &Surfprop_d(*surfprop) );

        if( status == OK )
            status = io_float( file, io_flag, format, &Surfprop_s(*surfprop) );

        if( status == OK )
            status = io_float( file, io_flag, format, &Surfprop_se(*surfprop) );

        if( status == OK )
            status = io_float( file, io_flag, format, &Surfprop_t(*surfprop) );

    }
    else
    {
        status = io_binary_data( file, io_flag, (void *) surfprop,
                                 sizeof(*surfprop), 1 );
    }

    if( io_flag == READ_FILE && Surfprop_t(*surfprop) == 0.0 )
        Surfprop_t(*surfprop) = 1.0;

    return( status );
}

private  Status  io_points(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    int             n,
    Point           *points[] )
{
    Status   status;
    int      i;

    status = OK;

    if( io_flag == READ_FILE )
    {
        ALLOC( *points, n );
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
            status = io_binary_data( file, io_flag, (void *) (*points),
                                     sizeof((*points)[0]), n );
        }
    }

    return( status );
}

private  Status  io_vectors(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    int             n,
    Vector          *vectors[] )
{
    Status   status;
    int      i;

    status = OK;

    if( io_flag == READ_FILE )
    {
        ALLOC( *vectors, n );
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
        status = io_binary_data( file, io_flag, (void *) (*vectors),
                                 sizeof((*vectors)[0]), n );
    }

    return( status );
}

public  Status  io_object_type(
    FILE           *file,
    IO_types       io_flag,
    File_formats   format,
    Object_types   type )
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
/*
        case  VOLUME:      ch = 'v';    break;
*/
        }

        if( format == ASCII_FORMAT )
        {
            ch += 'A' - 'a';
        }

        if( fputc( ch, file ) == EOF )
        {
            print( "Error outputting char.\n" );
            status = ERROR;
        }
    }

    return( status );
}

public  Status  input_object_type(
    FILE           *file,
    Object_types   *type,
    File_formats   *format,
    Boolean        *eof )
{
    char     ch;
    Status   status;

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
/*
        case  'v':   *type = VOLUME;       break;
*/

        default:
            print( "Unrecognized object type in file.\n" );
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

public  Status  io_pixel_colours(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    int             n,
    Colour          *pixel_colours[] )
{
    Status         status;
    unsigned char  buffer[3*BUFFER_SIZE];
    Colour         pixel_colour;
    int            i, block, n_blocks, n_to_do, start_pixel_index;

    status = OK;

    if( io_flag == READ_FILE )
        ALLOC( *pixel_colours, n );

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
                                        get_Colour_r(pixel_colour);
                        buffer[3*i+1] = (unsigned char)
                                        get_Colour_g(pixel_colour);
                        buffer[3*i+2] = (unsigned char)
                                        get_Colour_b(pixel_colour);
                    }
                }

                status = io_binary_data( file, io_flag, (void *) buffer,
                                     sizeof(buffer[0]), 3 * n_to_do );

                if( io_flag == READ_FILE )
                {
                    for_less( i, 0, n_to_do )
                    {
                        pixel_colour = make_Colour( buffer[3*i+0],
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

public  Status  io_pixel_colour(
    FILE            *file,
    IO_types        io_flag,
    File_formats    format,
    Colour          *pixel_colour )
{
    int      r, g, b;
    Status   status;

    status = OK;

    if( format == ASCII_FORMAT )
    {
        if( io_flag == WRITE_FILE )
        {
            r = get_Colour_r( *pixel_colour );
            g = get_Colour_g( *pixel_colour );
            b = get_Colour_b( *pixel_colour );
        }

        status = io_int( file, io_flag, format, &r );

        if( status == OK )
            status = io_int( file, io_flag, format, &g );

        if( status == OK )
            status = io_int( file, io_flag, format, &b );

        if( io_flag == READ_FILE && status == OK )
            *pixel_colour = make_Colour( r, g, b );
    }
    else
    {
        status = io_binary_data( file, io_flag, (void *) pixel_colour,
                                 sizeof(*pixel_colour), 1 );
    }

    return( status );
}
