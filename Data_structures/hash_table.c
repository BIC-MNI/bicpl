#include  <stdio.h>
#include  <def_alloc.h>
#include  <def_hash.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_hash_table
@INPUT      : n_keys
            : size
            : enlarge_threshold
            : new_density
@OUTPUT     : hash_table
@RETURNS    : Status
@DESCRIPTION: Initializes a hash table to empty, storing the number of keys,
            : initial size, and parameters controlling automatic growth.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public   Status  initialize_hash_table( hash_table, n_keys, size,
                                        enlarge_threshold, new_density )
    hash_table_struct  *hash_table;
    int                n_keys;
    int                size;
    Real               enlarge_threshold;
    Real               new_density;
{
    Status     status;
    int        i;

    if( n_keys < 1 )
        HANDLE_INTERNAL_ERROR( "Hash table # keys\n" );

    hash_table->n_keys = n_keys;
    hash_table->size = size;
    hash_table->n_entries = 0;
    hash_table->enlarge_threshold = enlarge_threshold;
    hash_table->new_density = new_density;

    ALLOC( status, hash_table->table, size );

    if( status == OK )
    {
        for( i = 0;  i < size;  ++i )
            hash_table->table[i] = (hash_entry_struct *) 0;
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_hash_table_list
@INPUT      : hash_table
@OUTPUT     : 
@RETURNS    : Status
@DESCRIPTION: Deletes the list of pointers in the hash table.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private   Status  delete_hash_table_list( hash_table )
    hash_table_struct  *hash_table;
{
    Status              status;

    status = OK;

    if( hash_table->size > 0 )
        FREE( status, hash_table->table );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_hash_table
@INPUT      : hash_table
@OUTPUT     : 
@RETURNS    : Status
@DESCRIPTION: Deletes the given hash table.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public   Status  delete_hash_table( hash_table )
    hash_table_struct  *hash_table;
{
    Status              status;
    int                 i;
    hash_entry_struct   *entry, *next;

    status = OK;

    for( i = 0;  i < hash_table->size;  ++i )
    {
        entry = hash_table->table[i];

        while( status == OK && entry != (hash_entry_struct *) 0 )
        {
            next = entry->next;
            FREE( status, entry );
            entry = next;
        }

        if( status != OK )
            break;
    }

    if( status == OK )
        status = delete_hash_table_list( hash_table );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : hash_function
@INPUT      : hash_table
            : keys
@OUTPUT     : 
@RETURNS    : an index into the hash table.
@DESCRIPTION: Hashes the one or more keys to create an index into the table.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private   int  hash_function( hash_table, keys )
    hash_table_struct   *hash_table;
    int                 keys[];
{
    int           i;
    unsigned int  single_hash;
    static int    factors[] = { 1, 1393, 2713, 4343 };

    single_hash = keys[0];

    for_less( i, 1, hash_table->n_keys )
        single_hash += factors[i % SIZEOF_STATIC_ARRAY(factors)] * keys[i];

    return( (int) (single_hash % (unsigned int) hash_table->size) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : keys_equal
@INPUT      : n_keys
            : keys1
            : keys2
@OUTPUT     : 
@RETURNS    : TRUE if equal
@DESCRIPTION: Compares the keys to see if they are equal.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  Boolean  keys_equal( n_keys, keys1, keys2 )
    int   n_keys;
    int   keys1[];
    int   keys2[];
{
    Boolean  equal;
    int      i;

    equal = (keys1[0] == keys2[0]);

    for_less( i, 1, n_keys )
    {
        if( !equal )
            break;

        equal = (keys1[i] == keys2[i]);
    }

    return( equal );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : lookup
@INPUT      : hash_table
            : keys
@OUTPUT     : 
@RETURNS    : a pointer to a pointer to the hash table entry
@DESCRIPTION: Looks up the given keys in the hash table, and returns a pointer
            : to a pointer to the entry, suitable for inserting or deleting.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private   hash_entry_struct  **lookup( hash_table, keys )
    hash_table_struct  *hash_table;
    int                keys[];
{
    int                 i;
    hash_entry_struct   **ptr_to_entry;

    i = hash_function( hash_table, keys );

    ptr_to_entry = &hash_table->table[i];

    while( *ptr_to_entry != (hash_entry_struct *) 0 )
    {
        if( keys_equal( hash_table->n_keys, (*ptr_to_entry)->keys, keys ) )
            break;

        ptr_to_entry = &(*ptr_to_entry)->next;
    }

    return( ptr_to_entry );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : insert_in_hash_table
@INPUT      : hash_table
            : keys
            : data_ptr
@OUTPUT     : 
@RETURNS    : Status
@DESCRIPTION: Inserts the data_ptr in the hash table.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  insert_in_hash_table( hash_table, keys, data_ptr )
    hash_table_struct  *hash_table;
    int                keys[];
    void               *data_ptr;
{
    Status              status;
    int                 i;
    hash_entry_struct   **ptr_to_entry;
    hash_entry_struct   *entry;
    Status              increase_hash_table_size();

    ptr_to_entry = lookup( hash_table, keys );

    entry = *ptr_to_entry;

    if( entry == (hash_entry_struct *) 0 )
    {
        ALLOC_VAR_SIZED_STRUCT( status, entry, int, hash_table->n_keys );

        if( status == OK )
        {
            for_less( i, 0, hash_table->n_keys )
                entry->keys[i] = keys[i];

            entry->data_ptr = data_ptr;
            entry->next = *ptr_to_entry;

            *ptr_to_entry = entry; 

            ++hash_table->n_entries;

            if( (Real) hash_table->n_entries / (Real) hash_table->size >
                hash_table->enlarge_threshold )
            {
                int   new_size;

                new_size = (int) (
                        hash_table->n_entries / hash_table->new_density + 0.5);
                status = increase_hash_table_size( hash_table, new_size );
            }
        }
    }
    else
    {
        HANDLE_INTERNAL_ERROR( "Insert in hash table" );
        status = ERROR;
    }

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : lookup_in_hash_table
@INPUT      : hash_table
            : keys
            : data_ptr
@OUTPUT     : 
@RETURNS    : TRUE if found
@DESCRIPTION: Lookups the given keys and data in the hash table.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean  lookup_in_hash_table( hash_table, keys, data_ptr )
    hash_table_struct  *hash_table;
    int                keys[];
    void               **data_ptr;
{
    Boolean             found;
    hash_entry_struct   **ptr_to_entry;
    hash_entry_struct   *entry;

    ptr_to_entry = lookup( hash_table, keys );

    entry = *ptr_to_entry;

    if( entry == (hash_entry_struct *) 0 )
    {
        found = FALSE;
    }
    else
    {
        if( data_ptr != (void **) 0 )
            *data_ptr = entry->data_ptr;

        found = TRUE;
    }

    return( found );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : remove_from_hash_table
@INPUT      : hash_table
            : keys
            : data_ptr
@OUTPUT     : 
@RETURNS    : TRUE if it existed in the table
@DESCRIPTION: Finds and removes the given data from the hash table.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean  remove_from_hash_table( hash_table, keys, data_ptr )
    hash_table_struct  *hash_table;
    int                keys[];
    void               **data_ptr;
{
    Status              status;
    Boolean             removed;
    hash_entry_struct   **ptr_to_entry;
    hash_entry_struct   *entry;

    ptr_to_entry = lookup( hash_table, keys );

    entry = *ptr_to_entry;

    if( entry == (hash_entry_struct *) 0 )
    {
        removed = FALSE;
    }
    else
    {
        if( data_ptr != (void **) 0 )
            *data_ptr = entry->data_ptr;

        *ptr_to_entry = entry->next;

        FREE( status, entry );

        if( status == OK )
        {
            removed = TRUE;
            --hash_table->n_entries;
        }
    }

    return( removed );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : move_hash_entries_to_new_table
@INPUT      : src           hash_table
@OUTPUT     : dest          hash_table
@RETURNS    : 
@DESCRIPTION: Moves the entries of one hash table to the other, without
            : reallocating them.  Used when the hash table size is increased.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private   void  move_hash_entries_to_new_table( dest, src )
    hash_table_struct  *dest;
    hash_table_struct  *src;
{
    int                 i, hash;
    hash_entry_struct   *entry, *next;

    for( i = 0;  i < src->size;  ++i )
    {
        entry = src->table[i];

        while( entry != (hash_entry_struct *) 0 )
        {
            next = entry->next;

            hash = hash_function( dest, entry->keys );
            entry->next = dest->table[hash];
            dest->table[hash] = entry;
            ++dest->n_entries;

            entry = next;
        }

        src->table[i] = (hash_entry_struct *) 0;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : increase_hash_table_size
@INPUT      : hash_table
            : new_size
@OUTPUT     : 
@RETURNS    : Status
@DESCRIPTION: Increases the size of the hash table, by creating a new hash
            : table, moving over the entries, then assigning the new table to
            : the old one.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public   Status  increase_hash_table_size( hash_table, new_size )
    hash_table_struct   *hash_table;
    int                 new_size;
{
    hash_table_struct   new_table;
    Status              status;

    status = initialize_hash_table( &new_table, hash_table->n_keys, new_size,
                                    hash_table->enlarge_threshold,
                                    hash_table->new_density );

    if( status == OK )
    {
        move_hash_entries_to_new_table( &new_table, hash_table );

        status = delete_hash_table_list( hash_table );
    }

    if( status == OK )
        *hash_table = new_table;

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_hash_pointer
@INPUT      : ptr
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Initializes a pointer to the beginning of the hash table.  This is
            : used to step through the hash table to visit every entry.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  initialize_hash_pointer( ptr )
    hash_table_pointer  *ptr;
{
    ptr->current_index = -1;
    ptr->current_entry = (hash_entry_struct *) 0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_next_hash_entry
@INPUT      : hash_table
            : ptr
@OUTPUT     : data_ptr
@RETURNS    : TRUE if one is found
@DESCRIPTION: Steps to the next entry in the hash table, updating the ptr,
            : and passing back the data ptr stored in that entry.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :                      David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean  get_next_hash_entry( hash_table, ptr, data_ptr )
    hash_table_struct   *hash_table;
    hash_table_pointer  *ptr;
    void                **data_ptr;
{
    Boolean   found;

    if( ptr->current_entry != (hash_entry_struct *) 0 )
        ptr->current_entry = ptr->current_entry->next;

    if( ptr->current_entry == (hash_entry_struct *) 0 )
    {
        do
        {
            ++ptr->current_index;
        }
        while( ptr->current_index < hash_table->size &&
               hash_table->table[ptr->current_index] == (hash_entry_struct *)0);

        if( ptr->current_index < hash_table->size )
            ptr->current_entry = hash_table->table[ptr->current_index];
    }

    found = (ptr->current_entry != (hash_entry_struct *) 0);

    if( found )
        *data_ptr = ptr->current_entry->data_ptr;

    return( found );
}
