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
        {
            print( "    if( n_slices1 == 1 && weights1[0] == 1.0 &&\n" .
                   "        (volume_data2 == (void *) NULL ||\n" .
                   "        n_slices2 == 1 && weights2[0] == 1.0) )\n" .
                   "    { \n" );
        }
        else
        {
            print( "    }\n" .
                   "    else\n" .
                   "    { \n" );
        }

        foreach $n_volumes ( 1, 2 )
        {
            if( $n_volumes == 1 )
            {
                print( "      if( volume_data2 == NULL )\n" .
                       "      { \n" );
            }
            else
            {
                print( "      }\n" .
                       "      else\n" .
                       "      { \n" );
            }

            foreach $cmap ( "rgb", "cmap" )
            {
                if( $cmap eq "rgb" )
                {
                    print( "        if( pixels->pixel_type == RGB_PIXEL )\n" .
                           "        { \n" );
                }
                else
                {
                    print( "        }\n" .
                           "        else\n" .
                           "        { \n" );
                }

                print( "          switch( volume1_type )\n" .
                       "          {\n" );

                foreach $type1 ( @types )
                {
                    $type_name1 = $type1;
                    $type_name1 =~ s/\s/_/g;
                    $caps = $type_name1;
                    $caps =~ tr/a-z/A-Z/;
                    $caps =~ s/CHAR/BYTE/g;

                    print( "          case ${caps}:\n" );

                    if( $n_volumes == 1 )
                    {
                        @types2 = ( 1 );
                    }
                    else
                    {
                        @types2 = @types;
                        print( "            switch( volume2_type )\n" .
                               "            {\n" );
                    }

                    foreach $type2 ( @types2 )
                    {
                        $func = "render_${slice}_${cmap}_${type_name1}";

                        $indent = "           ";

                        if( $n_volumes == 2 )
                        {
                            $type_name2 = $type2;
                            $type_name2 =~ s/\s/_/g;
                            $func = $func . "_$type_name2";
                            $caps = $type_name2;
                            $caps =~ tr/a-z/A-Z/;
                            $caps =~ s/CHAR/BYTE/g;
                            print( "            case ${caps}:\n" );
                            $indent .= "  ";
                        }

                        print( "$indent $func(\n" );
                        print( "$indent       volume_data1,\n" );
                        print( "$indent       y,\n" );
                        print( "$indent       start_x,\n" );
                        print( "$indent       end_x,\n" );
                        print( "$indent       y_offsets1,\n" );
                        print( "$indent       row_offsets1,\n" );
                        if( $slice eq "many" )
                        {
                            print( "$indent       start_slices1,\n" );
                            print( "$indent       n_slices1,\n" );
                            print( "$indent       weights1,\n" );
                        }
                        if( $n_volumes == 2 )
                        {
                            print( "$indent       volume_data2,\n" );
                            print( "$indent       y_offsets2,\n" );
                            print( "$indent       row_offsets2,\n" );
                            if( $slice eq "many" )
                            {
                                print( "$indent       start_slices2,\n" );
                                print( "$indent       n_slices2,\n" );
                                print( "$indent       weights2,\n" );
                            }
                        }

                        if( $cmap eq "cmap" )
                        {
                            print( "$indent       cmode_colour_map,\n" );
                            print( "$indent       cmap_pixel_ptr\n" );
                        }
                        else
                        {
                            print( "$indent       rgb_colour_map,\n" );
                            print( "$indent       rgb_pixel_ptr\n" );
                        }

                        print( "$indent       );\n" );
                        if( $n_volumes == 2 )
                            { print( "              break;\n" ); }
                    }

                    if( $n_volumes == 2 )
                        { print( "          }\n" ); }
                    print( "            break;\n" );
                }

                print( "          }\n" );

                if( $cmap eq "cmap" )
                    { print( "        }\n" ); }
            }

            if( $n_volumes eq 2 )
                { print( "      }\n" ); }
        }

        if( $slice eq "many" )
            { print( "    }\n" ); }
    }
