
#include  <internal_volume_io.h>
#include  <objects.h>

public  void   initialize_text(
    text_struct     *text,
    Point           *position,
    Colour          colour,
    Font_types      text_font,
    Real            size )
{
    if( position != NULL )
        text->origin = *position;

    text->colour = colour;
    text->font = text_font;
    text->size = size;
}
