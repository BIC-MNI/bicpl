#ifndef  DEF_STACK
#define  DEF_STACK

/* ----------------------------- MNI Header -----------------------------------
@NAME       : def_stack.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: A set of macros for manipulating stacks of any type.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <def_arrays.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : STACK_STRUCT
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Defines the type for a stack.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  STACK_STRUCT( type ) \
         struct \
         { \
             int      top; \
             int      n_stack_alloced; \
             type     *stack; \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : INITIALIZE_STACK
@INPUT      : s
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Initializes a stack to empty.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  INITIALIZE_STACK( s ) \
         { \
             (s).top = 0; \
             (s).n_stack_alloced = 0; \
             (s).stack = 0; \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : PUSH_STACK
@INPUT      : s
            : entry
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Pushes an entry onto the stack.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  PUSH_STACK( s, entry ) \
         { \
             ADD_ELEMENT_TO_ARRAY_WITH_SIZE( (s).stack, (s).n_stack_alloced, \
                          (s).top, entry, DEFAULT_CHUNK_SIZE )               \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : N_ENTRIES_IN_STACK
@INPUT      : s
@OUTPUT     : 
@RETURNS    : # entries on stack
@DESCRIPTION: Returns the number of entries on the stack.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  N_ENTRIES_IN_STACK( s )  ((s).top)

/* ----------------------------- MNI Header -----------------------------------
@NAME       : IS_STACK_EMPTY
@INPUT      : s
@OUTPUT     : 
@RETURNS    : TRUE if stack is empty
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  IS_STACK_EMPTY( s )  ((s).top <= 0)

/* ----------------------------- MNI Header -----------------------------------
@NAME       : TOP_OF_STACK
@INPUT      : s
@OUTPUT     : 
@RETURNS    : The top of stack
@DESCRIPTION: Returns the entry at top of stack without popping it.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  TOP_OF_STACK( s ) ((s).stack[(s).top-1])

/* ----------------------------- MNI Header -----------------------------------
@NAME       : POP_STACK
@INPUT      : s
@OUTPUT     : entry
@RETURNS    : 
@DESCRIPTION: Pops and returns the entry at top of stack.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  POP_STACK( s, entry ) \
         { \
             if( !IS_STACK_EMPTY(s) ) \
             { \
                 --(s).top; \
                 entry = (s).stack[(s).top]; \
             } \
         }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : DELETE_STACK
@INPUT      : s
@OUTPUT     : status
@RETURNS    : 
@DESCRIPTION: Deletes the stack.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#define  DELETE_STACK( s ) \
         { \
             if( (s).n_stack_alloced > 0 ) \
             { \
                 FREE( (s).stack ) \
             } \
         }


#endif
