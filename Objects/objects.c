#include  <def_mni.h>

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

/* --------------------- draw functions ------------------------ */

private  void  draw_lines(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_lines( window, get_lines_ptr(object) );
}

private  void  draw_marker(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_marker( window, get_marker_ptr(object) );
}

private  void  draw_model(
    window_struct   *window,
    object_struct   *object )                    /* ARGSUSED */
{
}

private  void  draw_pixels(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_pixels( window, get_pixels_ptr(object) );
}

private  void  draw_polygons(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_polygons( window, get_polygons_ptr(object) );
}

private  void  draw_quadmesh(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_quadmesh( window, get_quadmesh_ptr(object) );
}

private  void  draw_text(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_text( window, get_text_ptr(object) );
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
    int            i;
    model_struct   *model;

    model = get_model_ptr( object );

    for_less( i, 0, model->n_objects )
        delete_object( model->objects[i] );
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
}

private  void  delete_text_object(
    object_struct   *object )       /* ARGSUSED */
{
}

/* -------------------- function lookup table ------------------- */

typedef  struct
{
    void   (*draw_function)( window_struct *, object_struct * );
    void   (*delete_function)( object_struct * );
}
object_functions_list;

static  object_functions_list   object_functions[N_OBJECT_TYPES] =
{
    {
        draw_lines,
        delete_lines_object
    },                              /* LINES */
    {
        draw_marker,
        delete_marker_object
    },                              /* MARKER */
    {
        draw_model,
        delete_model_object
    },                              /* MODEL */
    {
        draw_pixels,
        delete_pixels_object
    },                              /* PIXELS */
    {
        draw_polygons,
        delete_polygons_object
    },                              /* POLYGONS */
    {
        draw_quadmesh,
        delete_quadmesh_object
    },                              /* QUADMESH */
    {
        draw_text,
        delete_text_object
    }                               /* TEXT */
};

public  void  draw_object(
    window_struct   *window,
    object_struct   *object )
{
    if( object->visibility )
        object_functions[object->object_type].draw_function( window, object );
}

public  void  delete_object(
    object_struct  *object )
{
    object_functions[object->object_type].delete_function( object );
    FREE( object );
}

