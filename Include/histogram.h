#ifndef  DEF_HISTOGRAM_H
#define  DEF_HISTOGRAM_H

#include  <mni.h>

typedef struct
{
    Real   delta;
    Real   offset;
    int    min_index;
    int    max_index;
    int    *counts;
} histogram_struct;

public  void  initialize_histogram(
    histogram_struct  *histogram,
    Real              delta,
    Real              offset );

public  void  delete_histogram(
    histogram_struct  *histogram );

public  void  add_to_histogram(
    histogram_struct  *histogram,
    Real              value );

public  void  display_histogram(
    histogram_struct  *histogram,
    int               x_size,
    int               y_size );

#endif
