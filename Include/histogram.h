#ifndef  DEF_HISTOGRAM_H
#define  DEF_HISTOGRAM_H

#include  <volume_io.h>

typedef struct
{
    Real   delta;
    Real   offset;
    int    min_index;
    int    max_index;
    int    *counts;
} histogram_struct;

#endif
