#!/usr/local/bin/perl

    @types = ( "unsigned char",
               "unsigned short",
               "unsigned long",
               "signed char",
               "signed short",
               "signed long",
               "float",
               "double" );

    foreach $slice ( "one", "many" )
    {
        if( $slice eq "one" )
            { print( "#define ONE_SLICE\n\n" ); }

        $func = "render_$slice";

        foreach $n_volumes ( 1, 2 )
        {
            if( $n_volumes == 2 )
                { print( "#define TWO_VOLUMES\n\n" ); }

            foreach $cmap ( "cmap", "rgb" )
            {
                if( $cmap eq "cmap" )
                    { print( "#define COLOUR_MAP\n\n" ); }

                $func = $func . "_$cmap";

                foreach $type1 ( @types )
                {
                    print( "#define TYPE1 $type1\n\n" );

                    $type_name1 = $type1;
                    $type_name1 =~ s/\s/_/g;

                    if( $n_volumes == 1 )
                        { @types2 = ( 1 ); }
                    else
                        { @types2 = @types; }

                    foreach $type2 ( @types2 )
                    {
                        if( $n_volumes == 2 )
                            { print( "#define TYPE2 $type2\n\n" ); }

                        $func = "render_${slice}_${cmap}_${type_name1}";

                        if( $n_volumes == 2 )
                        {
                            $type_name2 = $type2;
                            $type_name2 =~ s/\s/_/g;
                            $func = $func . "_$type_name2";
                        }

                        print( "private  void $func\n" );
                        print( "#include \"rend_f_include.c\"\n\n" );

                        if( $n_volumes == 2 )
                            { print( "#undef TYPE2\n\n" ); }
                    }

                    print( "#undef TYPE1\n\n" );
                }

                if( $cmap eq "cmap" )
                    { print( "#undef COLOUR_MAP\n\n" ); }
            }

            if( $n_volumes == 2 )
                { print( "#undef TWO_VOLUMES\n\n" ); }
        }

        if( $slice eq "one" )
            { print( "#undef ONE_SLICE\n\n" ); }
    }
