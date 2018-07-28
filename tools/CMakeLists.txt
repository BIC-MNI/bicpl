LINK_LIBRARIES( bicpl ${VOLUME_IO_LIBRARIES} ${MINC2_LIBRARIES} m )

ADD_EXECUTABLE(add_surfaces  add_surfaces.c)
ADD_EXECUTABLE(ascii_binary  ascii_binary.c)
ADD_EXECUTABLE(average_objects  average_objects.c)
ADD_EXECUTABLE(average_surfaces  average_surfaces.c)
ADD_EXECUTABLE(bicobj2oogl  bicobj2oogl.c)
ADD_EXECUTABLE(bicobj2vtk  bicobj2vtk.c)
ADD_EXECUTABLE(check_polygons  check_polygons.c)
ADD_EXECUTABLE(colour_object  colour_object.c)
ADD_EXECUTABLE(convert_object  convert_object.c)
ADD_EXECUTABLE(copy_colours  copy_colours.c)
ADD_EXECUTABLE(create_grid  create_grid.c)
ADD_EXECUTABLE(create_rectangle  create_rectangle.c)
ADD_EXECUTABLE(create_tetra  create_tetra.c)
ADD_EXECUTABLE(diff_points  diff_points.c)
ADD_EXECUTABLE(diff_surfaces  diff_surfaces.c)
ADD_EXECUTABLE(dump_curvatures  dump_curvatures.c)
ADD_EXECUTABLE(dump_points  dump_points.c)
ADD_EXECUTABLE(dump_point_diffs  dump_point_diffs.c)
ADD_EXECUTABLE(dump_polygons  dump_polygons.c)
ADD_EXECUTABLE(dump_vertex_normal_diffs  dump_vertex_normal_diffs.c )
ADD_EXECUTABLE(half_polygons  half_polygons.c)
ADD_EXECUTABLE(make_colour_bar  make_colour_bar.c)
ADD_EXECUTABLE(make_concentric_surface  make_concentric_surface.c)
ADD_EXECUTABLE(manifold_polygons  manifold_polygons.c)
ADD_EXECUTABLE(measure_surface_area  measure_surface_area.c)
ADD_EXECUTABLE(merge_polygons  merge_polygons.c)
ADD_EXECUTABLE(objconcat  objconcat.c)
ADD_EXECUTABLE(obj2brainvisa  obj2brainvisa.c)
ADD_EXECUTABLE(perturb_surface  perturb_surface.c)
ADD_EXECUTABLE(perturb_surface_normal  perturb_surface_normal.c)
ADD_EXECUTABLE(polygon_map  polygon_map.c)
ADD_EXECUTABLE(polygons_to_lines  polygons_to_lines.c)
ADD_EXECUTABLE(print_n_polygons  print_n_polygons.c)
ADD_EXECUTABLE(print_object_centroid  print_object_centroid.c)
ADD_EXECUTABLE(print_object_limits  print_object_limits.c)
ADD_EXECUTABLE(reconstitute_points  reconstitute_points.c)
ADD_EXECUTABLE(refine_mesh  refine_mesh.c )
ADD_EXECUTABLE(separate_polygons  separate_polygons.c)
ADD_EXECUTABLE(set_line_width  set_line_width.c)
ADD_EXECUTABLE(set_object_colour  set_object_colour.c)
ADD_EXECUTABLE(set_object_opacity  set_object_opacity.c)
ADD_EXECUTABLE(set_object_surfprop  set_object_surfprop.c)
ADD_EXECUTABLE(smooth_lines  smooth_lines.c)
ADD_EXECUTABLE(smooth_normals  smooth_normals.c)
ADD_EXECUTABLE(smooth_polygons  smooth_polygons.c)
ADD_EXECUTABLE(spline_lines  spline_lines.c)
ADD_EXECUTABLE(split_polygons  split_polygons.c)
ADD_EXECUTABLE(subdivide_polygons  subdivide_polygons.c)
ADD_EXECUTABLE(subdivide_values  subdivide_values.c)
ADD_EXECUTABLE(triangulate_polygons  triangulate_polygons.c)
ADD_EXECUTABLE(expand_object_along_surface_normal expand_object_along_surface_normal.c)

INSTALL(
  TARGETS 
    add_surfaces
    ascii_binary
    average_objects
    average_surfaces
    bicobj2oogl
    bicobj2vtk
    check_polygons
    colour_object
    convert_object
    copy_colours
    create_grid
    create_rectangle
    create_tetra
    diff_points
    diff_surfaces
    dump_curvatures
    dump_point_diffs
    dump_points
    dump_polygons
    dump_vertex_normal_diffs
    half_polygons
    make_colour_bar
    make_concentric_surface
    manifold_polygons
    measure_surface_area
    merge_polygons
    obj2brainvisa
    objconcat
    perturb_surface
    polygon_map
    polygons_to_lines
    print_n_polygons
    print_object_centroid
    print_object_limits
    reconstitute_points
    refine_mesh
    separate_polygons
    set_line_width
    set_object_colour
    set_object_opacity
    set_object_surfprop
    smooth_lines
    smooth_normals
    spline_lines
    split_polygons
    subdivide_polygons
    subdivide_values
    triangulate_polygons
    expand_object_along_surface_normal
  DESTINATION bin)
