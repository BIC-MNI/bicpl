{
    if( n_slices1 == 1 && weights1[0] == 1.0 &&
        (volume_data2 == (void *) NULL ||
         n_slices2 == 1 && weights2[0] == 1.0) )
    {
#define   ONE_SLICE
#include  "render_include2.c"
#undef    ONE_SLICE
    }
    else
    {
#include  "render_include2.c"
    }
}
