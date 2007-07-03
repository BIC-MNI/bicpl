AM_CPPFLAGS = -I$(top_srcdir)/Include
LDADD = ../libbicpl.la -lm

bin_PROGRAMS = add_surfaces \
               ascii_binary \
               average_objects \
               average_surfaces \
	       bicobj2oogl \
               bicobj2vtk \
               check_polygons \
               colour_object \
               convert_object \
               copy_colours \
               create_grid \
               create_rectangle \
               create_tetra \
               diff_points \
               diff_surfaces \
               dump_curvatures \
               dump_points \
               dump_point_diffs \
               dump_polygons \
               dump_vertex_normal_diffs \
               half_polygons \
               make_colour_bar \
               make_concentric_surface \
               manifold_polygons \
               measure_surface_area \
               merge_polygons \
               perturb_surface \
               polygon_map \
               polygons_to_lines \
               print_n_polygons \
               print_object_centroid \
               print_object_limits \
               reconstitute_points \
               refine_mesh \
               separate_polygons \
               set_line_width \
               set_object_colour \
               set_object_opacity \
               set_object_surfprop \
               smooth_lines \
               smooth_normals \
               spline_lines \
               split_polygons \
               subdivide_polygons \
               subdivide_values \
               triangulate_polygons

EXTRA_DIST = plytools-patch

add_surfaces_SOURCES = add_surfaces.c
ascii_binary_SOURCES = ascii_binary.c
average_objects_SOURCES = average_objects.c
average_surfaces_SOURCES = average_surfaces.c
bicobj2oogl_SOURCES = bicobj2oogl.c
bicobj2vtk_SOURCES = bicobj2vtk.c
check_polygons_SOURCES = check_polygons.c
colour_object_SOURCES = colour_object.c
convert_object_SOURCES = convert_object.c
copy_colours_SOURCES = copy_colours.c
create_grid_SOURCES = create_grid.c
create_rectangle_SOURCES = create_rectangle.c
create_tetra_SOURCES = create_tetra.c
diff_points_SOURCES = diff_points.c
diff_surfaces_SOURCES = diff_surfaces.c
dump_curvatures_SOURCES = dump_curvatures.c
dump_points_SOURCES = dump_points.c
dump_point_diffs_SOURCES = dump_point_diffs.c
dump_polygons_SOURCES = dump_polygons.c
dump_vertex_normal_diffs_SOURCES = dump_vertex_normal_diffs.c 
half_polygons_SOURCES = half_polygons.c
make_colour_bar_SOURCES = make_colour_bar.c
make_concentric_surface_SOURCES = make_concentric_surface.c
manifold_polygons_SOURCES = manifold_polygons.c
measure_surface_area_SOURCES = measure_surface_area.c
merge_polygons_SOURCES = merge_polygons.c
perturb_surface_SOURCES = perturb_surface.c
polygon_map_SOURCES = polygon_map.c
polygons_to_lines_SOURCES = polygons_to_lines.c
print_n_polygons_SOURCES = print_n_polygons.c
print_object_centroid_SOURCES = print_object_centroid.c
print_object_limits_SOURCES = print_object_limits.c
reconstitute_points_SOURCES = reconstitute_points.c
refine_mesh_SOURCES = refine_mesh.c 
separate_polygons_SOURCES = separate_polygons.c
set_line_width_SOURCES = set_line_width.c
set_object_colour_SOURCES = set_object_colour.c
set_object_opacity_SOURCES = set_object_opacity.c
set_object_surfprop_SOURCES = set_object_surfprop.c
smooth_lines_SOURCES = smooth_lines.c
smooth_normals_SOURCES = smooth_normals.c
spline_lines_SOURCES = spline_lines.c
split_polygons_SOURCES = split_polygons.c
subdivide_polygons_SOURCES = subdivide_polygons.c
subdivide_values_SOURCES = subdivide_values.c
triangulate_polygons_SOURCES = triangulate_polygons.c

# These two programs are not likely of general interest.
# Moreover, they won't compile on certain platforms, so
# I'm disabling them.

# noinst_PROGRAMS = bicobj2ply  make_mesh_map

#bicobj2ply_SOURCES = bicobj2ply.c ply.c ply.h
#make_mesh_map_SOURCES = make_mesh_map.c ply.c ply.h

