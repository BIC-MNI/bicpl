#include  <internal_volume_io.h>
#include  <objects.h>

public  void  initialize_model(
    model_struct  *model )
{
    model->n_objects = 0;
    model->extra_ptr = (void *) NULL;
}

public  void  delete_model(
    model_struct   *model )
{
    int   i;

    for_less( i, 0, model->n_objects )
        delete_object( model->objects[i] );

    if( model->n_objects > 0 )
        FREE( model->objects );

    if( model->extra_ptr != (void *) NULL )
        FREE( model->extra_ptr );
}


public  void  assign_model_extra_ptr(
    model_struct   *model,
    void           *ptr )
{
    model->extra_ptr = ptr;
}

public  void  *get_model_extra_ptr(
    model_struct   *model )
{
    return( model->extra_ptr );
}

public  void  add_object_to_list(
    int            *n_objects,
    object_struct  ***object_list,
    object_struct  *new_object )
{
    ADD_ELEMENT_TO_ARRAY( *object_list, *n_objects, new_object, 1 );
}

public  void  remove_object_from_list(
    int            *n_objects,
    object_struct  **object_list[],
    int            obj_index )
{
    DELETE_ELEMENT_FROM_ARRAY( *object_list, *n_objects, obj_index, 1 );
}

public  void  add_object_to_model(
    model_struct   *model,
    object_struct  *new_object )
{
    add_object_to_list( &model->n_objects, &model->objects, new_object );
}

public  int  find_object_index_in_model(
    model_struct   *model,
    object_struct  *object )
{
    int            obj_index;

    for_less( obj_index, 0, model->n_objects )
    {
        if( model->objects[obj_index] == object )
            return( obj_index );
    }

    return( -1 );
}

public  void  remove_ith_object_from_model(
    model_struct   *model,
    int            obj_index )
{
    remove_object_from_list( &model->n_objects, &model->objects, obj_index );
}

public  void  remove_object_from_model(
    model_struct   *model,
    object_struct  *object )
{
    remove_ith_object_from_model( model,
                                  find_object_index_in_model(model,object) );
}

public  void  delete_object_list(
    int              n_objects,
    object_struct    *object_list[] )
{
    int      i;

    if( n_objects > 0 )
    {
        for_less( i, 0, n_objects )
        {
            delete_object( object_list[i] );
        }

        FREE( object_list );
    }
}
