EXTRA_DOX = groups.dox
EXTRA_DIST = Changes.list bicpl.tex frank.obj lines_format.txt \
	polygons_format.txt t1.obj t2.obj \
	doxygen.config.in $(EXTRA_DOX) $(tex_sources)

tex_sources = bicpl.tex

bicpl.dvi: $(tex_sources)
	latex bicpl.tex

bicpl.ps: bicpl.dvi
	dvips bicpl -o bicpl.ps

# Enable when doxygen is probed for in configure
#all-local: html/index.html

clean-local:
	rm -rf html man

# The dependencies are incomplete, since doc files depend on the
# source files.  Until that is fixed, do "make clean" before "make"
# if you want docs for updated sources.
html/index.html: doxygen.config $(EXTRA_DOX)
	doxygen doxygen.config

dox: html/index.html
