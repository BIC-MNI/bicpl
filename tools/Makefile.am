EXTRA_DIST = plytools-patch

INCLUDES = -I$(top_srcdir)/Include
LDADD = $(BIC_LIBS) ../libbicpl.la -lm

bin_PROGRAMS = ascii_binary bicobj2oogl 

# These two programs are not likely of general interest.
# Moreover, they won't compile on certain platforms, so
# I'm disabling them.

# noinst_PROGRAMS = bicobj2ply  make_mesh_map

bicobj2oogl_SOURCES = bicobj2oogl.c
#bicobj2ply_SOURCES = bicobj2ply.c ply.c ply.h
#make_mesh_map_SOURCES = make_mesh_map.c ply.c ply.h

