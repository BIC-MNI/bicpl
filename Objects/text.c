
#include  <def_objects.h>

public  Status  initialize_text( text, position, colour, font, size )
    text_struct     *text;
    Point           *position;
    Colour          colour;
    Font_types      font;
    Real            size;
{
    text->origin = *position;
    text->colour = colour;
    text->font = font;
    text->size = size;

    return( OK );
}
