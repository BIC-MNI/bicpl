# When installed, bicpl.h is installed into $prefix/install, while the
# rest are installed into $prefix/include/bicpl.  This allows the library user
# to code "#include <bicpl.h>", giving "-I$prefix/install" on the command line
# to the compiler.

# For the above scheme to work while *building* the library, lots of
# the headers in this directory have to be available as <bicpl/foo.h>,
# so we make a link.  This is a terrible hack, since it introduces
# changes in the SOURCE hierarchy during the build.  The better
# solution is to put these files into an actual bicpl directory.
#
all-local:
	test -e $(srcdir)/bicpl || $(LN_S) . $(srcdir)/bicpl

include_HEADERS = bicpl.h 
pkginclude_HEADERS = \
	amoeba.h \
	bintree.h \
	bitlist.h \
	colour_coding.h \
	colours.h \
	compute_xfm.h \
	data_structures.h \
	ds_prototypes.h \
	geom.h \
	geom_prototypes.h \
	global_lookup.h \
	hash.h \
	histogram.h \
	image_prototypes.h \
	images.h \
	minimization.h \
	numeric_prototypes.h \
	numerical.h \
	obj_defs.h \
	object_prototypes.h \
	objects.h \
	priority_queue.h \
	prog_prototypes.h \
	prog_utils.h \
	queue.h \
	skiplist.h \
	stack.h \
	statistics.h \
	trans.h \
	trans_prototypes.h \
	vol_prototypes.h \
	vols.h


noinst_HEADERS = \
	compute_tps.h \
	deform.h \
	global_variables.h \
	globals.h \
	march_prototypes.h \
	marching.h \
	marching_cube_data.h \
	marching_cubes.h \
	matrix_basics.h \
	module.h \
	procrustes.h \
	rotmat_to_ang.h \
	splines.h \
	surface_rep.h
