#!/usr/local/bin/perl

    @types = ( "unsigned char",
               "unsigned short",
               "unsigned long",
               "signed char",
               "signed short",
               "signed long",
               "float",
               "double" );

    print( "    {\n" );
    print( "         BOOLEAN  one_slice;\n" );
    print( "         one_slice = ( n_slices1 == 1 && weights1[0] == 1.0 &&\n" .
           "         (volume_data2 == (void *) NULL ||\n" .
           "         n_slices2 == 1 && weights2[0] == 1.0) )\n" .
           "\n" .
           "         switch( volume1_type )\n" .
           "         {\n" );

    $ind1 = "         ";

    foreach $type1 ( @types )
    {
      $type_name1 = $type1;
      $type_name1 =~ s/\s/_/g;
      $caps = $type_name1;
      $caps =~ tr/a-z/A-Z/;
      $caps =~ s/CHAR/BYTE/g;

      print( "$ind1  case ${caps}:\n" );

      foreach $n_volumes ( 1, 2 )
      {
        $ind2 = $ind1 . "  ";
        if( $n_volumes == 1 )
        {
          @types2 = ( 1 );
          print( "$ind2  if( volume2_data == NULL )\n" .
                 "$ind2  {\n" ); 
          $ind3 = $ind2;
        }
        else
        {
          @types2 = @types;
          print( "$ind2  }\n" .
                 "$ind2  else\n" .
                 "$ind2  {\n" ); 
          print( "$ind2    switch( volume2_type )\n" .
                 "$ind2    {\n" );
          $ind3 = $ind2 . "  ";
        }

        foreach $type2 ( @types2 )
        {
          if( $n_volumes == 2 )
          {
            $type_name2 = $type2;
            $type_name2 =~ s/\s/_/g;
            $func = $func . "_$type_name2";
            $caps = $type_name2;
            $caps =~ tr/a-z/A-Z/;
            $caps =~ s/CHAR/BYTE/g;
            print( "$ind3  case ${caps}:\n" );
          }

          $ind4 = $ind3 . "  ";
          foreach $slice ( "one", "many" )
          {
            if( $slice eq "one" )
            {
              print( "$ind4  if( one_slice )\n" .
                     "$ind4  { \n" );
            }
            else
            {
              print( "$ind4  }\n" .
                     "$ind4  else\n" .
                     "$ind4  {\n" );
            }

            $ind5 = $ind4 . "  ";
            foreach $cmap ( "rgb", "cmap" )
            {
              if( $cmap eq "rgb" )
              {
                print( "$ind5  if( pixels->pixel_type == RGB_PIXEL )\n" .
                       "$ind5  { \n" );
              }
              else
              {
                print( "$ind5  }\n" .
                       "$ind5  else\n" .
                       "$ind5  { \n" );
              }

              if( $n_volumes == 1 && $slice eq "one" &&
                  $cmap eq "rgb" )
              {
                @tables = ( "direct", "table" );
                $ind6 = $ind5 . "  ";
              }
              else
              {
                @tables = ( "table" );
                $ind6 = $ind5;
              }

              foreach $table ( @tables )
              {
                if( $table eq "direct" )
                {
                  print( "$ind6  if( rgb_colour_map == NULL )\n" .
                         "$ind6  { \n" );
                  $ind7 = $ind6 . "  ";
                }
                elsif( @tables == 2 )
                {
                  print( "$ind6  }\n" .
                         "$ind6  else\n" .
                         "$ind6  {\n" );
                  $ind7 = $ind6 . "  ";
                }

                $func = "render_${slice}_${cmap}_${type_name1}";

                if( $table eq "direct" )
                    { $func .= "_direct"; }

                print( "$ind7  $func(\n" );
                print( "$ind7       volume_data1,\n" );
                print( "$ind7       y,\n" );
                print( "$ind7       start_x,\n" );
                print( "$ind7       end_x,\n" );
                print( "$ind7       y_offsets1,\n" );
                print( "$ind7       row_offsets1,\n" );
                if( $slice eq "many" )
                {
                    print( "$ind7       start_slices1,\n" );
                    print( "$ind7       n_slices1,\n" );
                    print( "$ind7       weights1,\n" );
                }
                if( $n_volumes == 2 )
                {
                    print( "$ind7       volume_data2,\n" );
                    print( "$ind7       y_offsets2,\n" );
                    print( "$ind7       row_offsets2,\n" );
                    if( $slice eq "many" )
                    {
                        print( "$ind7       start_slices2,\n" );
                        print( "$ind7       n_slices2,\n" );
                        print( "$ind7       weights2,\n" );
                    }
                }

                if( $cmap eq "cmap" )
                {
                    if( $table eq "table" )
                        { print( "$ind7       cmode_colour_map,\n" ); }
                    print( "$ind7       cmap_pixel_ptr\n" );
                }
                else
                {
                    if( $table eq "table" )
                        { print( "$ind7       rgb_colour_map,\n" ); }
                    print( "$ind7       rgb_pixel_ptr\n" );
                }

                print( "$ind7       );\n" );

                if( @tables == 2 && $table eq "table" )
                    { print( "$ind6  }\n" ); }
              }

              if( $cmap eq "cmap" )
                  { print( "$ind5  }\n" ); }
            }

            if( $slice eq "many" )
                { print( "$ind4  }\n"  ); }
          }

          if( $n_volumes == 2 )
              { print( "$ind3    break;\n" ); }
        }

        if( $n_volumes == 2 )
        {
          print( "$ind2    }\n" .
                 "$ind2  }\n" );
        }
      }
      print( "$ind1   break;\n" );
    }

    print( "        }\n" );
    print( "    }\n" );
