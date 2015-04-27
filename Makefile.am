AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS =
SUBDIRS= .

EXTRA_DIST = README.release \
	autogen.sh \
	Images/rgb_io_ppm.c Images/rgb_io_sgi.c Images/rgb_io_none.c

AM_CPPFLAGS = \
	-I$(top_srcdir)/Include

include_HEADERS = \
	Include/bicpl.h

include_bicpldir = $(includedir)/bicpl

include_bicpl_HEADERS = \
	Include/bicpl/amoeba.h \
	Include/bicpl/bintree.h \
	Include/bicpl/bitlist.h \
	Include/bicpl/colour_coding.h \
	Include/bicpl/colours.h \
	Include/bicpl/compute_xfm.h \
	Include/bicpl/data_structures.h \
	Include/bicpl/deform.h \
	Include/bicpl/deform_prototypes.h \
	Include/bicpl/ds_prototypes.h \
	Include/bicpl/geom.h \
	Include/bicpl/geom_prototypes.h \
	Include/bicpl/global_lookup.h \
	Include/bicpl/globals.h \
	Include/bicpl/hash.h \
	Include/bicpl/histogram.h \
	Include/bicpl/image_prototypes.h \
	Include/bicpl/images.h \
	Include/bicpl/march_prototypes.h \
	Include/bicpl/marching.h \
	Include/bicpl/marching_cubes.h \
	Include/bicpl/marching_cube_data.h \
	Include/bicpl/minimization.h \
	Include/bicpl/numeric_prototypes.h \
	Include/bicpl/numerical.h \
	Include/bicpl/obj_defs.h \
	Include/bicpl/object_prototypes.h \
	Include/bicpl/objects.h \
	Include/bicpl/priority_queue.h \
	Include/bicpl/prog_prototypes.h \
	Include/bicpl/prog_utils.h \
	Include/bicpl/queue.h \
	Include/bicpl/skiplist.h \
	Include/bicpl/splines.h \
	Include/bicpl/stack.h \
	Include/bicpl/statistics.h \
	Include/bicpl/trans.h \
	Include/bicpl/trans_prototypes.h \
	Include/bicpl/vol_prototypes.h \
	Include/bicpl/vols.h

noinst_HEADERS = \
	bicpl_clapack/blaswrap.h \
	bicpl_clapack/f2c.h \
	Include/bicpl_internal.h \
	Images/ppm/ppm.h \
	Numerical/minimize_lsq_include.c \
	Volumes/call_rend_f_include.c \
	Volumes/rend_f_include.c \
	Volumes/render_funcs_include.c \
	Volumes/render_include.c \
	Volumes/render_include2.c \
	Volumes/render_include3.c

# version info argument is CURRENT[:REVISION[:AGE]]
# see README.release for update instructions
# DO NOT SET THIS TO BE THE SAME AS THE PACKAGE VERSION!!!
libbicpl_la_LDFLAGS = -version-info 3:0:2

lib_LTLIBRARIES = libbicpl.la

libbicpl_la_SOURCES = version.c \
	bicpl_clapack/dbdsqr.c \
	bicpl_clapack/dcopy.c \
	bicpl_clapack/dgebd2.c \
	bicpl_clapack/dgebrd.c \
	bicpl_clapack/dgelq2.c \
	bicpl_clapack/dgelqf.c \
	bicpl_clapack/dgemm.c \
	bicpl_clapack/dgemv.c \
	bicpl_clapack/dgeqr2.c \
	bicpl_clapack/dgeqrf.c \
	bicpl_clapack/dger.c \
	bicpl_clapack/dgesvd.c \
	bicpl_clapack/dlabrd.c \
	bicpl_clapack/dlacpy.c \
	bicpl_clapack/dlamch.c \
	bicpl_clapack/dlange.c \
	bicpl_clapack/dlapy2.c \
	bicpl_clapack/dlarfb.c \
	bicpl_clapack/dlarf.c \
	bicpl_clapack/dlarfg.c \
	bicpl_clapack/dlarft.c \
	bicpl_clapack/dlartg.c \
	bicpl_clapack/dlas2.c \
	bicpl_clapack/dlascl.c \
	bicpl_clapack/dlaset.c \
	bicpl_clapack/dlasq1.c \
	bicpl_clapack/dlasq2.c \
	bicpl_clapack/dlasq3.c \
	bicpl_clapack/dlasq4.c \
	bicpl_clapack/dlasq5.c \
	bicpl_clapack/dlasq6.c \
	bicpl_clapack/dlasr.c \
	bicpl_clapack/dlasrt.c \
	bicpl_clapack/dlassq.c \
	bicpl_clapack/dlasv2.c \
	bicpl_clapack/dnrm2.c \
	bicpl_clapack/dorg2r.c \
	bicpl_clapack/dorgbr.c \
	bicpl_clapack/dorgl2.c \
	bicpl_clapack/dorglq.c \
	bicpl_clapack/dorgqr.c \
	bicpl_clapack/dorm2r.c \
	bicpl_clapack/dormbr.c \
	bicpl_clapack/dorml2.c \
	bicpl_clapack/dormlq.c \
	bicpl_clapack/dormqr.c \
	bicpl_clapack/drot.c \
	bicpl_clapack/dscal.c \
	bicpl_clapack/d_sign.c \
	bicpl_clapack/dswap.c \
	bicpl_clapack/dtrmm.c \
	bicpl_clapack/dtrmv.c \
	bicpl_clapack/exit_.c \
	bicpl_clapack/F77_aloc.c \
	bicpl_clapack/ieeeck.c \
	bicpl_clapack/ilaenv.c \
	bicpl_clapack/lsame.c \
	bicpl_clapack/pow_dd.c \
	bicpl_clapack/pow_di.c \
	bicpl_clapack/s_cat.c \
	bicpl_clapack/s_cmp.c \
	bicpl_clapack/s_copy.c \
	bicpl_clapack/xerbla.c \
	Data_structures/bintree.c \
	Data_structures/bitlist.c \
	Data_structures/build_bintree.c \
	Data_structures/hash_table.c \
	Data_structures/hash2_table.c \
	Data_structures/object_bintrees.c \
	Data_structures/point_bintree.c \
	Data_structures/ray_bintree.c \
	Data_structures/skiplist.c \
	Deform/find_in_direction.c \
	Deform/search_utils.c \
	Deform/intersect_voxel.c \
	Deform/model_objects.c \
	Deform/deform_line.c \
	Deform/models.c \
	Geometry/approx_sqrt.c \
	Geometry/clip_3d.c \
	Geometry/closest_point.c \
	Geometry/curvature.c \
	Geometry/flatten.c \
	Geometry/geodesic_distance.c \
	Geometry/geometry.c \
	Geometry/intersect.c \
	Geometry/line_circle.c \
	Geometry/map_polygons.c \
	Geometry/path_surface.c \
	Geometry/platonics.c \
	Geometry/plane_polygon_intersect.c \
	Geometry/points.c \
	Geometry/poly_dist.c \
	Geometry/polygon_sphere.c \
	Geometry/ray_intersect.c \
	Geometry/segment_polygons.c \
	Geometry/smooth_curvature.c \
	Geometry/smooth_lines.c \
	Geometry/smooth_polygons.c \
	Geometry/solve_plane.c \
	Geometry/subdivide_lines.c \
	Geometry/subdivide_polygons.c \
	Geometry/surface_area.c \
	Geometry/tetrahedrons.c \
	Geometry/tubes.c \
	Geometry/volume_slice.c \
	Images/crop_image.c \
	Images/rgb_io.c \
	Images/ppm/ppm.c \
	Marching_cubes/isosurfaces.c \
	Marching_cubes/marching_cubes.c \
	Marching_cubes/marching_no_holes.c \
	Marching_cubes/marching_tetra.c \
	Numerical/amoeba.c \
	Numerical/gaussian.c \
	Numerical/gradient_minimize.c \
	Numerical/histogram.c \
	Numerical/least_squares.c \
	Numerical/matrix_svd.c \
	Numerical/minimize_lsq.c \
	Numerical/minimize_lsq_float.c \
	Numerical/numerical.c \
	Numerical/quadratic.c \
	Numerical/real_quadratic.c \
	Numerical/statistics.c \
	Numerical/t_stat.c \
	Objects/coalesce.c \
	Objects/colours.c \
	Objects/graphics_io.c \
	Objects/landmark_file.c \
	Objects/lines.c \
	Objects/markers.c \
	Objects/models.c \
	Objects/object_io.c \
	Objects/objects.c \
	Objects/pixels.c \
	Objects/poly_neighs.c \
	Objects/polygons.c \
	Objects/quadmesh.c \
	Objects/rgb_lookup.c \
	Objects/tag_objects.c \
	Objects/text.c \
	Objects/texture_values.c \
	Prog_utils/arguments.c \
	Prog_utils/globals.c \
	Prog_utils/random.c \
	Prog_utils/time.c \
	Transforms/compute_tps.c \
	Transforms/compute_xfm.c \
	Transforms/matrix_basics.c \
	Transforms/optimize.c \
	Transforms/procrustes.c \
	Transforms/rotmat_to_ang.c \
	Transforms/safe_compute_xfm.c \
	Transforms/transforms.c \
	Transforms/transform_io.c \
	Volumes/box_filter.c \
	Volumes/change_labels.c \
	Volumes/colour_coding.c \
	Volumes/col_code_points.c \
	Volumes/col_code_io.c \
	Volumes/create_slice.c \
	Volumes/crop_volume.c \
	Volumes/dilate.c \
	Volumes/filters.c \
	Volumes/fill_volume.c \
	Volumes/interpolate.c \
	Volumes/input.c \
	Volumes/labels.c \
	Volumes/mapping.c \
	Volumes/output_free.c \
	Volumes/render.c \
	Volumes/rend_f.c \
	Volumes/resample.c \
	Volumes/scan_lines.c \
	Volumes/scan_markers.c \
	Volumes/scan_objects.c \
	Volumes/scan_polygons.c \
	Volumes/smooth.c \
	Volumes/talairach.c
