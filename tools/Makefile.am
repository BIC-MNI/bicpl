EXTRA_DIST = plytools-patch

INCLUDES = -I$(top_srcdir)/Include
LDADD = $(BIC_LIBS) ../libbicpl.la -lm

bin_PROGRAMS = bicobj2oogl bicobj2ply 

# this is not ready for release; use at your own risk
noinst_PROGRAMS = make_mesh_map

bicobj2oogl_SOURCES = bicobj2oogl.c
bicobj2ply_SOURCES = bicobj2ply.c ply.c ply.h
make_mesh_map_SOURCES = make_mesh_map.c ply.c ply.h

