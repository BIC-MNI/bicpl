# CMakeFiles.txt for the bicpl library
#
# Vladimir S. FONOV - vladimir.fonov@gmail.com

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(BICPL_PACKAGE_VERSION_MAJOR 2)
SET(BICPL_PACKAGE_VERSION_MINOR 0)
SET(BICPL_PACKAGE_VERSION_PATCH 0)

# Packaging defines
IF(NOT MINC_TOOLKIT_BUILD)
  PROJECT(bicpl)
  SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
  ENABLE_TESTING()
  INCLUDE(CTest)
  SET(CPACK_GENERATOR TGZ)
  SET(CPACK_PACKAGE_VERSION_MAJOR ${BICPL_PACKAGE_VERSION_MAJOR})
  SET(CPACK_PACKAGE_VERSION_MINOR ${BICPL_PACKAGE_VERSION_MINOR})
  SET(CPACK_PACKAGE_VERSION_PATCH ${BICPL_PACKAGE_VERSION_PATCH})
  INCLUDE(CPack)
  
  FIND_PACKAGE(LIBMINC REQUIRED )
  FIND_PACKAGE(NETPBM QUIET)
  
  OPTION(BICPL_BUILD_SHARED_LIBS "Build shared libraries." OFF)
ENDIF(NOT MINC_TOOLKIT_BUILD)


SET(PACKAGE "bicpl")
SET(PACKAGE_BUGREPORT "a.janke@gmail.com")

SET(PACKAGE_NAME "bicpl")
SET(PACKAGE_VERSION "${BICPL_PACKAGE_VERSION_MAJOR}.${BICPL_PACKAGE_VERSION_MINOR}.${BICPL_PACKAGE_VERSION_PATCH}")
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")


INCLUDE( ${LIBMINC_USE_FILE} )

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# add for building relocatable library
IF(UNIX)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(UNIX)

set(CMAKE_MACOSX_RPATH ON)


# aliases
SET(VERSION "${PACKAGE_VERSION}")

# check for prereqs
INCLUDE(CheckFunctionExists)
INCLUDE(CheckIncludeFiles)

CHECK_FUNCTION_EXISTS(mkstemp  HAVE_MKSTEMP)
CHECK_FUNCTION_EXISTS(tmpnam   HAVE_TMPNAM)
CHECK_FUNCTION_EXISTS(tempnam  HAVE_TEMPNAM)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR)
CHECK_FUNCTION_EXISTS(popen    HAVE_POPEN)
CHECK_FUNCTION_EXISTS(fork     HAVE_FORK)
CHECK_FUNCTION_EXISTS(vfork    HAVE_VFORK)
CHECK_FUNCTION_EXISTS(fdopen   HAVE_FDOPEN)
CHECK_FUNCTION_EXISTS(strdup   HAVE_STRDUP)
CHECK_FUNCTION_EXISTS(getpwnam HAVE_GETPWNAM) 
CHECK_FUNCTION_EXISTS(select   HAVE_SELECT)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR) 
CHECK_FUNCTION_EXISTS(sysconf  HAVE_SYSCONF)
CHECK_FUNCTION_EXISTS(system   HAVE_SYSTEM)
CHECK_FUNCTION_EXISTS(cbrt     HAVE_CBRT)
CHECK_FUNCTION_EXISTS(gamma    HAVE_GAMMA)
CHECK_FUNCTION_EXISTS(random   HAVE_RANDOM)
CHECK_FUNCTION_EXISTS(srandom  HAVE_SRANDOM)

CHECK_INCLUDE_FILES(float.h     HAVE_FLOAT_H)
CHECK_INCLUDE_FILES(sys/dir.h   HAVE_SYS_DIR_H)
CHECK_INCLUDE_FILES(sys/ndir.h  HAVE_SYS_NDIR_H)
CHECK_INCLUDE_FILES(sys/stat.h  HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(sys/wait.h  HAVE_SYS_WAIT_H)
CHECK_INCLUDE_FILES(values.h    HAVE_VALUES_H)
CHECK_INCLUDE_FILES(unistd.h    HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(dirent.h    HAVE_DIRENT_H)
CHECK_INCLUDE_FILES(memory.h    HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(stdlib.h    HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(fcntl.h     HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(vfork.h     HAVE_VFORK_H)
CHECK_INCLUDE_FILES(inttypes.h  HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(string.h    HAVE_STRING_H)
CHECK_INCLUDE_FILES(strings.h   HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(pwd.h       HAVE_PWD_H)

IF(NETPBM_FOUND)
  SET(HAVE_LIBNETPBM ON)
  SET(imageio_GLUE_CODE "rgb_io_ppm.c")
  SET(BICPL_LIBRARIES bicpl ${NETPBM_LIBRARY})
  INCLUDE_DIRECTORIES(${NETPBM_INCLUDE_DIR} )
ELSE(NETPBM_FOUND)
  SET(HAVE_LIBNETPBM OFF)
  SET(imageio_GLUE_CODE "rgb_io_none.c")#still going to produce ppm files
  SET(BICPL_LIBRARIES bicpl )
ENDIF(NETPBM_FOUND)

# config files for build
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

# others
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/epm-header.in ${CMAKE_CURRENT_BINARY_DIR}/epm-header)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Include )

SET(bicpl_HEADERS
  Include/bicpl/amoeba.h 
  Include/bicpl/bintree.h 
  Include/bicpl/bitlist.h 
  Include/bicpl/colour_coding.h 
  Include/bicpl/colours.h 
  Include/bicpl/compute_xfm.h 
  Include/bicpl/data_structures.h 
  Include/bicpl/deform.h 
  Include/bicpl/deform_prototypes.h 
  Include/bicpl/ds_prototypes.h 
  Include/bicpl/geom.h 
  Include/bicpl/geom_prototypes.h 
  Include/bicpl/global_lookup.h 
  Include/bicpl/globals.h 
  Include/bicpl/hash.h 
  Include/bicpl/histogram.h 
  Include/bicpl/image_prototypes.h 
  Include/bicpl/images.h 
  Include/bicpl/march_prototypes.h 
  Include/bicpl/marching.h 
  Include/bicpl/marching_cubes.h 
  Include/bicpl/marching_cube_data.h 
  Include/bicpl/minimization.h 
  Include/bicpl/numeric_prototypes.h 
  Include/bicpl/numerical.h 
  Include/bicpl/obj_defs.h 
  Include/bicpl/object_prototypes.h 
  Include/bicpl/objects.h 
  Include/bicpl/priority_queue.h 
  Include/bicpl/prog_prototypes.h 
  Include/bicpl/prog_utils.h 
  Include/bicpl/queue.h 
  Include/bicpl/skiplist.h 
  Include/bicpl/splines.h 
  Include/bicpl/stack.h 
  Include/bicpl/statistics.h 
  Include/bicpl/trans.h 
  Include/bicpl/trans_prototypes.h 
  Include/bicpl/vol_prototypes.h 
  Include/bicpl/vols.h
)

# some variables
SET(bicpl_LIB_SRCS
  ${bicpl_HEADERS}
  version.c 
  Objects/coalesce.c 
  Objects/colours.c 
  Objects/graphics_io.c 
  Objects/freesurfer_io.c 
  Objects/brainsuite_io.c
  Objects/landmark_file.c 
  Objects/lines.c 
  Objects/markers.c 
  Objects/models.c 
  Objects/object_io.c 
  Objects/objects.c 
  Objects/pixels.c 
  Objects/poly_neighs.c 
  Objects/polygons.c 
  Objects/quadmesh.c 
  Objects/rgb_lookup.c 
  Objects/tag_objects.c 
  Objects/text.c 
  Objects/texture_values.c
  Data_structures/bintree.c 
  Data_structures/bitlist.c 
  Data_structures/build_bintree.c 
  Data_structures/hash_table.c 
  Data_structures/hash2_table.c 
  Data_structures/object_bintrees.c 
  Data_structures/point_bintree.c 
  Data_structures/ray_bintree.c 
  Data_structures/skiplist.c
  Images/crop_image.c Images/${imageio_GLUE_CODE}
  Numerical/amoeba.c 
  Numerical/gaussian.c 
  Numerical/gradient_minimize.c 
  Numerical/histogram.c 
  Numerical/least_squares.c 
  Numerical/matrix_svd.c 
  Numerical/minimize_lsq.c 
  Numerical/minimize_lsq_float.c 
  Numerical/numerical.c 
  Numerical/quadratic.c 
  Numerical/real_quadratic.c 
  Numerical/statistics.c 
  Numerical/t_stat.c
  Prog_utils/arguments.c 
  Prog_utils/globals.c 
  Prog_utils/random.c 
  Prog_utils/time.c  
  Volumes/box_filter.c 
  Volumes/change_labels.c 
  Volumes/colour_coding.c 
  Volumes/col_code_points.c 
  Volumes/col_code_io.c 
  Volumes/create_slice.c 
  Volumes/crop_volume.c 
  Volumes/dilate.c 
  Volumes/filters.c 
  Volumes/fill_volume.c 
  Volumes/interpolate.c 
  Volumes/input.c 
  Volumes/labels.c 
  Volumes/mapping.c 
  Volumes/output_free.c 
  Volumes/render.c 
  Volumes/rend_f.c 
  Volumes/resample.c 
  Volumes/scan_lines.c 
  Volumes/scan_markers.c 
  Volumes/scan_objects.c 
  Volumes/scan_polygons.c 
  Volumes/smooth.c 
  Volumes/talairach.c
  Geometry/approx_sqrt.c 
  Geometry/clip_3d.c 
  Geometry/closest_point.c 
  Geometry/curvature.c 
  Geometry/flatten.c 
  Geometry/geodesic_distance.c 
  Geometry/geometry.c 
  Geometry/intersect.c 
  Geometry/line_circle.c 
  Geometry/map_polygons.c 
  Geometry/path_surface.c 
  Geometry/platonics.c 
  Geometry/plane_polygon_intersect.c 
  Geometry/points.c 
  Geometry/poly_dist.c 
  Geometry/polygon_sphere.c 
  Geometry/ray_intersect.c 
  Geometry/segment_polygons.c 
  Geometry/smooth_curvature.c 
  Geometry/smooth_lines.c 
  Geometry/smooth_polygons.c 
  Geometry/solve_plane.c 
  Geometry/subdivide_lines.c 
  Geometry/subdivide_polygons.c 
  Geometry/surface_area.c 
  Geometry/tetrahedrons.c 
  Geometry/tubes.c 
  Geometry/volume_slice.c
  Marching_cubes/isosurfaces.c 
  Marching_cubes/marching_cubes.c 
  Marching_cubes/marching_no_holes.c 
  Marching_cubes/marching_tetra.c
  Transforms/compute_tps.c 
  Transforms/compute_xfm.c 
  Transforms/matrix_basics.c 
  Transforms/optimize.c 
  Transforms/procrustes.c 
  Transforms/rotmat_to_ang.c 
  Transforms/safe_compute_xfm.c 
  Transforms/transforms.c 
  Transforms/transform_io.c
  Deform/search_utils.c 
  Deform/intersect_voxel.c 
  Deform/model_objects.c 
  Deform/deform_line.c 
  Deform/find_in_direction.c
  Deform/models.c
  Deform/find_in_direction.c
  bicpl_clapack/blaswrap.h
  bicpl_clapack/f2c.h
  bicpl_clapack/dbdsqr.c 
  bicpl_clapack/dcopy.c 
  bicpl_clapack/dgebd2.c 
  bicpl_clapack/dgebrd.c 
  bicpl_clapack/dgelq2.c 
  bicpl_clapack/dgelqf.c 
  bicpl_clapack/dgemm.c 
  bicpl_clapack/dgemv.c 
  bicpl_clapack/dgeqr2.c 
  bicpl_clapack/dgeqrf.c 
  bicpl_clapack/dger.c 
  bicpl_clapack/dgesvd.c 
  bicpl_clapack/dlabrd.c 
  bicpl_clapack/dlacpy.c 
  bicpl_clapack/dlamch.c 
  bicpl_clapack/dlange.c 
  bicpl_clapack/dlapy2.c 
  bicpl_clapack/dlarfb.c 
  bicpl_clapack/dlarf.c 
  bicpl_clapack/dlarfg.c 
  bicpl_clapack/dlarft.c 
  bicpl_clapack/dlartg.c 
  bicpl_clapack/dlas2.c 
  bicpl_clapack/dlascl.c 
  bicpl_clapack/dlaset.c 
  bicpl_clapack/dlasq1.c 
  bicpl_clapack/dlasq2.c 
  bicpl_clapack/dlasq3.c 
  bicpl_clapack/dlasq4.c 
  bicpl_clapack/dlasq5.c 
  bicpl_clapack/dlasq6.c 
  bicpl_clapack/dlasr.c 
  bicpl_clapack/dlasrt.c 
  bicpl_clapack/dlassq.c 
  bicpl_clapack/dlasv2.c 
  bicpl_clapack/dnrm2.c 
  bicpl_clapack/dorg2r.c 
  bicpl_clapack/dorgbr.c 
  bicpl_clapack/dorgl2.c 
  bicpl_clapack/dorglq.c 
  bicpl_clapack/dorgqr.c 
  bicpl_clapack/dorm2r.c 
  bicpl_clapack/dormbr.c 
  bicpl_clapack/dorml2.c 
  bicpl_clapack/dormlq.c 
  bicpl_clapack/dormqr.c 
  bicpl_clapack/drot.c 
  bicpl_clapack/dscal.c 
  bicpl_clapack/d_sign.c 
  bicpl_clapack/dswap.c 
  bicpl_clapack/dtrmm.c 
  bicpl_clapack/dtrmv.c 
  bicpl_clapack/exit_.c 
  bicpl_clapack/F77_aloc.c 
  bicpl_clapack/ieeeck.c 
  bicpl_clapack/ilaenv.c 
  bicpl_clapack/lsame.c 
  bicpl_clapack/pow_dd.c 
  bicpl_clapack/pow_di.c 
  bicpl_clapack/s_cat.c 
  bicpl_clapack/s_cmp.c 
  bicpl_clapack/s_copy.c 
  bicpl_clapack/xerbla.c 
)

IF(BICPL_BUILD_SHARED_LIBS)
  SET(LIBRARY_TYPE SHARED)
  SET(LIBRARY_INSTALL LIBRARY)
ELSE(BICPL_BUILD_SHARED_LIBS)
  SET(LIBRARY_TYPE STATIC)
  SET(LIBRARY_INSTALL ARCHIVE)
ENDIF(BICPL_BUILD_SHARED_LIBS)


ADD_LIBRARY(bicpl ${LIBRARY_TYPE} ${bicpl_LIB_SRCS})
TARGET_LINK_LIBRARIES(bicpl ${VOLUME_IO_LIBRARIES} ${LIBMINC_LIBRARIES} )

IF(NETPBM_FOUND)
  TARGET_LINK_LIBRARIES(bicpl ${NETPBM_LIBRARY} )
ENDIF(NETPBM_FOUND)

IF(BICPL_BUILD_SHARED_LIBS)
  ADD_LIBRARY(bicpl_static STATIC ${bicpl_LIB_SRCS})
  TARGET_LINK_LIBRARIES(bicpl_static ${VOLUME_IO_STATIC_LIBRARIES} ${LIBMINC_STATIC_LIBRARIES}  )
  IF(NETPBM_FOUND)
    TARGET_LINK_LIBRARIES(bicpl_static ${NETPBM_LIBRARY} )
  ENDIF(NETPBM_FOUND)
ENDIF(BICPL_BUILD_SHARED_LIBS)

ADD_SUBDIRECTORY( tools )

INSTALL(FILES Include/bicpl.h
   DESTINATION include)

INSTALL(FILES ${bicpl_HEADERS}
   DESTINATION include/bicpl)

INSTALL(TARGETS bicpl ${LIBRARY_INSTALL} DESTINATION lib${LIB_SUFFIX})

set(BICPL_USE_FILE_CONFIG      "${CMAKE_CURRENT_BINARY_DIR}/UseBICPL.cmake")
set(BICPL_INCLUDE_DIRS_CONFIG   ${CMAKE_CURRENT_SOURCE_DIR}/Include ${NETPBM_INCLUDE_DIR})
set(BICPL_LIBRARY_DIRS_CONFIG   ${CMAKE_CURRENT_BINARY_DIR} )

set(BICPL_LIBRARIES_CONFIG bicpl)

IF(NETPBM_FOUND)
  set(BICPL_LIBRARIES_CONFIG ${BICPL_LIBRARIES_CONFIG} ${NETPBM_LIBRARY})
ENDIF  (NETPBM_FOUND)

IF(BICPL_BUILD_SHARED_LIBS)
  SET(BICPL_LIBRARIES_STATIC_CONFIG bicpl_static )
  SET(BICPL_STATIC_LIBRARIES bicpl_static ${NETPBM_LIBRARY})
  
  IF(NETPBM_FOUND)
    SET(BICPL_LIBRARIES_STATIC_CONFIG ${BICPL_LIBRARIES_STATIC_CONFIG} ${NETPBM_LIBRARY})
    SET(BICPL_STATIC_LIBRARIES ${BICPL_STATIC_LIBRARIES} ${NETPBM_LIBRARY})
  ENDIF(NETPBM_FOUND)
ENDIF(BICPL_BUILD_SHARED_LIBS)


configure_file(BICPLConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/BICPLConfig.cmake @ONLY )

configure_file(UseBICPL.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/UseBICPL.cmake @ONLY)

# config for install dir
set(BICPL_USE_FILE_CONFIG "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/UseBICPL.cmake")
set(BICPL_INCLUDE_DIRS_CONFIG  ${CMAKE_INSTALL_PREFIX}/include )

IF(NETPBM_FOUND)
  set(BICPL_INCLUDE_DIRS_CONFIG  ${BICPL_INCLUDE_DIRS_CONFIG} ${NETPBM_INCLUDE_DIR})
ENDIF(NETPBM_FOUND)

set(BICPL_LIBRARY_DIRS_CONFIG  ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX} )
set(BICPL_LIBRARIES_CONFIG     bicpl)

IF(NETPBM_FOUND)
  set(BICPL_LIBRARY_DIRS_CONFIG  ${BICPL_LIBRARY_DIRS_CONFIG} ${NETPBM_LIBRARY_DIR})
  set(BICPL_LIBRARIES_CONFIG     ${BICPL_LIBRARIES_CONFIG} ${NETPBM_LIBRARY})
ENDIF(NETPBM_FOUND)

set(BICPL_LIBRARIES_STATIC_CONFIG "") #don't install

IF(SUPERBUILD_STAGING_PREFIX)
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" BICPL_INCLUDE_DIRS_CONFIG "${BICPL_INCLUDE_DIRS_CONFIG}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" BICPL_LIBRARY_DIRS_CONFIG "${BICPL_LIBRARY_DIRS_CONFIG}")
  STRING(REPLACE "${SUPERBUILD_STAGING_PREFIX}/" "" BICPL_LIBRARIES_CONFIG    "${BICPL_LIBRARIES_CONFIG}")
ENDIF(SUPERBUILD_STAGING_PREFIX)


configure_file(BICPLConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/BICPLConfig.cmake @ONLY )

configure_file(UseBICPL.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/UseBICPL.cmake @ONLY)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/UseBICPL.cmake ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/BICPLConfig.cmake
  DESTINATION lib${LIB_SUFFIX}
  COMPONENT Development)

# testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY( Testing )
ENDIF(BUILD_TESTING)
