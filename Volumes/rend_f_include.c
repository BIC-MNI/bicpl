#undef  TWO
#ifdef TWO_VOLUMES
#define  TWO( x )  x
#else
#define  TWO( x )
#endif

    /* TWO    if needed
       TYPE1, TYPE2 */

(
    void            *volume_data1,
    int             y,
    int             start_x,
    int             end_x,
    int             **y_offsets1,
    int             **row_offsets1,
#ifndef  ONE_SLICE
    void            **start_slices1,
    int             n_slices1,
    Real            weights1[],
#endif
#ifdef  TWO_VOLUMES
    void            *volume_data2,
    int             **y_offsets2,
    int             **row_offsets2,
#ifndef  ONE_SLICE
    void            **start_slices2,
    int             n_slices2,
    Real            weights2[],
#endif
#endif
#ifdef COLOUR_MAP
    unsigned short  **cmode_colour_map,
    unsigned short  *pixel_ptr
#else
#ifndef  NO_COLOUR_TABLE
    Colour          **rgb_colour_map,
#endif
    Colour          *pixel_ptr
#endif
)

{
    int            x;
#ifdef  ONE_SLICE
    TYPE1          *ptr1;
    int            *offset_ptr1;
#ifdef  TWO_VOLUMES
    TYPE2          *ptr2;
    int            *offset_ptr2;
#endif
#else
    int            s;
    TYPE1          **start_data1;
    Real           real_voxel_data1;
#ifdef TWO_VOLUMES
    TYPE2          **start_data2;
    Real           real_voxel_data2;
#endif
#endif
#ifndef  TWO_VOLUMES
#ifdef  COLOUR_MAP
    unsigned short   *single_cmode_map = cmode_colour_map[0];
#else
#ifndef  NO_COLOUR_TABLE
    Colour           *single_rgb_map = rgb_colour_map[0];
#endif
#endif
#endif

#ifndef ONE_SLICE
    start_data1 = (TYPE1 **) start_slices1;
    TWO( start_data2 = (TYPE2 **) start_slices2; )
#endif

#ifdef ONE_SLICE
    ptr1 = &((TYPE1 *) volume_data1) [y_offsets1[0][y]];
    offset_ptr1 = &row_offsets1[0][start_x];
    TWO( ptr2 = &((TYPE2 *) volume_data2) [y_offsets2[0][y]]; )
    TWO( offset_ptr2 = &row_offsets2[0][start_x]; )
#else
    for_less( s, 0, n_slices1 )
        start_data1[s] = &((TYPE1 *) volume_data1) [y_offsets1[s][y]];
#ifdef  TWO_VOLUMES
    for_less( s, 0, n_slices2 )
        start_data2[s] = &((TYPE2 *) volume_data2) [y_offsets2[s][y]];
#endif
#endif

    for_inclusive( x, start_x, end_x )
    {
#ifdef ONE_SLICE
#ifdef TWO_VOLUMES
#ifdef COLOUR_MAP
        *pixel_ptr++ = cmode_colour_map[(int) ptr1[*offset_ptr1++]]
                                       [(int) ptr2[*offset_ptr2++]];
#else
        *pixel_ptr++ = rgb_colour_map[(int) ptr1[*offset_ptr1++]]
                                     [(int) ptr2[*offset_ptr2++]];
#endif
#else
#ifdef COLOUR_MAP
        *pixel_ptr++ = single_cmode_map[(int) ptr1[*offset_ptr1++]];
#else
#ifdef  NO_COLOUR_TABLE
        *pixel_ptr++ = ptr1[*offset_ptr1++];
#else
        *pixel_ptr++ = single_rgb_map[(int) ptr1[*offset_ptr1++]];
#endif
#endif 
#endif /* TWO_VOLUMES */
#else
        real_voxel_data1 = 0.5;
        for_less( s, 0, n_slices1 )
        {
            real_voxel_data1 += weights1[s] *
                                start_data1[s][row_offsets1[s][x]];
        }

#ifdef  TWO_VOLUMES
        real_voxel_data2 = 0.5;
        for_less( s, 0, n_slices2 )
        {
            real_voxel_data2 += weights2[s] *
                                start_data2[s][row_offsets2[s][x]];
        }

#ifdef COLOUR_MAP
        *pixel_ptr++ = cmode_colour_map[(int) real_voxel_data1]
                                       [(int) real_voxel_data2];
#else
        *pixel_ptr++ = rgb_colour_map[(int) real_voxel_data1]
                                     [(int) real_voxel_data2];
#endif
#else

#ifdef COLOUR_MAP
        *pixel_ptr++ = single_cmode_map[(int) real_voxel_data1];
#else
        *pixel_ptr++ = single_rgb_map[(int) real_voxel_data1];
#endif
#endif /* TWO_VOLUMES */
#endif /* ONE_SLICE */
    }
}
