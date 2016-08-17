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
#ADD_EXECUTABLE(render_speed render_speed.c)

ADD_TEST(NAME bicpl_rgb_io COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_rgb_io image.rgb out.rgb)
ADD_TEST(NAME bicpl_globals COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_globals)
ADD_TEST(NAME bicpl_hash COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_hash 250000 0 0.25 0.4)
ADD_TEST(NAME bicpl_skip COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_hash 250000 1 0.25 0.4)
#ADD_TEST(NAME render_speed COMMAND ${CMAKE_CURRENT_BINARY_DIR}/render_speed)

#TARGET_LINK_LIBRARIES(test_xfm ${VOLUME_IO_LIBRARY} ${LIBMINC_LIBRARIES})

#TODO: addd more tests?
