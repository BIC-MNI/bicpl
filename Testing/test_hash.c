#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

#define  N_DATA 1000000

int  main(
    int   argc,
    char  *argv[] )
{
    int                i, ind, n_iters, method;
    Real               density, chance_of_delete;
    BOOLEAN            hashing, removing;
    hash_table_struct  hash_table;
    skiplist_struct    skiplist;
    void               *ptr;

    initialize_argument_processing( argc, argv );

    (void) get_int_argument( 10000, &n_iters );
    (void) get_int_argument( 0, &method );
    (void) get_real_argument( 0.25, &density );
    (void) get_real_argument( 0.0, &chance_of_delete );

    hashing = method == 0;

    if( hashing )
        initialize_hash_table( &hash_table, 10000, sizeof(int),
                               2.0 * density, density );
    else
        initialize_skiplist( &skiplist );

    for_less( i, 0, n_iters )
    {
        ind = get_random_int( N_DATA );
        removing = get_random_0_to_1() < chance_of_delete;
        if( hashing )
        {
            if( !lookup_in_hash_table( &hash_table, ind, (void *) &ind ) )
            {
                if( removing )
                    remove_from_hash_table( &hash_table, ind, &ind );
                else
                    insert_in_hash_table( &hash_table, ind, &ind );
            }
        }
        else
        {
            if( !search_skiplist( &skiplist, ind, &ptr ) )
            {
                if( removing )
                    (void) delete_from_skiplist( &skiplist, ind, &ptr );
                else
                    insert_in_skiplist( &skiplist, ind, NULL );
            }
        }
    }

    if( hashing )
        delete_hash_table( &hash_table );
    else
        delete_skiplist( &skiplist );

    return( 0 );
}
