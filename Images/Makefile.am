AM_CPPFLAGS = -I$(top_srcdir)/Include $(imageio_CFLAGS)

noinst_LTLIBRARIES = libbicpl_i.la
libbicpl_i_la_SOURCES = crop_image.c rgb_io.c
libbicpl_i_la_LIBADD = $(imageio_LIBS)

EXTRA_libbicpl_i_la_SOURCES = rgb_io_ppm.c rgb_io_sgi.c rgb_io_none.c

