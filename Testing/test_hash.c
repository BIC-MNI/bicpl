#include  <internal_volume_io.h>
#include  <bicpl.h>

int  compare_function(
    void  *p1,
    void  *p2 )
{
    int    i1, i2;

    i1 = *((int *) p1);
    i2 = *((int *) p2);
    if( i1 < i2 )
        return( -1 );
    else if( i1 > i2 )
        return( 1 );
    else
        return( 0 );
}

#define  N_DATA 1000000

int  main(
    int   argc,
    char  *argv[] )
{
    int                i, ind, n_iters, method;
    int                *data;
    Real               density;
    BOOLEAN            hashing;
    hash_table_struct  hash_table;
    skiplist_struct    skiplist;
    void               *ptr;

    initialize_argument_processing( argc, argv );

    (void) get_int_argument( 10000, &n_iters );
    (void) get_int_argument( 0, &method );
    (void) get_real_argument( 0.25, &density );

    hashing = method == 0;

    if( hashing )
        initialize_hash_table( &hash_table, 1, 10000, 2.0 * density, density );
    else
        initialize_skiplist( &skiplist, compare_function );

    ALLOC( data, N_DATA );
    for_less( i, 0, N_DATA )
        data[i] = i;

    for_less( i, 0, n_iters )
    {
        ind = get_random_int( N_DATA );
        if( hashing )
        {
            if( !lookup_in_hash_table( &hash_table, &ind, &ptr ) )
                insert_in_hash_table( &hash_table, &ind, &i );
        }
        else
        {
            if( !search_skiplist( &skiplist, &ind, &ptr ) )
                insert_in_skiplist( &skiplist, &data[ind] );
        }
    }

    if( hashing )
        delete_hash_table( &hash_table );
    else
        delete_skiplist( &skiplist );

    return( 0 );
}
