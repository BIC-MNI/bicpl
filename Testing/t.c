#include  <stdio.h>
#include  <malloc.h>

#undef  USE_LOOKUP
#define  USE_LOOKUP

#define  TYPE  double
#define  TYPE2  unsigned char

#define  MAX_TYPE  (1 << (8*sizeof(TYPE2)))

int  main(
    int  argc,
    char *argv[] )
{
    int              i, n, val, iter;
    TYPE             r[1000];
    TYPE             *lookup;
    TYPE2            v[1000], vi;

    (void) sscanf( argv[1], "%d", &val );

    n = 1000;

    lookup = malloc( MAX_TYPE * sizeof(TYPE) );

    for( i = 0;  i < MAX_TYPE;  ++i )
        lookup[vi] = (TYPE) vi;

    for( i = 0;  i < n;  ++i )
        v[i] = (TYPE2) val;

    (void) printf( "Initialized\n" );

    for( iter = 0;  iter < 100000;  ++iter )
    {
        if( v[n-10] == 0 )
            v[i] = 34;

        for( i = 0;  i < n;  ++i )
        {
#ifndef  USE_LOOKUP
            r[i] = (TYPE) v[i];
#else
            r[i] = lookup[v[i]];
#endif
        }
    }

    return( 0 );
}
