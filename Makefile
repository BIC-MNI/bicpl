DATA_STRUCT_LIB        = libdata_structures.a
DATA_STRUCT_LINT_LIB   = lint/llib-ldata_structures.ln
DEFORM_LIB             = libdeform.a
DEFORM_LINT_LIB        = lint/llib-ldeform.ln
GEOMETRY_LIB           = libgeometry.a
GEOMETRY_LINT_LIB      = lint/llib-lgeometry.ln
MARCHING_LIB           = libmarching.a
MARCHING_LINT_LIB      = lint/llib-lmarching.ln
NUMERICAL_LIB          = libnumerical.a
NUMERICAL_LINT_LIB     = lint/llib-lnumerical.ln
SURFACE_LIB            = libsurface.a
SURFACE_LINT_LIB       = lint/llib-lsurface.ln

LIBS = \
       $(DATA_STRUCT_LIB) \
       $(DEFORM_LIB) \
       $(GEOMETRY_LIB) \
       $(MARCHING_LIB) \
       $(SURFACE_LIB)

LINT_LIBS = \
            $(DEFORM_LINT_LIB) \
            $(DATA_STRUCT_LINT_LIB) \
            $(GEOMETRY_LINT_LIB) \
            $(MARCHING_LINT_LIB) \
            $(SURFACE_LINT_LIB)

DATA_STRUCT_OBJ = \
                  Data_structures/bintree.o \
                  Data_structures/build_bintree.o \
                  Data_structures/object_bintrees.o \
                  Data_structures/point_bintree.o \
                  Data_structures/ray_bintree.o \
                  Data_structures/skiplist.o

DEFORM_OBJ = \
             Deform/deform_line.o \
             Deform/deform_polygons.o \
             Deform/find_in_direction.o \
             Deform/intersect_voxel.o \
             Deform/models.o \
             Deform/model_objects.o \
             Deform/search_utils.o

GEOMETRY_OBJ = \
               Geometry/closest_point.o \
               Geometry/curvature.o \
               Geometry/intersect.o \
               Geometry/ray_intersect.o \
               Geometry/line_circle.o \
               Geometry/map_polygons.o \
               Geometry/path_surface.o \
               Geometry/poly_dist.o \
               Geometry/polygon_sphere.o \
               Geometry/segment_polygons.o \
               Geometry/smooth_curvature.o \
               Geometry/smooth_lines.o \
               Geometry/smooth_polygons.o \
               Geometry/subdivide_lines.o \
               Geometry/subdivide_polygons.o \
               Geometry/surface_area.o \
               Geometry/tetrahedrons.o \
               Geometry/generate_tube.o \
               Geometry/tubes.o \
               Geometry/volume_slice.o

MARCHING_OBJ = \
               Marching_cubes/isosurfaces.o \
               Marching_cubes/marching_cubes.o \
               Marching_cubes/marching_no_holes.o

SURFACE_OBJ = \
              Surface_rep/spline.o \
              Surface_rep/superquadric.o \
              Surface_rep/surface_reps.o

NUMERICAL_OBJ  = \
             Numerical/amoeba.o \
             Numerical/statistics.o \
             Numerical/histogram.o

PROTOTYPE_FILE = Include/module_prototypes.h

LIBS = \
       $(DATA_STRUCT_LIB)  $(DATA_STRUCT_LINT_LIB) \
       $(DEFORM_LIB)       $(DEFORM_LINT_LIB) \
       $(GEOMETRY_LIB)     $(GEOMETRY_LINT_LIB) \
       $(MARCHING_LIB)     $(MARCHING_LINT_LIB) \
       $(SURFACE_LIB)      $(SURFACE_LINT_LIB) \
       $(NUMERICAL_LIB)    $(NUMERICAL_LINT_LIB)

all: $(PROTOTYPE_FILE) $(LIBS) $(LINT_LIBS)
	cd lint ; ln -s ../*.ln .

include ../C_dev/Makefile.include

OPT = $(OPT_O)

OPT_g = -g
OPT_O = -O

INCLUDE = $(C_UTILS_INCLUDE) -IInclude

OBJECTS = $(DEFORM_OBJ) \
          $(GEOMETRY_OBJ) \
          $(MARCHING_OBJ) \
          $(NUMERICAL_OBJ) \
          $(SURFACE_OBJ)

$(PROTOTYPE_FILE): $(OBJECTS:.o=.c)
	@echo "#ifndef  DEF_MODULE_PROTOTYPES"              >  $@
	@echo "#define  DEF_MODULE_PROTOTYPES"              >> $@
	@extract_functions -public $(OBJECTS:.o=.c)         >> $@
	@echo "#endif"                                      >> $@

DS_PROTOTYPE_FILE = Include/data_structs_prototypes.h

$(DS_PROTOTYPE_FILE): $(DATA_STRUCT_OBJ:.o=.c)
	@echo "#ifndef  DEF_DATA_STRUCT_PROTOTYPES"         >  $@
	@echo "#define  DEF_DATA_STRUCT_PROTOTYPES"         >> $@
	@extract_functions -public $(DATA_STRUCT_OBJ:.o=.c) >> $@
	@echo "#endif"                                      >> $@


$(DATA_STRUCT_LIB): $(DS_PROTOTYPE_FILE) $(DATA_STRUCT_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(DATA_STRUCT_OBJ)
	$(RANLIB) $@

$(DATA_STRUCT_LINT_LIB): $(DATA_STRUCT_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o data_structures $(DATA_STRUCT_OBJ:.o=.ln)

$(DEFORM_LIB): $(DEFORM_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(DEFORM_OBJ)
	$(RANLIB) $@

$(DEFORM_LINT_LIB): $(DEFORM_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o deform $(DEFORM_OBJ:.o=.ln)

$(GEOMETRY_LIB): $(GEOMETRY_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(GEOMETRY_OBJ)
	$(RANLIB) $@

$(GEOMETRY_LINT_LIB): $(GEOMETRY_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o geometry $(GEOMETRY_OBJ:.o=.ln)

$(MARCHING_LIB): $(MARCHING_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(MARCHING_OBJ)
	$(RANLIB) $@

$(MARCHING_LINT_LIB): $(MARCHING_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o marching $(MARCHING_OBJ:.o=.ln)

$(NUMERICAL_LIB): $(NUMERICAL_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(NUMERICAL_OBJ)
	$(RANLIB) $@

$(NUMERICAL_LINT_LIB): $(NUMERICAL_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o numerical $(NUMERICAL_OBJ:.o=.ln)

$(SURFACE_LIB): $(SURFACE_OBJ)
	@if( -e $@ ) \rm -f $@
	$(MAKE_LIBRARY) $@ $(SURFACE_OBJ)
	$(RANLIB) $@

$(SURFACE_LINT_LIB): $(SURFACE_OBJ:.o=.ln)
	@echo "--- Linting ---"
	lint -u -o surface $(SURFACE_OBJ:.o=.ln)
