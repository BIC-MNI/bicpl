#ifdef  GLOBALS_LOOKUP_NAME
#undef  DEF_GLOBALS
#endif

#ifndef  DEF_GLOBALS
#define  DEF_GLOBALS

#include  <volume_io.h>

/*  ----------------- globals are being defined --------------------- */

#ifdef  GLOBALS_LOOKUP_NAME

#define  START_GLOBALS
#define  END_GLOBALS

#define  DEF_GLOBAL( variable, type, initial ) \
    type  variable = { initial };

#define  DEF_GLOBAL3( variable, type, initial1, initial2, initial3 ) \
    type  variable = { initial1, initial2, initial3 };

#define  DEF_GLOBAL5( variable, type, initial1, initial2, initial3, initial4, initial5 ) \
    type  variable = { initial1, initial2, initial3, initial4, initial5 };

#define  DEF_GLOBAL_STRING( variable, initial ) \
    DEF_GLOBAL( variable, STRING, initial )

#include  <global_variables.h>

#undef  START_GLOBALS
#undef  END_GLOBALS
#undef  DEF_GLOBAL
#undef  DEF_GLOBAL3
#undef  DEF_GLOBAL5
#undef  DEF_GLOBAL_STRING

/*  ----- database of correspondence between globals and string names ----- */

#include  <global_lookup.h>

#define  START_GLOBALS   private  global_struct  GLOBALS_LOOKUP_NAME[] = {
#define  END_GLOBALS                                        };

#define  DEF_GLOBAL( variable, type, initial ) \
       { \
           (void *) &variable, CREATE_STRING(variable), GLUE(type,_type) \
       },

#define  DEF_GLOBAL3( variable, type, initial1, initial2, initial3 ) \
         DEF_GLOBAL( variable, type, initial1 )

#define  DEF_GLOBAL5( variable, type, i1, i2, i3, i4, i5 ) \
         DEF_GLOBAL( variable, type, i1 )

#define  DEF_GLOBAL_STRING( variable, initial ) \
       { \
           (void *) variable, CREATE_STRING(variable), String_type \
       },

#include  <global_variables.h>

#undef  START_GLOBALS
#undef  END_GLOBALS
#undef  DEF_GLOBAL
#undef  DEF_GLOBAL3
#undef  DEF_GLOBAL5
#undef  DEF_GLOBAL_STRING

#else   /* GLOBALS_LOOKUP_NAME */

/*  ------ globals are being externed from application code ------------- */

#define  START_GLOBALS
#define  END_GLOBALS

#define  DEF_GLOBAL( variable, type, initial ) \
         extern  type  variable;

#define  DEF_GLOBAL3( variable, type, i1, i2, i3 ) \
         DEF_GLOBAL( variable, type, i1 )

#define  DEF_GLOBAL5( variable, type, i1, i2, i3, i4, i5 ) \
         DEF_GLOBAL( variable, type, i1 )

#define  DEF_GLOBAL_STRING( variable, initial ) \
         extern  STRING  variable;

#include  <global_variables.h>

#undef  START_GLOBALS
#undef  END_GLOBALS
#undef  DEF_GLOBAL
#undef  DEF_GLOBAL3
#undef  DEF_GLOBAL5
#undef  DEF_GLOBAL_STRING

#endif   /* GLOBALS_LOOKUP_NAME */

#undef GLOBALS_LOOKUP_NAME

#endif
