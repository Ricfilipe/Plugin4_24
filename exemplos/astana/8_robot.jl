include("2_facade.jl")

# ROBOT ANALYSIS _______________________________________________________________

z0 = 10 # plano de intersecção
intersections = true

set_backend_family(
  default_truss_bar_family(),
  robot,
  Khepri.robot_truss_bar_family(
  material=[
    "ElasticIsotropic",   # name
    Khepri.I_MT_STEEL,    # Type
    "Steel",              # Name
    "I'm really steel",   # Nuance
    180000000000.0,       # E
    0.3,                  # NU
    81000000000.0,        # Kirchoff
    77010.0,              # RO
    1.2e-05,              # LX
    0.04,                 # DumpCoef
    235000000.0,          # RE
    360000000.0],         # RT
  section=[
    "Tube",               #name
    "ElasticIsotropic",   #material_name
    false,                #iswood
    [(true,               #solid?
      0.0213,             #diameter
      0.0026)]]))

set_backend_family(default_truss_bar_family(), unity, unity_material_family("Default/Materials/Aluminum"))
free_node_fam = truss_node_family_element(default_truss_node_family(), radius=0.1)
set_backend_family(free_node_fam, unity, unity_material_family("Default/Materials/Aluminum"))
sup_node_fam = truss_node_family_element(default_truss_node_family(), support=create_node_support("SupportA", ux=true, uy=true, uz=true), radius=0.1)
# set_backend_family(sup_node_fam, unity, unity_material_family("materials/metal/CopperOld"))
set_backend_family(sup_node_fam, unity, unity_material_family("materials/metal/Copper"))

intersect_plane_seg(p1, p2, z)=
  let (x1, y1, z1) = p1,
      (x2, y2, z2) = p2

    (min(z1, z2) > z || max(z1, z2) < z) && return nothing
    z1 == z ? xyz(x1, y1, z) :
    z2 == z ? xyz(x2, y2, z) :
      let x = x1 + (z - z1) * (x2 - x1) / (z2 - z1),
          y = y1 + (z - z1) * (y2 - y1) / (z2 - z1)
          # z = intersection plan
          # p1, p2 = segment to intersect
          xyz(x, y, z)
      end
  end

higher_p(p1, p2) = [p1, p2][argmax(cz.([p1, p2]))]
intersected_tuss_bar(p1, p2, z) =
  let i = intersect_plane_seg(p1, p2, z)
    if all((p1.z, p2.z) .<= z) # @. (p1.z, p2.z) <= z
      nothing
    elseif i === nothing
      truss_bar(p1, p2)
    else
      truss_bar(i, higher_p(p1, p2))
    end
  end

rejected_nodes = []
connection_truss_bar(p1, p2, z) =
  p1.z <= z || p2.z <= z ?
    push!(rejected_nodes, p1, p2) :
      truss_bar(p1, p2)

intersected_bar(pts) = map((p0, p1) -> (intersections ? intersected_tuss_bar : truss_bar)(p0, p1, z0), pts[1:end-1], pts[2:end])
robot_bar(pts) = map((p0, p1) -> (intersections ? connection_truss_bar : truss_bar)(p0, p1, z0), pts[1:end-1], pts[2:end])

find_intersections(pts, z0) = map((p0, p1) -> intersect_plane_seg(p0, p1, z0), pts[1:end-1], pts[2:end])
find_intersections(z) = pts -> find_intersections(pts, z)

free_node(pt) = with(default_truss_node_family, free_node_fam) do
                    pt.z <= z0 ? nothing : truss_node(pt)
                end

fixed_node(pt) = with(default_truss_node_family, sup_node_fam) do
                  truss_node(pt)
                end

out_nodes(p) = map(free_node, flatten(bays_points(p)))

# same_point(p1, p2) =
#   abs(p1.x - p2.x) <= 0.01 &&
#   abs(p1.y - p2.y) <= 0.01 &&
#   abs(p1.z - p2.z) <= 0.01
#
# any_true(arr) = arr == [] ? false : arr[1] ? true : any_true(arr[2:end])
# in_set(p, pts) = any_true([same_point(p, q) for q in pts])
# filter_set(pts1, pts2) = filter(p -> !(in_set(p, pts2)), pts1)
#
# filter_points(p, rejected_set) = filter_set(connection_point_inside(p), rejected_set)
filter_points(p, rejected_set) = setdiff!(connection_point_inside(p), rejected_set)
iner_nodes(p, rejected_set) = map(fixed_node, filter_points(p, rejected_set))

frame_bars(p) =
  for m in facadeMATRIX(p, 0, x_divisions, y_divisions, n_frames_for_frames)
    map(intersected_bar, m)
  end

frame_intersection_nodes(p)=
  for m in facadeMATRIX(p, 0, x_divisions, y_divisions, n_frames_for_frames)
    map(fixed_node, filter(!isnothing, flatten(map(find_intersections(z0), m))))
  end

diagonal_bay_bars(p) =
  begin
    #bays used in the real Astana
    map(intersected_bar, transpose_matrix(bays_points(p)))
    map(intersected_bar, transpose_matrix(bays_contour_points(p)))
  end

function diagonal_bay_intersection_nodes(p)
    map(fixed_node, filter(!isnothing, flatten(map(find_intersections(z0), transpose_matrix(bays_points(p))))))
    map(fixed_node, filter(!isnothing, flatten(map(find_intersections(z0), transpose_matrix(bays_contour_points(p))))))
end

new_conections(p) =
  let pairs = conection_pairs(p)
    for i in pairs
        robot_bar(i)
    end
  end

perpendicular_bay_bars(p) =   #variação interesante das bays só para teste
  for m in facadeMATRIX(p, 0, x_divisions, y_divisions, n_frames_for_frames)
    map(intersected_bar, transpose_matrix(m))
  end

perpendicular_bay_intersection_nodes(p)=
  for m in facadeMATRIX(p, 0, x_divisions, y_divisions, n_frames_for_frames)
    map(fixed_node, filter(!isnothing, flatten(map(find_intersections(z0), transpose_matrix(m)))))
  end

ground() =
  # with(current_material, get_material("Materials/Grass/Grass")) do
  #   surface_grid(sin_u_mul_v(100))
  with(current_material, get_material("Materials/Plaster/Plaster1")) do
      surface_grid([[xyz(-1000,1000,0), xyz(1000,1000,-.1)], [xyz(-1000,-1000,0), xyz(1000,-1000,0)]])
  end


# |||||||||||||||||||||||||||||||| GENERATE |||||||||||||||||||||||||||||||||| #

delete_all_shapes()
# ground()

function astana_truss()
  # NODES
  out_nodes(astana_center_p) # -- façade nodes = free nodes
  intersections ? frame_intersection_nodes(astana_center_p) : nothing # -- anchor points on the ground
  # -- bays option 1 : perpendicular
  intersections ? perpendicular_bay_intersection_nodes(astana_center_p) : nothing # anchor points on the ground
  # -- bays option 2 : used in BIG's Astana
  intersections ? diagonal_bay_intersection_nodes(astana_center_p) : nothing # anchor points on the ground
  # -- conncetion nodes = anchor points / fixed nodes (same for both options)
  iner_nodes(astana_center_p, rejected_nodes)

  # FRAMES
  frame_bars(astana_center_p) # -- frames
  perpendicular_bay_bars(astana_center_p) # -- bay bars
  diagonal_bay_bars(astana_center_p) # -- bay bars
  # -- connection opyion 1: orinal by BIG, makes no sence
  # connections(astana_center_p) #TODO #change beams to lines in original func if necessary for robot backend
  # -- connection opyion 2: dischange on intersections
  new_conections(astana_center_p)


end

# |||||||||||||||||||||||||||||||| ANALYSIS |||||||||||||||||||||||||||||||||| #

project_kind(Khepri.I_PT_SHELL)
backend(robot)
new_robot_analysis(
  ()->
    begin
      delete_all_shapes()
      astana_truss()
    end,
  vxyz(3000,3000,-7500), self_weight=true) do results #vxyz(3000.0 #=vento=#, 3000.0 #=vento=#, -7500.0 #=gravitica=#)) do results
    let displs = displacements(nodes(results))
        node_displ = node -> norm(node_displacement_vector(displs, node.id, Khepri.I_LRT_NODE_DISPLACEMENT))
        disps = maximum([node_displ(node) for node in values(added_nodes())])
      with(current_backend, autocad) do
        delete_all_shapes()
        show_truss_deformation(results, factor=1)
      end
      disps
    end
  end

# backend(unity)
switch_to_layer("Deformation")
