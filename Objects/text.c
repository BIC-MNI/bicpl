
#include  <def_mni.h>

public  Status  initialize_text(
    text_struct     *text,
    Point           *position,
    Colour          colour,
    Font_types      text_font,
    Real            size )
{
    text->origin = *position;
    text->colour = colour;
    text->font = text_font;
    text->size = size;

    return( OK );
}
