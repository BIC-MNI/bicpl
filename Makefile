def: all

include ../Volume_io/Makefile.include

OPT = -O

BIC_OBJ = $(BIC_SRC:.c=.o)

BIC_LIB      = libbicpl.a
BIC_LIB-O3   = libbicpl-O3.a
BIC_LINT_LIB = llib-lbicpl.ln

include Makefile.objects

#Numerical/minimize_lsq.c  Numerical/minimize_lsq_float.c: \
#                               Numerical/minimize_lsq_include.c

INCLUDE = $(VOLUME_IO_INCLUDE) -IInclude $(RECIPES_INCLUDE)

#-----------------------------------------------------------------------------

DS_PROTOTYPE_FILE = Include/ds_prototypes.h

$(DS_PROTOTYPE_FILE): $(DATA_STRUCT_SRC)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(DATA_STRUCT_SRC)

#-----------------------------------------------------------------------------

TRANSFORM_PROTOTYPE_FILE = Include/trans_prototypes.h

$(TRANSFORM_PROTOTYPE_FILE): $(TRANSFORM_SRC)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(TRANSFORM_SRC)

#-----------------------------------------------------------------------------

GEOMETRY_PROTOTYPE_FILE = Include/geom_prototypes.h

$(GEOMETRY_PROTOTYPE_FILE): $(GEOMETRY_SRC)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(GEOMETRY_SRC)

#-----------------------------------------------------------------------------

NUMERIC_PROTOTYPE_FILE = Include/numeric_prototypes.h

$(NUMERIC_PROTOTYPE_FILE): $(NUMERICAL_SRC)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(NUMERICAL_SRC)

#-----------------------------------------------------------------------------

OBJ_PROTOTYPE_FILE = Include/object_prototypes.h

$(OBJ_PROTOTYPE_FILE): $(OBJECTS_SRC)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(OBJECTS_SRC)

#-----------------------------------------------------------------------------

PROG_PROTOTYPE_FILE = Include/prog_prototypes.h

$(PROG_PROTOTYPE_FILE): $(PROG_UTILS_SRC)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(PROG_UTILS_SRC)

#-----------------------------------------------------------------------------

VOLUMES_PROTOTYPE_FILE = Include/vol_prototypes.h

$(VOLUMES_PROTOTYPE_FILE): $(VOLUMES_SRC)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(VOLUMES_SRC)

#-----------------------------------------------------------------------------

$(BIC_LIB): $(DS_PROTOTYPE_FILE) \
            $(GEOMETRY_PROTOTYPE_FILE) \
            $(TRANSFORM_PROTOTYPE_FILE) \
            $(NUMERIC_PROTOTYPE_FILE) \
            $(OBJ_PROTOTYPE_FILE) \
            $(PROG_PROTOTYPE_FILE) \
            $(VOLUMES_PROTOTYPE_FILE) \
            $(BIC_OBJ)
	\rm -f $@
	$(MAKE_LIBRARY) $@ $(BIC_OBJ)
	$(RANLIB) $@

$(BIC_LIB-O3): $(DS_PROTOTYPE_FILE) \
            $(GEOMETRY_PROTOTYPE_FILE) \
            $(TRANSFORM_PROTOTYPE_FILE) \
            $(NUMERIC_PROTOTYPE_FILE) \
            $(OBJ_PROTOTYPE_FILE) \
            $(PROG_PROTOTYPE_FILE) \
            $(VOLUMES_PROTOTYPE_FILE) \
            $(BIC_OBJ:.o=.u)
	\rm -f $@
	$(MAKE_LIBRARY) $@ $(BIC_OBJ:.o=.u)
	$(RANLIB) $@

$(BIC_LINT_LIB): $(DS_PROTOTYPE_FILE) \
                 $(GEOMETRY_PROTOTYPE_FILE) \
                 $(TRANSFORM_PROTOTYPE_FILE) \
                 $(NUMERIC_PROTOTYPE_FILE) \
                 $(OBJ_PROTOTYPE_FILE) \
                 $(PROG_PROTOTYPE_FILE) \
                 $(VOLUMES_PROTOTYPE_FILE) \
                 $(BIC_SRC:.c=.ln)
	$(LINT) -o bicpl $(BIC_SRC:.c=.ln)

all: $(BIC_LIB)

lint: $(BIC_LINT_LIB)
