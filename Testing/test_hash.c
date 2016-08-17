#include  <internal_volume_io.h>
#include  <bicpl.h>
#include  <assert.h>

#define  N_DATA 100000000
#define  N_HASH 10000

/* copied from the source file */
#define  MAX_SKIP_LEVELS   50
static  VIO_BOOL  test_skiplist_integrity(
    skiplist_struct  *skiplist )
{
    int            i;
    typedef  struct
    {
      skip_struct   *update[MAX_SKIP_LEVELS];
    } update_struct;
    update_struct  update;
    VIO_BOOL        okay;

    for_less( i, 0, skiplist->level )
        update.update[i] = skiplist->header->forward[i];

    while( update.update[0] != NULL )
    {
        i = 1;
        while( i < skiplist->level && update.update[0] == update.update[i] )
        {
            update.update[i] = update.update[i]->forward[i];
            ++i;
        }
        update.update[0] = update.update[0]->forward[0];
    }

    okay = TRUE;

    for_less( i, 0, skiplist->level )
        if( update.update[i] != NULL )
            okay = FALSE;

    return( okay );
}

int  main(
    int   argc,
    char  *argv[] )
{
    int                i, ind, n_iters, method;
    VIO_Real               density, chance_of_delete;
    VIO_BOOL            hashing, removing;
    hash_table_struct  hash_table;
    skiplist_struct    skiplist;
    void               *ptr;
    int                n_removed = 0;
    int                n_added = 0;
    int                n_test = 0;

    initialize_argument_processing( argc, argv );

    (void) get_int_argument( 10000, &n_iters );
    (void) get_int_argument( 0, &method );
    (void) get_real_argument( 0.25, &density );
    (void) get_real_argument( 0.0, &chance_of_delete );

    hashing = method == 0;

    if( hashing )
    {
        initialize_hash_table( &hash_table, N_HASH, sizeof(int),
                               2.0 * density, density );
        assert(hash_table.n_entries == 0);

        ind = get_random_int( N_DATA );
        assert( !lookup_in_hash_table( &hash_table, ind, (void *) &ind ) );
    }
    else
    {
        initialize_skiplist( &skiplist );

        ind = get_random_int( N_DATA );
        assert( !search_skiplist( &skiplist, ind, &ptr ) );
    }

    for_less( i, 0, n_iters )
    {
        ind = get_random_int( N_DATA );
        removing = get_random_0_to_1() < chance_of_delete;
        if( hashing )
        {
            if( !lookup_in_hash_table( &hash_table, ind, (void *) &ind ) )
            {
                if( removing )
                {
                    assert( !remove_from_hash_table( &hash_table, ind, &ind ) );
                }
                else
                {
                    insert_in_hash_table( &hash_table, ind, &ind );
                    n_added++;
                }
            }
            else
            {
                if ( removing )
                {
                    assert( remove_from_hash_table( &hash_table, ind, &ind ) );
                    n_removed++;
                    assert(!lookup_in_hash_table( &hash_table, ind, (void *) &ind ));
                }
            }

            assert(n_added - n_removed == hash_table.n_entries);
        }
        else
        {
            if( !search_skiplist( &skiplist, ind, &ptr ) )
            {
                if( removing )
                {
                    /* delete item even though it isn't there! */
                    assert( !delete_from_skiplist( &skiplist, ind, &ptr ) );
                }
                else
                {
                    assert(insert_in_skiplist( &skiplist, ind, NULL ) );
                    n_added++;
                }
            }
            else
            {
                if ( removing )
                {
                    assert(delete_from_skiplist( &skiplist, ind, &ptr ));
                    n_removed++;
                    assert(!search_skiplist( &skiplist, ind, &ptr ));
                    if (get_random_0_to_1() < 0.1)
                    {
                        assert(test_skiplist_integrity(&skiplist));
                        n_test++;
                    }
                }
            }
        }
    }

    if( hashing )
        delete_hash_table( &hash_table );
    else
        delete_skiplist( &skiplist );

    printf("No errors, added %d, removed %d.\n", n_added, n_removed);
    printf("Integrity tests: %d\n", n_test);
    return( 0 );
}
