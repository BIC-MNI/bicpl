#!/usr/local/bin/perl

    $f = shift;
    $i = ! $f;

    @types = ( "unsigned char",
               "unsigned short",
               "unsigned long",
               "signed char",
               "signed short",
               "signed long",
               "float",
               "double" );

    $f || print( "    {\n" );
    $f || print( "         BOOLEAN  one_slice;\n" );
    $f || print( "         one_slice = ( n_slices1 == 1 && weights1[0] == 1.0 &&\n" .
           "         (volume_data2 == (void *) NULL ||\n" .
           "         n_slices2 == 1 && weights2[0] == 1.0) );\n" .
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

      $f || print( "$ind1  case ${caps}:\n" );
      $i || print( "#define TYPE1 $type1\n\n" );

      foreach $n_volumes ( 1, 2 )
      {
        if( $f && $n_volumes == 2 )
          { print( "#define TWO_VOLUMES\n\n" ); }

        $ind2 = $ind1 . "  ";
        if( $n_volumes == 1 )
        {
          @types2 = ( 1 );
          $f || print( "$ind2  if( volume_data2 == NULL )\n" .
                 "$ind2  {\n" ); 
          $ind3 = $ind2;
        }
        else
        {
          @types2 = @types;
          $f || print( "$ind2  }\n" .
                 "$ind2  else\n" .
                 "$ind2  {\n" ); 
          $f || print( "$ind2    switch( volume2_type )\n" .
                 "$ind2    {\n" );
          $ind3 = $ind2 . "  ";
        }

        foreach $type2 ( @types2 )
        {
          if( $n_volumes == 2 )
          {
            $type_name2 = $type2;
            $type_name2 =~ s/\s/_/g;
            $caps = $type_name2;
            $caps =~ tr/a-z/A-Z/;
            $caps =~ s/CHAR/BYTE/g;
            $f || print( "$ind3  case ${caps}:\n" );
            $i || print( "#define TYPE2 $type2\n\n" );
          }

          $ind4 = $ind3 . "  ";
          foreach $slice ( "one", "many" )
          {
            if( $slice eq "one" )
            {
              $f || print( "$ind4  if( one_slice )\n" .
                     "$ind4  { \n" );
              $i || print( "#define ONE_SLICE\n\n" );
            }
            else
            {
              $f || print( "$ind4  }\n" .
                     "$ind4  else\n" .
                     "$ind4  {\n" );
            }

            $ind5 = $ind4 . "  ";
            foreach $cmap ( "rgb", "cmap" )
            {
              if( $cmap eq "rgb" )
              {
                $f || print( "$ind5  if( pixels->pixel_type == RGB_PIXEL )\n" .
                       "$ind5  { \n" );
              }
              else
              {
                $f || print( "$ind5  }\n" .
                       "$ind5  else\n" .
                       "$ind5  { \n" );
                $i || print( "#define COLOUR_MAP\n\n" );
              }

              if( $n_volumes == 1 && $slice eq "one" &&
                  $cmap eq "rgb" && $type1 eq "unsigned long" )
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
                  $f || print( "$ind6  if( rgb_colour_map == NULL )\n" .
                         "$ind6  { \n" );
                  $ind7 = $ind6 . "  ";
                  $i || print( "#define NO_COLOUR_TABLE\n\n" );
                }
                elsif( @tables == 2 )
                {
                  $f || print( "$ind6  }\n" .
                         "$ind6  else\n" .
                         "$ind6  {\n" );
                  $ind7 = $ind6 . "  ";
                }

                $func = "render_${slice}_${cmap}_${type_name1}";

                if( $n_volumes == 2 )
                    { $func = $func . "_$type_name2"; }

                if( $table eq "direct" )
                    { $func .= "_direct"; }

                if( $f )
                {
                    print( "private  void $func\n" );
                    print( "#include \"rend_f_include.c\"\n\n" );
                }

                $f || print( "$ind7  $func(\n" );
                $f || print( "$ind7       volume_data1,\n" );
                $f || print( "$ind7       y,\n" );
                $f || print( "$ind7       start_x,\n" );
                $f || print( "$ind7       end_x,\n" );
                $f || print( "$ind7       y_offsets1,\n" );
                $f || print( "$ind7       row_offsets1,\n" );
                if( $slice eq "many" )
                {
                    $f || print( "$ind7       start_slices1,\n" );
                    $f || print( "$ind7       n_slices1,\n" );
                    $f || print( "$ind7       weights1,\n" );
                }
                if( $n_volumes == 2 )
                {
                    $f || print( "$ind7       volume_data2,\n" );
                    $f || print( "$ind7       y_offsets2,\n" );
                    $f || print( "$ind7       row_offsets2,\n" );
                    if( $slice eq "many" )
                    {
                        $f || print( "$ind7       start_slices2,\n" );
                        $f || print( "$ind7       n_slices2,\n" );
                        $f || print( "$ind7       weights2,\n" );
                    }
                }

                if( $cmap eq "cmap" )
                {
                    { $f || print( "$ind7       cmode_colour_map,\n" ); }
                    $f || print( "$ind7       cmap_pixel_ptr\n" );
                }
                else
                {
                    if( $table eq "table" )
                        { $f || print( "$ind7       rgb_colour_map,\n" ); }
                    $f || print( "$ind7       rgb_pixel_ptr\n" );
                }

                $f || print( "$ind7       );\n" );

                if( @tables == 2 && $table eq "table" )
                    { $f || print( "$ind6  }\n" ); }
                if( $table eq "direct" )
                    { $i || print( "#undef NO_COLOUR_TABLE\n\n" ); }
              }

              if( $cmap eq "cmap" )
                  { $f || print( "$ind5  }\n" ); }
              if( $cmap eq "cmap" )
                  { $i || print( "#undef COLOUR_MAP\n\n" ); }
            }

            if( $slice eq "many" )
                { $f || print( "$ind4  }\n"  ); }
            if( $slice eq "one" )
                { $i || print( "#undef ONE_SLICE\n\n" ); }
          }

          if( $n_volumes == 2 )
              { $f || print( "$ind3    break;\n" ); }
          if( $n_volumes == 2 )
              { $i || print( "#undef TYPE2\n\n" ); }
        }

        if( $n_volumes == 2 )
        {
          $f || print( "$ind2    }\n" .
                       "$ind2  }\n" );
          $i || print( "#undef  TWO_VOLUMES\n\n" );
        }
      }
      $f || print( "$ind1   break;\n" );
      $i || print( "#undef TYPE1\n\n" );
    }

    $f || print( "        }\n" );
    $f || print( "    }\n" );
