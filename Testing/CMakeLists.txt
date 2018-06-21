INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../Include/bicpl" "${CMAKE_CURRENT_SOURCE_DIR}")
LINK_LIBRARIES(bicpl ${VOLUME_IO_LIBRARIES} ${LIBMINC_LIBRARIES} )

ADD_EXECUTABLE(test_rgb_io test_rgb_io.c)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/image.rgb" "${CMAKE_CURRENT_BINARY_DIR}/image.rgb" COPYONLY)
ADD_EXECUTABLE(test_globals test_globals.c)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/globals.input" "${CMAKE_CURRENT_BINARY_DIR}/globals.input" COPYONLY)
#ADD_EXECUTABLE(test_volume test_volume.c)
#ADD_EXECUTABLE(test_splines test_splines.c)
#ADD_EXECUTABLE(test_splines test_splines.c)
#ADD_EXECUTABLE(test_render test_render.c)
ADD_EXECUTABLE(test_hash test_hash.c)
ADD_EXECUTABLE(test_obj_io test_obj_io.c)
#ADD_EXECUTABLE(render_speed render_speed.c)
ADD_EXECUTABLE(test_procrustes test_procrustes.c)

ADD_TEST(NAME bicpl_rgb_io COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_rgb_io image.rgb out.rgb)
ADD_TEST(NAME bicpl_globals COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_globals)
ADD_TEST(NAME bicpl_hash COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_hash 250000 0 0.25 0.4)
ADD_TEST(NAME bicpl_skip COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_hash 250000 1 0.25 0.4)
ADD_TEST(NAME bicpl_procrustes COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_procrustes)



ADD_TEST(NAME bicpl_polygons_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/sixteen.obj)
SET_TESTS_PROPERTIES(bicpl_polygons_read PROPERTIES PASS_REGULAR_EXPRESSION
  "15: POLYGONS n_items 384 n_points 194 6 6 0 ffffffff")

ADD_TEST(NAME bicpl_pixels_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/pixels.obj.gz)
SET_TESTS_PROPERTIES(bicpl_pixels_read PROPERTIES PASS_REGULAR_EXPRESSION
  "0: PIXELS type 0 position 0 0 size 512 512 zoom 1 1 psum 62732504")

ADD_TEST(NAME bicpl_dft_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/brainsuite.dft)
SET_TESTS_PROPERTIES(bicpl_dft_read PROPERTIES PASS_REGULAR_EXPRESSION
  "0: LINES n_items 4552 n_points 1838336 87.7011 129.012 144.883 6346e0ff")

ADD_TEST(NAME bicpl_dfs_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/brainsuite.dfs)
SET_TESTS_PROPERTIES(bicpl_dfs_read PROPERTIES PASS_REGULAR_EXPRESSION
  "0: POLYGONS n_items 425780 n_points 212890 83.206 116.037 149.019 7f0000ff")

ADD_TEST(NAME bicpl_trk_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/tracks300.trk)
SET_TESTS_PROPERTIES(bicpl_trk_read PROPERTIES PASS_REGULAR_EXPRESSION
  "0: LINES n_items 300 n_points 14576 88.4266 108.729 82.4062 ffffffff")

ADD_TEST(NAME bicpl_ascii_ply_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/ascii.ply)
SET_TESTS_PROPERTIES(bicpl_ascii_ply_read PROPERTIES PASS_REGULAR_EXPRESSION
  "0: POLYGONS n_items 12 n_points 20 0 0 0 ffffffff")

ADD_TEST(NAME bicpl_lsb_ply_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/little-endian.ply)
SET_TESTS_PROPERTIES(bicpl_lsb_ply_read PROPERTIES PASS_REGULAR_EXPRESSION
  "0: POLYGONS n_items 151937 n_points 76270 17.3857 -39.0798 1344.98 2f3430ff")

ADD_TEST(NAME bicpl_msb_ply_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/big-endian.ply)
SET_TESTS_PROPERTIES(bicpl_msb_ply_read PROPERTIES PASS_REGULAR_EXPRESSION
  "0: POLYGONS n_items 364980 n_points 182488 0.240775 -11.4108 -2.34549 ffffffff")

ADD_TEST(NAME bicpl_freesurfer_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/lh.pial)
SET_TESTS_PROPERTIES(bicpl_freesurfer_read PROPERTIES PASS_REGULAR_EXPRESSION
"0: POLYGONS n_items 266798 n_points 133401 -26.9819 -19.7331 15.375 ffffffff")

ADD_TEST(NAME bicpl_wavefront_read COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/cessna.obj)
SET_TESTS_PROPERTIES(bicpl_wavefront_read PROPERTIES PASS_REGULAR_EXPRESSION
"0: POLYGONS n_items 3897 n_points 3745 -5.53869 -2.94724 0.0254704 ffffffff")

ADD_TEST(NAME bicpl_wavefront_read_empty COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_obj_io ${CMAKE_CURRENT_SOURCE_DIR}/data/empty.obj)
SET_TESTS_PROPERTIES(bicpl_wavefront_read_empty PROPERTIES PASS_REGULAR_EXPRESSION
"n_objects: 0")

#ADD_TEST(NAME render_speed COMMAND ${CMAKE_CURRENT_BINARY_DIR}/render_speed)

#TARGET_LINK_LIBRARIES(test_xfm ${VOLUME_IO_LIBRARY} ${LIBMINC_LIBRARIES})

#TODO: addd more tests?
