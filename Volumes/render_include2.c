{
    switch( volume1_type )
    {
    case  UNSIGNED_BYTE:
#define  TYPE1   unsigned char
#include  "render_include3.c"
#undef   TYPE1
        break;

    case  SIGNED_BYTE:
#define  TYPE1   signed char
#include  "render_include3.c"
#undef   TYPE1
        break;

    case  UNSIGNED_SHORT:
#define  TYPE1   unsigned short
#include  "render_include3.c"
#undef   TYPE1
        break;

    case  SIGNED_SHORT:
#define  TYPE1   signed short
#include  "render_include3.c"
#undef   TYPE1
        break;

    case  UNSIGNED_LONG:
#define  TYPE1   unsigned long
#include  "render_include3.c"
#undef   TYPE1
        break;

    case  SIGNED_LONG:
#define  TYPE1   signed long
#include  "render_include3.c"
#undef   TYPE1
        break;

    case  FLOAT:
#define  TYPE1   float
#include  "render_include3.c"
#undef   TYPE1
        break;

    case  DOUBLE:
#define  TYPE1   double
#include  "render_include3.c"
#undef   TYPE1
        break;
    }
}
