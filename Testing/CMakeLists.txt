LINK_LIBRARIES(bicpl ${VOLUME_IO_LIBRARIES} ${MINC2_LIBRARIES} )

ADD_EXECUTABLE(test_rgb_io test_rgb_io.c)

#TODO: addd more tests?
