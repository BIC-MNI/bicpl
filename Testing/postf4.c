#define GABE

#ifdef GABE

#define  GLOBALS_LOOKUP_NAME  globals_list
#include <globals.h>

#else

#include  <internal_volume_io.h>

#define  GLOBALS_LOOKUP_NAME  globals_list
#include  <globals.h>

#endif
