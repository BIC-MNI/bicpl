EXTRA_DIST = plytools-patch

INCLUDES = -I$(top_srcdir)/Include
LDADD = $(BIC_LIBS) ../libbicpl.la -lm

bin_PROGRAMS = bicobj2oogl bicobj2ply

bicobj2oogl_SOURCES = bicobj2oogl.c
bicobj2ply_SOURCES = bicobj2ply.c ply.c ply.h

