DEFORM_LIB         = libdeform.a
DEFORM_LINT_LIB    = lint/llib-ldeform.ln
GEOMETRY_LIB       = libgeometry.a
GEOMETRY_LINT_LIB  = lint/llib-lgeometry.ln
MARCHING_LIB       = libmarching.a
MARCHING_LINT_LIB  = lint/llib-lmarching.ln
STATS_LIB          = libstatistics.a
STATS_LINT_LIB     = lint/llib-lstatistics.ln
SURFACE_LIB        = libsurface.a
SURFACE_LINT_LIB   = lint/llib-lsurface.ln

LIBS = $(DEFORM_LIB) \
       $(GEOMETRY_LIB) \
       $(MARCHING_LIB) \
       $(SURFACE_LIB)
LINT_LIBS = $(DEFORM_LINT_LIB) \
            $(GEOMETRY_LINT_LIB) \
            $(MARCHING_LINT_LIB) \
            $(SURFACE_LINT_LIB)

DEFORM_OBJ = \
             Deform/curvature_colour.o \
             Deform/deform_line.o \
             Deform/deform_polygons.o \
             Deform/find_in_direction.o \
             Deform/intersect_voxel.o \
             Deform/models.o \
             Deform/model_objects.o \
             Deform/search_utils.o

GEOMETRY_OBJ = Geometry/intersect.o \
               Geometry/line_circle.o \
               Geometry/path_surface.o \
               Geometry/polygon_sphere.o \
               Geometry/segment_polygons.o \
               Geometry/smooth_lines.o \
               Geometry/smooth_polygons.o \
               Geometry/subdivide_lines.o \
               Geometry/subdivide_polygons.o \
               Geometry/tetrahedrons.o

MARCHING_OBJ = \
               Marching_cubes/marching_cubes.o \
               Marching_cubes/marching_no_holes.o

SURFACE_OBJ = \
              Surface_rep/spline.o \
              Surface_rep/superquadric.o \
              Surface_rep/surface_reps.o

STATS_OBJ  = \
             Statistics/statistics.o

PROTOTYPE_FILE = Include/module_prototypes.h

LIBS = \
       $(DEFORM_LIB)   $(DEFORM_LINT_LIB) \
       $(GEOMETRY_LIB) $(GEOMETRY_LINT_LIB) \
       $(MARCHING_LIB) $(MARCHING_LINT_LIB) \
       $(SURFACE_LIB)  $(SURFACE_LINT_LIB) \
       $(STATS_LIB)    $(STATS_LINT_LIB)

all: $(PROTOTYPE_FILE) $(LIBS) $(LINT_LIBS)
	cd lint ; ln -s ../*.ln .

include $(C_DEV_DIRECTORY)/Make/Makefile.include

OPT = $(OPT_O)

OPT_g = -g
OPT_O = -O

INCLUDE = -I$(C_DEV_DIRECTORY)/Include -IInclude -I/usr/local/include

OBJECTS = $(DEFORM_OBJ) \
          $(GEOMETRY_OBJ) \
          $(MARCHING_OBJ) \
          $(STATS_OBJ) \
          $(SURFACE_OBJ)

$(PROTOTYPE_FILE): $(OBJECTS:.o=.c)
	@echo "#ifndef  DEF_MODULE_PROTOTYPES"              >  $@
	@echo "#define  DEF_MODULE_PROTOTYPES"              >> $@
	@extract_functions -public $(OBJECTS:.o=.c)         >> $@
	@echo "#endif"                                      >> $@

$(DEFORM_LIB): $(DEFORM_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(DEFORM_OBJ)

$(DEFORM_LINT_LIB): $(DEFORM_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o deform $(DEFORM_OBJ:.o=.ln)

$(GEOMETRY_LIB): $(GEOMETRY_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(GEOMETRY_OBJ)

$(GEOMETRY_LINT_LIB): $(GEOMETRY_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o geometry $(GEOMETRY_OBJ:.o=.ln)

$(MARCHING_LIB): $(MARCHING_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(MARCHING_OBJ)

$(MARCHING_LINT_LIB): $(MARCHING_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o marching $(MARCHING_OBJ:.o=.ln)

$(STATS_LIB): $(STATS_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(STATS_OBJ)

$(STATS_LINT_LIB): $(STATS_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o stats $(STATS_OBJ:.o=.ln)

$(SURFACE_LIB): $(SURFACE_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(SURFACE_OBJ)

$(SURFACE_LINT_LIB): $(SURFACE_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o surface $(SURFACE_OBJ:.o=.ln)
