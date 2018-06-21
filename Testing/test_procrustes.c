#include  <volume_io.h>
#include  <bicpl.h>

VIO_Real margin = 0.005;

int main(
    int   argc,
    char  *argv[])
{

    //Check that procrustes does not fail on large point sets
    int         npoints = 1000000;
    int         ndim = 4;
    VIO_Real    **Apoints;
    VIO_Real    **Bpoints;    
    VIO_ALLOC2D( Apoints, npoints, ndim );
    VIO_ALLOC2D( Bpoints, npoints, ndim );
 

    set_random_seed( 12345 );

    //Create a set of points centered around the origin
    int dim, i;
    for_less( dim, 0, ndim )
    {
        for_less( i, 0, npoints )
        {
            Apoints[i][dim] = 2.0*get_random_0_to_1()-1.0;
        }
    }
       
    //Create a translation in x between 0 and 5
    VIO_Transform   tra0;
    VIO_Real        translateby=5*get_random_0_to_1();
    make_translation_transform( translateby, 0, 0, &tra0);
    
    //Create a rotation around z between 0 and PI
    VIO_Transform   rot0; 
    VIO_Vector      rot0_axis={0,0,0};
    VIO_Real        rot0_angle=get_random_0_to_1()*3.1415;
    make_rotation_transform(rot0_angle, VIO_Z, &rot0);

    //Scale in every dimension with a factor between 1 and 2
    VIO_Transform   sca0;
    VIO_Real        scale_factor=1.0+get_random_0_to_1();   
    make_scale_transform(scale_factor, scale_factor, scale_factor, &sca0);

    concat_transforms(&tra0, &rot0, &tra0);
    concat_transforms(&tra0, &tra0, &sca0);

    //Apply all transformations to point set A to obtain B
    for_less(i, 0, npoints)
    {
      transform_point(&tra0, Apoints[i][0], Apoints[i][1], Apoints[i][2], &Bpoints[i][0], &Bpoints[i][1], &Bpoints[i][2]); 
    }

    //Calculate procrustes transformation
    VIO_Real        tra[ndim];
    VIO_Real        crot[ndim];
    VIO_Transform   rot;
    VIO_Real        scale_ptr;
    procrustes( npoints, ndim, Apoints, Bpoints, tra, crot, &rot, &scale_ptr );
 
    VIO_FREE2D( Apoints );
    VIO_FREE2D( Bpoints );
    
    VIO_Status status = 0 ;
    
    //Check that the calculated center of rot. is the origin
    if( !within_margin(crot[0], 0.0) || !within_margin(crot[1], 0.0) || !within_margin(crot[2], 0.0) )
    {
      print_error("Estimated center of rotation is not correct ");
      print_error("(%f, %f, %f,) != (%f,%f,%f)\n", crot[0],crot[1],crot[2],0.0,0.0,0.0);
      status = 2;
    }

    //Check that the calculated rotation is correct
    VIO_Real angles[3];
    rotmat_to_ang(&rot, angles);
    if( !within_margin(angles[2], rot0_angle) || !within_margin(angles[1], 0.0) || !within_margin(angles[0], 0.0) )
    {
      print_error("Estimated rotation is not correct");
      print_error("(%f, %f, %f,) != (%f,%f,%f)\n", angles[2],angles[1],angles[0],rot0_angle,0.0,0.0);
      status = 2;
    }

    //Check that the scale is correct
    if( !within_margin(scale_ptr, 1.0/scale_factor) )
    {
      print_error("Estimated scale is not correct");
      print_error("%f != %f\n", scale_ptr, 1.0/scale_factor);
      status = 2;
    }

    //Check that the translation is correct
    if( !within_margin(tra[0], -tra0.m[3][0]) || !within_margin(tra[1], -tra0.m[3][1]) || !within_margin(tra[2], -tra0.m[3][2]) )
    {
      print_error("Estimated translation is not correct");
      print_error("(%f, %f, %f,) != (%f,%f,%f)\n", tra[0],tra[1],tra[2],-tra0.m[3][0],-tra0.m[3][1],-tra0.m[3][2]);
      status = 2;
    }
 
    return( status );

}


//Checks if two numbers are within margin of each other
int within_margin(VIO_Real a, VIO_Real b)
{
    if( (a-b)<= margin && (b-a)<=margin)
    {
        return( 1 );
    }
    return( 0 );    
}

