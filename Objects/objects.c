#include  <def_mni.h>

public  object_struct  *create_object(
    Object_types   object_type )
{
    Status          status;
    object_struct   *object;

    ALLOC( status, object, 1 );

    if( status == OK )
    {
        object->object_type = object_type;
        object->visibility = FALSE;
    }

    return( object );
}

public  void  delete_object(
    object_struct  *object )
{
    Status          status;

    FREE( status, object );

    if( status != OK ) {}
}

public  void  set_object_visibility(
    object_struct  *object,
    Boolean        visibility )
{
    object->visibility = visibility;
}

public  void  *get_object_pointer(
    object_struct  *object )
{
    void   *ptr;

    switch( object->object_type )
    {
    case  LINES:       ptr = (void *) &object->specific.lines;         break;
    case  MARKER:      ptr = (void *) &object->specific.marker;        break;
    case  PIXELS:      ptr = (void *) &object->specific.pixels;        break;
    case  POLYGONS:    ptr = (void *) &object->specific.polygons;      break;
    case  QUADMESH:    ptr = (void *) &object->specific.quadmesh;      break;
    case  TEXT:        ptr = (void *) &object->specific.text;          break;
    }

    return( ptr );
}

/* --------------------- object functions ------------------------ */

private  void  draw_lines(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_lines( window, (lines_struct *) get_object_pointer(object) );
}

private  void  draw_marker(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_marker( window, (marker_struct *) get_object_pointer(object) );
}

private  void  draw_pixels(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_pixels( window, (pixels_struct *) get_object_pointer(object) );
}

private  void  draw_polygons(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_polygons( window, (polygons_struct *) get_object_pointer(object) );
}

private  void  draw_quadmesh(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_quadmesh( window, (quadmesh_struct *) get_object_pointer(object) );
}

private  void  draw_text(
    window_struct   *window,
    object_struct   *object )
{
    G_draw_text( window, (text_struct *) get_object_pointer(object) );
}

/* -------------------- function lookup table ------------------- */

typedef  struct
{
    void   (*draw_function)( window_struct *, object_struct * );
}
object_functions_list;

static  object_functions_list   object_functions[N_OBJECT_TYPES] =
{
    {
        draw_lines
    },                              /* LINES */
    {
        draw_marker
    },                              /* MARKER */
    {
        draw_pixels
    },                              /* PIXELS */
    {
        draw_polygons
    },                              /* POLYGONS */
    {
        draw_quadmesh
    },                              /* QUADMESH */
    {
        draw_text
    }                               /* TEXT */
};

public  void  draw_object(
    window_struct   *window,
    object_struct   *object )
{
    object_functions[object->object_type].draw_function( window, object );
}
