#ifndef  DEF_OBJECT_DEFS
#define  DEF_OBJECT_DEFS

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

#ifndef lint
static char obj_defs_rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Include/Attic/obj_defs.h,v 1.9 2000-01-19 22:59:51 stever Exp $";
#endif

#include  <volume_io.h>
#include  <bicpl/stack.h>
#include  <bicpl/bintree.h>

#define  SET_OBJECT_SIZE( object, n, size ) \
             { \
                 if( (n) == 0 ) \
                     (object).end_indices[0] = (size); \
                 else \
                     (object).end_indices[n] = (size) + \
                                               (object).end_indices[(n)-1]; \
             }

#define  GET_OBJECT_SIZE( object, n ) \
             ( ((n) == 0) ? (object).end_indices[0] : \
                         ((object).end_indices[n] - \
                          (object).end_indices[(n)-1]) )

#define  NUMBER_INDICES( object ) \
             ( ((object).n_items == 0) ? 0 : \
                  (object).end_indices[(object).n_items-1] )

#define  START_INDEX( end_indices, n ) \
             ( ((n) == 0) ? 0 : (end_indices)[(n)-1] )

#define  POINT_INDEX( end_indices, n, edge ) \
             ( START_INDEX(end_indices,n) + (edge) )

typedef enum { ONE_COLOUR, PER_ITEM_COLOURS, PER_VERTEX_COLOURS } Colour_flags;

typedef  struct
{
    Colour_flags   colour_flag;
    Colour         *colours;
    float          line_thickness;

    int            n_points;
    Point          *points;

    int            n_items;
    int            *end_indices;
    int            *indices;

    bintree_struct_ptr  bintree;
} lines_struct;

typedef  enum  { BOX_MARKER, SPHERE_MARKER,
                 N_MARKER_TYPES } Marker_types;

typedef  struct
{
    Marker_types   type;
    Colour         colour;
    Point          position;
    Real           size;
    STRING         label;
    int            structure_id;
    int            patient_id;
} marker_struct;

#define  MAX_POINTS_PER_POLYGON     20
#define  MAX_POLYGON_NEIGHBOURS     2048

typedef  struct
{
    Colour_flags    colour_flag;
    Colour          *colours;
    Surfprop        surfprop;
    float           line_thickness;

    int             n_points;
    Point           *points;
    Vector          *normals;

    int             n_items;
    int             *end_indices;
    int             *indices;

    Smallest_int    *visibilities;
    int             *neighbours;
    bintree_struct_ptr  bintree;
} polygons_struct;

typedef  struct
{
    Colour_flags    colour_flag;
    Colour          *colours;
    Surfprop        surfprop;

    BOOLEAN         m_closed, n_closed;

    int             m, n;
    Point           *points;
    Vector          *normals;

    bintree_struct_ptr  bintree;
} quadmesh_struct;

typedef enum { FIXED_FONT, SIZED_FONT } Font_types;

typedef  struct
{
    Point       origin;
    Colour      colour;
    Font_types  font;
    Real        size;
    STRING      string;
} text_struct;

typedef enum {
                 COLOUR_INDEX_8BIT_PIXEL, COLOUR_INDEX_16BIT_PIXEL, RGB_PIXEL
             } Pixel_types;

typedef  struct
{
    int             x_position;
    int             y_position;
    int             x_size;
    int             y_size;
    Real            x_zoom;      
    Real            y_zoom;      
    Pixel_types     pixel_type;
    union
    {
        unsigned char   *pixels_8bit_colour_index;
        unsigned short  *pixels_16bit_colour_index;
        Colour          *pixels_rgb;
    } data;

} pixels_struct;

#define  PIXEL_COLOUR_INDEX_8( pixels, x, y )                                 \
           ((pixels).data.pixels_8bit_colour_index[IJ(y,x,(pixels).x_size)])

#define  PIXEL_COLOUR_INDEX_16( pixels, x, y )                                \
           ((pixels).data.pixels_16bit_colour_index[IJ(y,x,(pixels).x_size)])

#define  PIXEL_RGB_COLOUR( pixels, x, y )                                     \
           ((pixels).data.pixels_rgb[IJ(y,x,(pixels).x_size)])

/* ------------------------------------------------------------------------ */

typedef enum  { LINES, MARKER, MODEL, PIXELS, POLYGONS, QUADMESH, TEXT,
                N_OBJECT_TYPES } Object_types;

typedef  struct
{
    STRING                 filename;
    int                    n_objects;
    struct  object_struct  **objects;
    void                   *extra_ptr;
} model_struct;

typedef struct  object_struct
{
    Object_types    object_type;
    BOOLEAN         visibility;

    union
    {
    lines_struct       lines;
    marker_struct      marker;
    model_struct       model;
    pixels_struct      pixels;
    polygons_struct    polygons;
    quadmesh_struct    quadmesh;
    text_struct        text;
    } specific;

} object_struct;


typedef  struct
{
    int              index;
    int              n_objects;
    object_struct    **object_list;
} object_stack_struct;

#define  MAX_OBJECT_TRAVERSE   100

typedef  struct
{
    BOOLEAN                    visible_ones_only;
    int                        n_stack_alloced;
    int                        top_of_stack;
    object_stack_struct        *stack;
    object_stack_struct        static_stack[MAX_OBJECT_TRAVERSE];
    object_stack_struct        *alloced_stack;
} object_traverse_struct;

#endif
