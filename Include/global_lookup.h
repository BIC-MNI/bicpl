#ifndef  DEF_GLOBAL_LOOKUP
#define  DEF_GLOBAL_LOOKUP

#include  <volume_io.h>

typedef  enum {
                 BOOLEAN_type,
                 int_type,
                 Real_type,
                 String_type,
                 Point_type,
                 Vector_type,
                 Colour_type,
                 Surfprop_type
              } Variable_types;

typedef  struct 
{
    void             *ptr_to_global;
    STRING           variable_name;
    Variable_types   type;
} global_struct;

#endif
