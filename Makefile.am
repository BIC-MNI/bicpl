SUBDIRS = Include Data_structures Transforms Geometry Numerical Objects \
	Prog_utils Volumes Marching_cubes Images .

AUTOMAKE_OPTIONS = check-news

# version info argument is CURRENT[:REVISION[:AGE]]
# see below
libbicpl_la_LDFLAGS = -version-info 0:1

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


# The following is taken from the libtool documentation.

#   Here are a set of rules to help you update your library version
#information:
#
#  1. Start with version information of `0:0:0' for each libtool library.
#
#  2. Update the version information only immediately before a public
#     release of your software.  More frequent updates are unnecessary,
#     and only guarantee that the current interface number gets larger
#     faster.
#
#  3. If the library source code has changed at all since the last
#     update, then increment REVISION (`C:R:A' becomes `C:r+1:A').
#
#  4. If any interfaces have been added, removed, or changed since the
#     last update, increment CURRENT, and set REVISION to 0.
#
#  5. If any interfaces have been added since the last public release,
#     then increment AGE.
#
#  6. If any interfaces have been removed since the last public release,
#     then set AGE to 0.
