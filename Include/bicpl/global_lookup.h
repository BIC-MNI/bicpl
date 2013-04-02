#ifndef  DEF_GLOBAL_LOOKUP
#define  DEF_GLOBAL_LOOKUP

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


#include  <volume_io.h>

typedef  enum {
                 VIO_BOOL_type,
                 int_type,
                 VIO_Real_type,
                 VIO_STR_type,
                 VIO_Point_type,
                 VIO_Vector_type,
                 VIO_Colour_type,
                 VIO_Surfprop_type
              } Variable_types;

typedef  struct 
{
    void                 *ptr_to_global;
    VIO_STR              variable_name;
    Variable_types       type;
    VIO_SCHAR            set_flag;
} global_struct;

#endif
