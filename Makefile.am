SUBDIRS = Include Data_structures Transforms Geometry Numerical Objects \
	Prog_utils Volumes Marching_cubes Images .

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
	$(top_builddir)/Transforms/libbicpl_t.la

