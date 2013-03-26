#ifndef  DEF_ds_prototypes
#define  DEF_ds_prototypes

#ifdef __cplusplus
extern "C" {
#endif

BICAPI  void  initialize_bintree(
    VIO_Real                 x_min,
    VIO_Real                 x_max,
    VIO_Real                 y_min,
    VIO_Real                 y_max,
    VIO_Real                 z_min,
    VIO_Real                 z_max,
    bintree_struct_ptr   bintree );

BICAPI  void  delete_bintree_node(
    bintree_node_struct   *node );

BICAPI  void  delete_bintree(
    bintree_struct_ptr   bintree );

BICAPI  void  get_bintree_limits(
    bintree_struct_ptr    bintree,
    range_struct          *limits );

BICAPI  bintree_node_struct  *create_bintree_internal_node(
    int                   split_coord,
    VIO_Real                  split_position,
    bintree_node_struct   *left,
    bintree_node_struct   *right );

BICAPI  bintree_node_struct  *create_bintree_leaf(
    VIO_Real                  split_position,
    int                   n_objects,
    int                   object_list[] );

BICAPI  VIO_BOOL  bintree_node_is_leaf(
    bintree_node_struct  *node );

BICAPI  int  get_bintree_leaf_objects(
    bintree_node_struct  *node,
    int                  *object_list[] );

BICAPI  VIO_BOOL  get_bintree_left_child_ptr(
    bintree_node_struct  *node,
    bintree_node_struct  ***ptr_to_left_child );

BICAPI  VIO_BOOL  get_bintree_left_child(
    bintree_node_struct  *node,
    bintree_node_struct  **left_child );

BICAPI  VIO_BOOL  get_bintree_right_child_ptr(
    bintree_node_struct  *node,
    bintree_node_struct  ***ptr_to_right_child );

BICAPI  VIO_BOOL  get_bintree_right_child(
    bintree_node_struct  *node,
    bintree_node_struct  **right_child );

BICAPI  int  get_node_split_axis(
    bintree_node_struct  *node );

BICAPI  VIO_Real  get_node_split_position(
    bintree_node_struct  *node );

BICAPI  VIO_BOOL  point_within_range(
    VIO_Point         *point,
    range_struct  *range );

BICAPI  VIO_Real  range_volume(
    range_struct  *range );

BICAPI  VIO_Real  range_surface_area(
    range_struct  *range );

BICAPI  VIO_Status  io_bintree(
    FILE                 *file,
    VIO_IO_types             direction,
    VIO_File_formats         format,
    bintree_struct_ptr   bintree );

BICAPI  void  create_bitlist(
    int             n_bits,
    bitlist_struct  *bitlist );

BICAPI  void  zero_bitlist(
    bitlist_struct  *bitlist );

BICAPI  void  fill_bitlist(
    bitlist_struct  *bitlist );

BICAPI  void     set_bitlist_bit(
    bitlist_struct  *bitlist,
    int             bit_index,
    VIO_BOOL         value );

BICAPI  VIO_BOOL     get_bitlist_bit(
    bitlist_struct  *bitlist,
    int             bit_index );

BICAPI  void  delete_bitlist(
    bitlist_struct  *bitlist );

BICAPI  void  create_bitlist_3d(
    int                nx,
    int                ny,
    int                nz,
    bitlist_3d_struct  *bitlist );

BICAPI  void  zero_bitlist_3d(
    bitlist_3d_struct  *bitlist );

BICAPI  void  get_bitlist_3d_sizes(
    bitlist_3d_struct  *bitlist,
    int                *nx,
    int                *ny,
    int                *nz );

BICAPI  void  fill_bitlist_3d(
    bitlist_3d_struct  *bitlist );

BICAPI  void     set_bitlist_bit_3d(
    bitlist_3d_struct  *bitlist,
    int                x,
    int                y,
    int                z,
    VIO_BOOL            value );

BICAPI  VIO_BOOL     get_bitlist_bit_3d(
    bitlist_3d_struct  *bitlist,
    int                x,
    int                y,
    int                z );

BICAPI  void  delete_bitlist_3d(
    bitlist_3d_struct  *bitlist );

BICAPI  VIO_Status  io_bitlist_3d(
    FILE               *file,
    VIO_IO_types           io_type,
    bitlist_3d_struct  *bitlist );

BICAPI  void  create_object_bintree(
    int                  n_objects,
    range_struct         bound_vols[],
    bintree_struct_ptr   bintree,
    int                  max_nodes );

BICAPI  void  evaluate_bintree_efficiency(
    bintree_struct_ptr   bintree,
    VIO_Real                 *avg_nodes_visited,
    VIO_Real                 *avg_objects_visited );

BICAPI   void  initialize_hash_table(
    hash_table_struct  *hash_table,
    int                size,
    int                data_size,
    VIO_Real               enlarge_threshold,
    VIO_Real               new_density );

BICAPI   void  delete_hash_table(
    hash_table_struct  *hash_table );

BICAPI  void  insert_in_hash_table(
    hash_table_struct  *hash_table,
    int                key,
    void               *data_ptr );

BICAPI  VIO_BOOL  lookup_in_hash_table(
    hash_table_struct  *hash_table,
    int                key,
    void               *data_ptr );

BICAPI  VIO_BOOL  remove_from_hash_table(
    hash_table_struct  *hash_table,
    int                key,
    void               *data_ptr );

BICAPI   void  increase_hash_table_size(
    hash_table_struct   *hash_table,
    int                 new_size );

BICAPI  void  initialize_hash_pointer(
    hash_table_pointer  *ptr );

BICAPI  VIO_BOOL  get_next_hash_entry(
    hash_table_struct   *hash_table,
    hash_table_pointer  *ptr,
    void                *data_ptr );

BICAPI   void  initialize_hash2_table(
    hash2_table_struct  *hash_table,
    int                 size,
    int                 data_size,
    VIO_Real                enlarge_threshold,
    VIO_Real                new_density );

BICAPI   void  delete_hash2_table(
    hash2_table_struct  *hash_table );

BICAPI  void  insert_in_hash2_table(
    hash2_table_struct  *hash_table,
    int                 key1,
    int                 key2,
    void                *data_ptr );

BICAPI  VIO_BOOL  lookup_in_hash2_table(
    hash2_table_struct  *hash_table,
    int                 key1,
    int                 key2,
    void                *data_ptr );

BICAPI  VIO_BOOL  remove_from_hash2_table(
    hash2_table_struct  *hash_table,
    int                 key1,
    int                 key2,
    void                *data_ptr );

BICAPI   void  increase_hash2_table_size(
    hash2_table_struct   *hash_table,
    int                  new_size );

BICAPI  void  initialize_hash2_pointer(
    hash2_table_pointer  *ptr );

BICAPI  VIO_BOOL  get_next_hash2_entry(
    hash2_table_struct   *hash_table,
    hash2_table_pointer  *ptr,
    void                 *data_ptr );

BICAPI  void  delete_the_bintree(
    bintree_struct_ptr  *bintree );

BICAPI  bintree_struct_ptr allocate_bintree( void );

BICAPI  void  create_lines_bintree(
    lines_struct   *lines,
    int            max_nodes );

BICAPI  void  create_polygons_bintree(
    polygons_struct   *polygons,
    int               max_nodes );

BICAPI  void  create_quadmesh_bintree(
    quadmesh_struct   *quadmesh,
    int               max_nodes );

BICAPI  VIO_Real  find_closest_point_in_bintree(
    VIO_Point               *point,
    bintree_struct_ptr  bintree,
    object_struct       *object,
    int                 *obj_index,
    VIO_Point               *point_on_object );

BICAPI  VIO_Real  find_closest_vertex_in_bintree(
    VIO_Point               *point,
    bintree_struct_ptr  bintree,
    object_struct       *object,
    int                 *vertex_on_object );

BICAPI  void  print_bintree_stats(
    int   n_objects );

BICAPI  int  intersect_ray_with_bintree(
    VIO_Point               *origin,
    VIO_Vector              *direction,
    bintree_struct_ptr  bintree,
    object_struct       *object,
    int                 *obj_index,
    VIO_Real                *dist,
    VIO_Real                *distances[] );

BICAPI  VIO_BOOL  ray_intersects_range(
    range_struct  *range,
    VIO_Point         *origin,
    VIO_Vector        *direction,
    VIO_Real          *t_min,
    VIO_Real          *t_max );

BICAPI   void  initialize_skiplist(
    skiplist_struct  *skiplist );

BICAPI   void  delete_skiplist(
    skiplist_struct  *skiplist );

BICAPI  VIO_BOOL  search_skiplist(
    skiplist_struct          *skiplist,
    float                    key,
    void                     **data_ptr );

BICAPI  VIO_BOOL  search_skiplist_and_return_pointer(
    skiplist_struct          *skiplist,
    float                    key,
    skip_struct              **entry_ptr,
    void                     **data_ptr );

BICAPI  VIO_BOOL  insert_in_skiplist(
    skiplist_struct          *skiplist,
    float                    key,
    void                     *data_ptr );

BICAPI  VIO_BOOL  delete_from_skiplist(
    skiplist_struct  *skiplist,
    float            key,
    void             **data_ptr );

BICAPI  VIO_BOOL  get_first_skiplist_entry(
    skiplist_struct   *skiplist,
    skip_struct       **entry_ptr,
    float             *key,
    void              **data_ptr );

BICAPI  VIO_BOOL  get_next_skiplist_entry(
    skip_struct       **entry_ptr,
    float             *key,
    void              **data_ptr );


#ifdef __cplusplus
}
#endif

#endif
