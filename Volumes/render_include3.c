{
    if( volume_data2 == (void *) NULL )
    {
#include  "render_include.c"
    }
    else
    {
#define  TWO_VOLUMES
        switch( volume2_type )
        {
        case  UNSIGNED_BYTE:
#define  TYPE2   unsigned char
#include  "render_include.c"
#undef   TYPE2
            break;

        case  SIGNED_BYTE:
#define  TYPE2   signed char
#include  "render_include.c"
#undef   TYPE2
            break;

        case  UNSIGNED_SHORT:
#define  TYPE2   unsigned short
#include  "render_include.c"
#undef   TYPE2
            break;

        case  SIGNED_SHORT:
#define  TYPE2   signed short
#include  "render_include.c"
#undef   TYPE2
            break;

        case  UNSIGNED_LONG:
#define  TYPE2   unsigned long
#include  "render_include.c"
#undef   TYPE2
            break;

        case  SIGNED_LONG:
#define  TYPE2   signed long
#include  "render_include.c"
#undef   TYPE2
            break;

        case  FLOAT:
#define  TYPE2   float
#include  "render_include.c"
#undef   TYPE2
            break;

        case  DOUBLE:
#define  TYPE2   double
#include  "render_include.c"
#undef   TYPE2
            break;
        }
    }
#undef  TWO_VOLUMES
}
