INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../Include/bicpl" "${CMAKE_CURRENT_SOURCE_DIR}")
LINK_LIBRARIES(bicpl ${VOLUME_IO_LIBRARIES} ${LIBMINC_LIBRARIES} )

ADD_EXECUTABLE(test_rgb_io test_rgb_io.c)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/image.rgb" "${CMAKE_CURRENT_BINARY_DIR}/image.rgb" COPYONLY)
ADD_EXECUTABLE(test_globals test_globals.c)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/globals.input" "${CMAKE_CURRENT_BINARY_DIR}/globals.input" COPYONLY)
#ADD_EXECUTABLE(test_volume test_volume.c)
#ADD_EXECUTABLE(test_splines test_splines.c)
#ADD_EXECUTABLE(test_splines test_splines.c)

ADD_TEST(NAME test_rgb_io COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_rgb_io image.rgb out.rgb)
ADD_TEST(NAME test_globals COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_globals)
#TARGET_LINK_LIBRARIES(test_xfm ${VOLUME_IO_LIBRARY} ${LIBMINC_LIBRARIES})

#TODO: addd more tests?
