DEFORM_LIB         = libdeform.a
DEFORM_LINT_LIB    = lint/llib-ldeform.ln
GEOMETRY_LIB       = libgeometry.a
GEOMETRY_LINT_LIB  = lint/llib-lgeometry.ln
MARCHING_LIB       = libmarching.a
MARCHING_LINT_LIB  = lint/llib-lmarching.ln
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

DEFORM_OBJ = deform/anneal_line.o \
             deform/create_sphere.o \
             deform/deform_line.o \
             deform/deform_polygons.o \
             deform/find_in_direction.o \
             deform/search_utils.o

GEOMETRY_OBJ = geometry/intersect.o \
               geometry/line_circle.o \
               geometry/path_surface.o \
               geometry/polygon_sphere.o \
               geometry/segment_polygons.o \
               geometry/smooth_lines.o \
               geometry/smooth_polygons.o \
               geometry/subdivide_lines.o \
               geometry/subdivide_polygons.o

MARCHING_OBJ = \
               marching_cubes/marching_cubes.o \
               marching_cubes/marching_no_holes.o

SURFACE_OBJ = \
              surface_rep/spline.o \
              surface_rep/superquadric.o \
              surface_rep/surface_reps.o

PROTOTYPE_FILE = Include/def_module_prototypes.h

LIBS = \
       $(DEFORM_LIB)   $(DEFORM_LINT_LIB) \
       $(GEOMETRY_LIB) $(GEOMETRY_LINT_LIB) \
       $(MARCHING_LIB) $(MARCHING_LINT_LIB) \
       $(SURFACE_LIB)  $(SURFACE_LINT_LIB)

all: $(PROTOTYPE_FILE) $(LIBS) $(LINT_LIBS)

include $(C_DEV_DIRECTORY)/Make/Makefile.include

OPT = $(OPT_O)

OPT_g = -g
OPT_O = -O

INCLUDE = -I$(C_DEV_DIRECTORY)/Include -IInclude

OBJECTS = $(DEFORM_OBJ) \
          $(GEOMETRY_OBJ) \
          $(MARCHING_OBJ) \
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

$(SURFACE_LIB): $(SURFACE_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(SURFACE_OBJ)

$(SURFACE_LINT_LIB): $(SURFACE_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o surface $(SURFACE_OBJ:.o=.ln)
