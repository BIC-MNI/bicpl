def: all

include ../Volume_io/Makefile.include

OPT = $(OPT_O)

BIC_OBJ = $(BIC_SRC:.c=.o)

BIC_LIB      = $(ARCH_DIR)/libbicpl.a
BIC_LIB-O3   = $(ARCH_DIR)/libbicpl-O3.a
BIC_LINT_LIB = $(ARCH_DIR)/llib-lbicpl.ln

include Makefile.objects

INCLUDE = $(VOLUME_IO_INCLUDE) -IInclude

#-----------------------------------------------------------------------------

$(DATA_STRUCT_SRC) $(TRANSFORM_SRC) $(GEOMETRY_SRC) $(NUMERICAL_SRC) \
$(OBJECTS_SRC) $(PROG_UTILS_SRC) $(VOLUMES_SRC) \
Numerical/$(ARCH_DIR)/minimize_lsq_include.c \
Volumes/$(ARCH_DIR)/call_rend_f_include.c \
Volumes/$(ARCH_DIR)/rend_f_include.c \
Volumes/$(ARCH_DIR)/render_funcs_include.c:
	@$(MAKE_SOURCE_LINKS)

Numerical/$(ARCH_DIR)/minimize_lsq.o \
Numerical/$(ARCH_DIR)/minimize_lsq_float.o \
Numerical/$(ARCH_DIR)/minimize_lsq.u \
Numerical/$(ARCH_DIR)/minimize_lsq_float.u: \
                           Numerical/$(ARCH_DIR)/minimize_lsq_include.c

Volumes/$(ARCH_DIR)/rend_f.o \
Volumes/$(ARCH_DIR)/rend_f.u: Volumes/$(ARCH_DIR)/call_rend_f_include.c \
                              Volumes/$(ARCH_DIR)/rend_f_include.c \
                              Volumes/$(ARCH_DIR)/render_funcs_include.c

#-----------------------------------------------------------------------------

DS_PROTOTYPE_FILE = Include/ds_prototypes.h

$(DS_PROTOTYPE_FILE): $(DATA_STRUCT_SRC)
	@$(MAKE_PROTOTYPES) $@ $(DATA_STRUCT_SRC)

#-----------------------------------------------------------------------------

TRANSFORM_PROTOTYPE_FILE = Include/trans_prototypes.h

$(TRANSFORM_PROTOTYPE_FILE): $(TRANSFORM_SRC)
	@$(MAKE_PROTOTYPES) $@ $(TRANSFORM_SRC)

#-----------------------------------------------------------------------------

GEOMETRY_PROTOTYPE_FILE = Include/geom_prototypes.h

$(GEOMETRY_PROTOTYPE_FILE): $(GEOMETRY_SRC)
	@$(MAKE_PROTOTYPES) $@ $(GEOMETRY_SRC)

#-----------------------------------------------------------------------------

NUMERIC_PROTOTYPE_FILE = Include/numeric_prototypes.h

$(NUMERIC_PROTOTYPE_FILE): $(NUMERICAL_SRC)
	@$(MAKE_PROTOTYPES) $@ $(NUMERICAL_SRC)

#-----------------------------------------------------------------------------

OBJ_PROTOTYPE_FILE = Include/object_prototypes.h

$(OBJ_PROTOTYPE_FILE): $(OBJECTS_SRC)
	@$(MAKE_PROTOTYPES) $@ $(OBJECTS_SRC)

#-----------------------------------------------------------------------------

PROG_PROTOTYPE_FILE = Include/prog_prototypes.h

$(PROG_PROTOTYPE_FILE): $(PROG_UTILS_SRC)
	@$(MAKE_PROTOTYPES) $@ $(PROG_UTILS_SRC)

#-----------------------------------------------------------------------------

VOLUMES_PROTOTYPE_FILE = Include/vol_prototypes.h

$(VOLUMES_PROTOTYPE_FILE): $(VOLUMES_SRC)
	@$(MAKE_PROTOTYPES) $@ $(VOLUMES_SRC)

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

$(ARCH_DIR):
	$(MKDIR) $@

all: $(ARCH_DIR) $(BIC_LIB)

lint: $(BIC_LINT_LIB)
