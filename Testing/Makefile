include ../Makefile.include

INCLUDE = -I. $(BIC_PL_INCLUDE)
OPT = -O
DEFINE =

all: test_render \
     lint_test_render

# ----------------------------------------------------------------------------

test_render_objects = test_render.o

test_render: $(test_render_objects)
	$(CC) $(test_render_objects) -o $@ $(BIC_PL_LIBS)

lint_test_render: $(test_render_objects:.o=.ln)
	$(LINT) -u $(LINTFLAGS) $(test_render_objects:.o=.ln) \
                                $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

test_render-O3: test_render.u
	$(CC) -O3 test_render.u -o $@ $(BIC_PL_LIBS-O3)

# ----------------------------------------------------------------------------

test_render.pixie: test_render
	pixie -o $@ test_render

prof: test_render.pixie
	test_render.pixie $(AVG) 1000
	prof -gprof -pixie test_render > prof.gprof
	prof -pixie -heavy -quit 200 test_render > prof.heavy
	prof -pixie -proc -quit 200 test_render > prof.proc

# ----------------------------------------------------------------------------

test_volume_objects = test_volume.o

test_volume: $(test_volume_objects)
	$(CC) $(test_volume_objects) -o $@ $(BIC_PL_LIBS)

lint_test_volume: $(test_volume_objects:.o=.ln)
	$(LINT) -u $(LINTFLAGS) $(test_volume_objects:.o=.ln) \
                                $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

test_splines_objects = test_splines.o

test_splines: $(test_splines_objects)
	$(CC) $(test_splines_objects) -o $@ $(BIC_PL_LIBS)

lint_test_splines: $(test_splines_objects:.o=.ln)
	$(LINT) -u $(LINTFLAGS) $(test_splines_objects:.o=.ln) \
                                $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

test_mapping_objects = test_mapping.o

test_mapping: $(test_mapping_objects)
	$(CC) $(test_mapping_objects) -o $@ $(BIC_PL_LIBS)

lint_test_mapping: $(test_mapping_objects:.o=.ln)
	$(LINT) -u $(LINTFLAGS) $(test_mapping_objects:.o=.ln) \
                                $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

test_globals_objects = test_globals.o

test_globals: $(test_globals_objects)
	$(CC) $(test_globals_objects) -o $@ $(BIC_PL_LIBS)

lint_test_globals: $(test_globals_objects:.o=.ln)
	$(LINT) -u $(LINTFLAGS) $(test_globals_objects:.o=.ln) \
                                $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

test_amoeba_objects = test_amoeba.o

test_amoeba: $(test_amoeba_objects)
	$(CC) $(test_amoeba_objects) -o $@ $(BIC_PL_LIBS)

lint_test_amoeba: $(test_amoeba_objects:.o=.ln)
	$(LINT) -u $(LINTFLAGS) $(test_amoeba_objects:.o=.ln) \
                                $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

test_rotmat_objects = test_rotmat.o

test_rotmat: $(test_rotmat_objects)
	$(CC) $(test_rotmat_objects) -o $@ $(BIC_PL_LIBS)

lint_test_rotmat: $(test_rotmat_objects:.o=.ln)
	$(LINT) -u $(LINTFLAGS) $(test_rotmat_objects:.o=.ln) \
                                $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

test_gradient: test_gradient.o
	$(CC) test_gradient.o -o $@ $(BIC_PL_LIBS)

lint_test_gradient: test_gradient.ln
	$(LINT) -u $(LINTFLAGS) test_gradient.ln $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

$(ARCH_DIR)/render_speed.c:
	@$(MAKE_SOURCE_LINKS) $(ARCH_DIR)/render_speed.c

$(ARCH_DIR)/render_speed: $(ARCH_DIR)/render_speed.o
	$(CC) $(ARCH_DIR)/render_speed.o -o $@ $(BIC_PL_LIBS)

$(ARCH_DIR)/render_speed-O3: $(ARCH_DIR)/render_speed.u
	$(CC) -O3 $(ARCH_DIR)/render_speed.u -o $@ $(BIC_PL_LIBS-O3)

lint_render_speed: $(ARCH_DIR)/render_speed.ln
	$(LINT) -u $(LINTFLAGS) $(ARCH_DIR)/render_speed.ln $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

test_hash: test_hash.o
	$(CC) test_hash.o -o $@ $(BIC_PL_LIBS)

test_hash-O3: test_hash.u
	$(CC) -O3 test_hash.u -o $@ $(BIC_PL_LIBS-O3)

lint_test_hash: test_hash.ln
	$(LINT) -u $(LINTFLAGS) test_hash.ln $(BIC_PL_LINT_LIBS)

# ----------------------------------------------------------------------------

test_filter_speed: test_filter_speed.o
	$(CC) test_filter_speed.o -o $@ $(BIC_PL_LIBS)

test_filter_speed2: test_filter_speed2.o
	$(CC) test_filter_speed2.o -o $@ $(BIC_PL_LIBS)

test_filter_speed-O3: test_filter_speed.u
	$(CC) -O3 test_filter_speed.u -o $@ $(BIC_PL_LIBS-O3)

test_filter_speed2-O3: test_filter_speed2.u
	$(CC) -O3 test_filter_speed2.u -o $@ $(BIC_PL_LIBS-O3)

test_filter_speed.pixie: test_filter_speed
	pixie -o $@ test_filter_speed

test_filter_speed2.pixie: test_filter_speed2
	pixie -o $@ test_filter_speed2

prof2: test_filter_speed2.Counts
	prof -E sinc -E memset -E __sqrt -E __sin -E initialize_sinc_approx -E MI_convert_type -E memcpy -quit 100 -pixie -h -p test_filter_speed2 >&! $@

# ----------------------------------------------------------------------------

test_weights: test_weights.o ../libbicpl.a
	$(CC) test_weights.o -o $@ $(BIC_PL_LIBS)

# ----------------------------------------------------------------------------

test_flatten: test_flatten.o
	$(CC) test_flatten.o -o $@ $(BIC_PL_LIBS)

# ----------------------------------------------------------------------------

test_prediction: test_prediction.o ../libbicpl.a
	$(CC) test_prediction.o -o $@ $(BIC_PL_LIBS)

# ----------------------------------------------------------------------------

test_prediction_3d: test_prediction_3d.o ../libbicpl.a
	$(CC) test_prediction_3d.o -o $@ $(BIC_PL_LIBS)

# ----------------------------------------------------------------------------

test_sphere: test_sphere.o ../libbicpl.a
	$(CC) test_sphere.o -o $@ $(BIC_PL_LIBS)

# ----------------------------------------------------------------------------

$(ARCH)/test_bintree.c $(ARCH)/ray_intersect.c:
	@$(MAKE_SOURCE_LINKS) $@

$(ARCH)/test_bintree: $(ARCH)/test_bintree.o $(ARCH)/ray_intersect.o
	$(CC) $(ARCH)/test_bintree.o $(ARCH)/ray_intersect.o -o $@ $(BIC_PL_LIBS)

$(ARCH)/test_bintree-O3: $(ARCH)/test_bintree.u $(ARCH)/ray_intersect.u
	$(CC) -O3 $(ARCH)/test_bintree.u $(ARCH)/ray_intersect.u -o $@ $(BIC_PL_LIBS-O3)

$(ARCH)/test_bintree.pixie: $(ARCH)/test_bintree
	pixie -o $@ $(ARCH)/test_bintree

$(ARCH)/test_bintree-O3.pixie: $(ARCH)/test_bintree-O3
	pixie -o $@ $(ARCH)/test_bintree-O3

prof.test_bintree: test_bintree.Counts Makefile
	prof -E _random \
             -E get_planes \
             -E get_random \
             -E get_random_0_to_1 \
             -E check_initialized \
             -E main \
             -E get_node_split_position \
             -E get_node_split_axis \
             -E find_best_split_node_for_axis \
             -quit 100 -pixie -h -p  $(ARCH)/test_bintree test_bintree.Counts >&! $@

prof.test_bintree-O3: test_bintree-O3.Counts Makefile
	prof -E _random \
             -E get_planes \
             -E get_random \
             -E get_random_0_to_1 \
             -E check_initialized \
             -E main \
             -E get_node_split_position \
             -E get_node_split_axis \
             -E find_best_split_node_for_axis \
             -quit 100 -pixie -h -p  $(ARCH)/test_bintree-O3 test_bintree-O3.Counts >&! $@

# ----------------------------------------------------------------------------

prof.render: render_speed.Counts
	prof -E copy_multidim_data_reordered -E memset -E MI_convert_type -E memcpy -quit 100 -pixie -h -p render_speed >&! $@
