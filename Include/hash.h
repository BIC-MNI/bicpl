#ifndef  DEF_HASH
#define  DEF_HASH

/* ----------------------------- MNI Header -----------------------------------
@NAME       : def_hash.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Structures and definitions for the generic hash table routines.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

#include  <def_mni.h>

typedef  struct  hash_entry_struct
{
    char                         *data_ptr;
    struct   hash_entry_struct   *next;
    int                          keys[1];
} hash_entry_struct;

typedef  struct
{
    int                 n_keys;
    int                 size;
    int                 n_entries;
    float               enlarge_threshold;
    float               new_density;
    hash_entry_struct   **table;
} hash_table_struct;

typedef  struct
{
    int                current_index;
    hash_entry_struct  *current_entry;
} hash_table_pointer;

Status        initialize_hash_table();
Status        insert_in_hash_table();
Boolean       lookup_in_hash_table();
Boolean       remove_from_hash_table();
Status        delete_hash_table();
void          initialize_hash_pointer();
Boolean       get_next_hash_entry();

#endif
