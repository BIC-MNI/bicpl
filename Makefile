def: all

include ../Volume_io/Makefile.include

OPT = -O

BIC_LIB      = libbicpl.a
BIC_LINT_LIB = llib-lbicpl.ln

include Makefile.objects

INCLUDE = $(VOLUME_IO_INCLUDE) -IInclude $(RECIPES_INCLUDE)

#-----------------------------------------------------------------------------

DS_PROTOTYPE_FILE = Include/ds_prototypes.h

$(DS_PROTOTYPE_FILE): $(DATA_STRUCT_OBJ:.o=.c)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(DATA_STRUCT_OBJ:.o=.c)

#-----------------------------------------------------------------------------

GEOMETRY_PROTOTYPE_FILE = Include/geom_prototypes.h

$(GEOMETRY_PROTOTYPE_FILE): $(GEOMETRY_OBJ:.o=.c)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(GEOMETRY_OBJ:.o=.c)

#-----------------------------------------------------------------------------

NUMERIC_PROTOTYPE_FILE = Include/numeric_prototypes.h

$(NUMERIC_PROTOTYPE_FILE): $(NUMERICAL_OBJ:.o=.c)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(NUMERICAL_OBJ:.o=.c)

#-----------------------------------------------------------------------------

OBJ_PROTOTYPE_FILE = Include/object_prototypes.h

$(OBJ_PROTOTYPE_FILE): $(OBJECTS_OBJ:.o=.c)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(OBJECTS_OBJ:.o=.c)

#-----------------------------------------------------------------------------

PROG_PROTOTYPE_FILE = Include/prog_prototypes.h

$(PROG_PROTOTYPE_FILE): $(PROG_UTILS_OBJ:.o=.c)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(PROG_UTILS_OBJ:.o=.c)

#-----------------------------------------------------------------------------

VOLUMES_PROTOTYPE_FILE = Include/vol_prototypes.h

$(VOLUMES_PROTOTYPE_FILE): $(VOLUMES_OBJ:.o=.c)
	@$(MAKE_DIRECTORY)/create_prototypes.csh $@ $(VOLUMES_OBJ:.o=.c)

#-----------------------------------------------------------------------------

$(BIC_LIB): $(DS_PROTOTYPE_FILE) \
            $(GEOMETRY_PROTOTYPE_FILE) \
            $(NUMERIC_PROTOTYPE_FILE) \
            $(OBJ_PROTOTYPE_FILE) \
            $(PROG_PROTOTYPE_FILE) \
            $(VOLUMES_PROTOTYPE_FILE) \
            $(BIC_OBJ)
	\rm -f $@
	$(MAKE_LIBRARY) $@ $(BIC_OBJ)
	$(RANLIB) $@

$(BIC_LINT_LIB): $(DS_PROTOTYPE_FILE) \
                 $(GEOMETRY_PROTOTYPE_FILE) \
                 $(NUMERIC_PROTOTYPE_FILE) \
                 $(OBJ_PROTOTYPE_FILE) \
                 $(PROG_PROTOTYPE_FILE) \
                 $(VOLUMES_PROTOTYPE_FILE) \
                 $(BIC_OBJ:.o=.ln)
	$(LINT) -o bicpl $(BIC_OBJ:.o=.ln)

all: $(BIC_LIB)

lint: $(BIC_LINT_LIB)
