
#include  <def_mni.h>
#include  <def_arrays.h>
#include  <def_objects.h>

#define  INVALID_DISTANCE   -1

public   Status   find_path_between_polygons( polygon1, polygon2, n_polygons,
                                              end_indices, visibilities,
                                              neighbours,
                                              path_exists, path_length, path )
    int            polygon1;
    int            polygon2;
    int            n_polygons;
    int            end_indices[];
    Smallest_int   visibilities[];
    int            neighbours[];
    Boolean        *path_exists;
    int            *path_length;
    int            *path[];
{
    Status     status;
    Boolean    found_polygon1;
    int        start_index, end_index, neighbour;
    int        i, n, *distances, distance, curr_index, prev_index;
    int        n_step[2], n_step_alloced[2], *step_list[2];
    void       follow_path();

    ALLOC( status, distances, n_polygons );

    if( status == OK )
    {
        for_less( i, 0, n_polygons )
        {
            distances[i] = INVALID_DISTANCE;
        }

        n_step[0] = 0;
        n_step[1] = 0;
        n_step_alloced[0] = 0;
        n_step_alloced[1] = 0;
        prev_index = 0;
        curr_index = 1;

        ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status, n_step_alloced[curr_index],
                      n_step[curr_index], step_list[curr_index],
                      polygon2, DEFAULT_CHUNK_SIZE );
        distances[polygon2] = 0;

        distance = 0;

    }

    if( status == OK )
    {
        found_polygon1 = (polygon1 == polygon2);

        while( !found_polygon1 && n_step[curr_index] > 0 )
        {
            ++distance;
            curr_index = 1 - curr_index;
            prev_index = 1 - prev_index;

            n_step[curr_index] = 0;

            for_less( i, 0, n_step[prev_index] )
            {
                start_index = START_INDEX( end_indices,
                                           step_list[prev_index][i] );
                end_index = end_indices[step_list[prev_index][i]];

                for_less( n, start_index, end_index )
                {
                    neighbour = neighbours[n];

                    if( neighbour >= 0 &&
                        (visibilities == (Smallest_int *) 0 ||
                         visibilities[neighbour]) &&
                        distances[neighbour] == INVALID_DISTANCE )
                    {
                        if( neighbour == polygon1 )
                        {
                            found_polygon1 = TRUE;
                            break;
                        }
                        else
                        {
                            ADD_ELEMENT_TO_ARRAY_WITH_SIZE( status,
                              n_step_alloced[curr_index], n_step[curr_index],
                              step_list[curr_index],
                              neighbour, DEFAULT_CHUNK_SIZE );
                            distances[neighbour] = distance;
                        }
                    }
                }

                if( found_polygon1 )
                {
                    break;
                }
            }
        }
    }

    if( status == OK )
    {
        if( n_step_alloced[0] > 0 )
        {
            FREE( status, step_list[0] );
        }
    }

    if( status == OK )
    {
        if( n_step_alloced[1] > 0 )
        {
            FREE( status, step_list[1] );
        }
    }

    if( status == OK && found_polygon1 )
    {
        *path_exists = TRUE;

        *path_length = distance + 1;

        ALLOC( status, *path, *path_length );

        if( status == OK )
        {
            follow_path( polygon1, polygon2, end_indices,
                         neighbours, distances, *path_length, *path );
        }
    }
    else
    {
        *path_exists = FALSE;
    }

    if( status == OK )
    {
        FREE( status, distances );
    }

    return( status );
}

private  void  follow_path( polygon1, polygon2, end_indices,
                            neighbours, distances, path_length, path )
    int   polygon1, polygon2;
    int   end_indices[];
    int   neighbours[];
    int   distances[];
    int   path_length;
    int   path[];
{
    int   n, start_index, end_index, path_index, neighbour, current_polygon;

    path[0] = polygon1;
    path[path_length-1] = polygon2;

    current_polygon = polygon1;

    for_less( path_index, 1, path_length-1 )
    {
        start_index = START_INDEX( end_indices, current_polygon );
        end_index = end_indices[current_polygon];

        for_less( n, start_index, end_index )
        {
            neighbour = neighbours[n];

            if( neighbour >= 0 &&
                distances[neighbour] == path_length - path_index - 1 )
            {
                break;
            }
        }

        if( n == end_index )
        {
            HANDLE_INTERNAL_ERROR( "follow_path" );
        }

        current_polygon = neighbour;
        path[path_index] = current_polygon;
    }
}
