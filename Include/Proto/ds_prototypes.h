#ifndef  DEF_ds_prototypes
#define  DEF_ds_prototypes

public  void  initialize_bintree(
    VIO_Real                 x_min,
    VIO_Real                 x_max,
    VIO_Real                 y_min,
    VIO_Real                 y_max,
    VIO_Real                 z_min,
    VIO_Real                 z_max,
    bintree_struct_ptr   bintree );

public  void  delete_bintree_node(
    bintree_node_struct   *node );

public  void  delete_bintree(
    bintree_struct_ptr   bintree );

public  void  get_bintree_limits(
    bintree_struct_ptr    bintree,
    range_struct          *limits );

public  bintree_node_struct  *create_bintree_internal_node(
    int                   split_coord,
    VIO_Real                  split_position,
    bintree_node_struct   *left,
    bintree_node_struct   *right );

public  bintree_node_struct  *create_bintree_leaf(
    VIO_Real                  split_position,
    int                   n_objects,
    int                   object_list[] );

public  VIO_BOOL  bintree_node_is_leaf(
    bintree_node_struct  *node );

public  int  get_bintree_leaf_objects(
    bintree_node_struct  *node,
    int                  *object_list[] );

public  VIO_BOOL  get_bintree_left_child_ptr(
    bintree_node_struct  *node,
    bintree_node_struct  ***ptr_to_left_child );

public  VIO_BOOL  get_bintree_left_child(
    bintree_node_struct  *node,
    bintree_node_struct  **left_child );

public  VIO_BOOL  get_bintree_right_child_ptr(
    bintree_node_struct  *node,
    bintree_node_struct  ***ptr_to_right_child );

public  VIO_BOOL  get_bintree_right_child(
    bintree_node_struct  *node,
    bintree_node_struct  **right_child );

public  int  get_node_split_axis(
    bintree_node_struct  *node );

public  VIO_Real  get_node_split_position(
    bintree_node_struct  *node );

public  VIO_BOOL  point_within_range(
    Point         *point,
    range_struct  *range );

public  VIO_Real  range_volume(
    range_struct  *range );

public  VIO_Real  range_surface_area(
    range_struct  *range );

public  Status  io_bintree(
    FILE                 *file,
    IO_types             direction,
    File_formats         format,
    bintree_struct_ptr   bintree );

public  void  create_bitlist(
    int             n_bits,
    bitlist_struct  *bitlist );

public  void  zero_bitlist(
    bitlist_struct  *bitlist );

public  void  fill_bitlist(
    bitlist_struct  *bitlist );

public  void     set_bitlist_bit(
    bitlist_struct  *bitlist,
    int             bit_index,
    VIO_BOOL         value );

public  VIO_BOOL     get_bitlist_bit(
    bitlist_struct  *bitlist,
    int             bit_index );

public  void  delete_bitlist(
    bitlist_struct  *bitlist );

public  void  create_bitlist_3d(
    int                nx,
    int                ny,
    int                nz,
    bitlist_3d_struct  *bitlist );

public  void  zero_bitlist_3d(
    bitlist_3d_struct  *bitlist );

public  void  fill_bitlist_3d(
    bitlist_3d_struct  *bitlist );

public  void     set_bitlist_bit_3d(
    bitlist_3d_struct  *bitlist,
    int                x,
    int                y,
    int                z,
    VIO_BOOL            value );

public  VIO_BOOL     get_bitlist_bit_3d(
    bitlist_3d_struct  *bitlist,
    int                x,
    int                y,
    int                z );

public  void  delete_bitlist_3d(
    bitlist_3d_struct  *bitlist );

public  Status  io_bitlist_3d(
    FILE               *file,
    IO_types           io_type,
    bitlist_3d_struct  *bitlist );

public  void  create_object_bintree(
    int                  n_objects,
    range_struct         bound_vols[],
    bintree_struct_ptr   bintree,
    int                  max_nodes );

public  void  evaluate_bintree_efficiency(
    bintree_struct_ptr   bintree,
    VIO_Real                 *avg_nodes_visited,
    VIO_Real                 *avg_objects_visited );

public   void  initialize_hash_table(
    hash_table_struct  *hash_table,
    int                n_keys,
    int                size,
    VIO_Real               enlarge_threshold,
    VIO_Real               new_density );

public   void  delete_hash_table(
    hash_table_struct  *hash_table );

public  void  insert_in_hash_table(
    hash_table_struct  *hash_table,
    int                keys[],
    void               *data_ptr );

public  VIO_BOOL  lookup_in_hash_table(
    hash_table_struct  *hash_table,
    int                keys[],
    void               **data_ptr );

public  VIO_BOOL  remove_from_hash_table(
    hash_table_struct  *hash_table,
    int                keys[],
    void               **data_ptr );

public   void  increase_hash_table_size(
    hash_table_struct   *hash_table,
    int                 new_size );

public  void  initialize_hash_pointer(
    hash_table_pointer  *ptr );

public  VIO_BOOL  get_next_hash_entry(
    hash_table_struct   *hash_table,
    hash_table_pointer  *ptr,
    void                **data_ptr );

public  void  delete_the_bintree(
    bintree_struct_ptr  *bintree );

public  void  *allocate_bintree();

public  void  create_lines_bintree(
    lines_struct   *lines,
    int            max_nodes );

public  void  create_polygons_bintree(
    polygons_struct   *polygons,
    int               max_nodes );

public  void  create_quadmesh_bintree(
    quadmesh_struct   *quadmesh,
    int               max_nodes );

public  VIO_Real  find_closest_point_in_bintree(
    Point               *point,
    bintree_struct_ptr  bintree,
    object_struct       *object,
    int                 *obj_index,
    Point               *point_on_object );

public  void  print_bintree_stats(
    int   n_objects );

public  int  intersect_ray_with_bintree(
    Point               *origin,
    Vector              *direction,
    bintree_struct_ptr  bintree,
    object_struct       *object,
    int                 *obj_index,
    VIO_Real                *dist,
    VIO_Real                *distances[] );

public  VIO_BOOL  ray_intersects_range(
    range_struct  *range,
    Point         *origin,
    Vector        *direction,
    VIO_Real          *t_min,
    VIO_Real          *t_max );

public   void  initialize_skiplist(
    skiplist_struct  *skiplist,
    int              (*compare_function) ( void *, void * ) );

public   void  delete_skiplist(
    skiplist_struct  *skiplist );

public  VIO_BOOL  search_skiplist(
    skiplist_struct          *skiplist,
    void                     *key_data_ptr,
    void                     **data_ptr );

public  VIO_BOOL  insert_in_skiplist(
    skiplist_struct          *skiplist,
    void                     *data_ptr );

public  VIO_BOOL  delete_from_skiplist(
    skiplist_struct  *skiplist,
    void             *key_ptr,
    void             **data_ptr );

public  VIO_BOOL  get_first_skiplist_entry(
    skiplist_struct   *skiplist,
    skip_struct       **entry_ptr,
    void              **data_ptr );

public  VIO_BOOL  get_next_skiplist_entry(
    skip_struct       **entry_ptr,
    void              **data_ptr );
#endif
