SUBDIRS = Include Deform Data_structures Transforms Geometry Numerical \
	Objects Prog_utils Volumes Marching_cubes Images bicpl_clapack . tools \
	Documentation Testing

AUTOMAKE_OPTIONS = check-news
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = README.release \
	mni_bicpl.epm.header \
	Makefile.msvc-win32 \
	config.h.msvc-win32 \
	$(m4_files)

m4_files = \
	m4/mni_REQUIRE_LIB.m4 \
	m4/mni_REQUIRE_MNILIBS.m4 \
	m4/smr_REQUIRED_LIB.m4 \
	m4/smr_WITH_BUILD_PATH.m4

# version info argument is CURRENT[:REVISION[:AGE]]
# see README.release for update instructions
# DO NOT SET THIS TO BE THE SAME AS THE PACKAGE VERSION!!!
libbicpl_la_LDFLAGS = -version-info 3:0:2

lib_LTLIBRARIES = libbicpl.la

libbicpl_la_SOURCES = version.c
libbicpl_la_LIBADD = \
	$(top_builddir)/Data_structures/libbicpl_ds.la \
	$(top_builddir)/Images/libbicpl_i.la \
	$(top_builddir)/Numerical/libbicpl_n.la \
	$(top_builddir)/Prog_utils/libbicpl_pu.la \
	$(top_builddir)/Volumes/libbicpl_v.la \
	$(top_builddir)/Geometry/libbicpl_g.la \
	$(top_builddir)/Marching_cubes/libbicpl_mc.la \
	$(top_builddir)/Objects/libbicpl_o.la \
	$(top_builddir)/Transforms/libbicpl_t.la \
	$(top_builddir)/Deform/libbicpl_deform.la \
	$(top_builddir)/bicpl_clapack/libbicpl_cla.la


