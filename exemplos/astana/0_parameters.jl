using Khepri
backend(unity)
# backend(robot)
###########
# Khepri.select_shapes(p::String, b::Khepri.Unity)= [select_shape(p, b)]
# Khepri.in_shape_collection(true)
# clear_trace!()
#############
# backend(autocad)
#delete_all_shapes()
archicad = nothing
#revit = nothing

# INDEX ________________________________________________________________________

# PARAMETERS
# FAMILIES
# VARIATIONS
# UNITY ASSETS

# PARAMETERS ___________________________________________________________________

astana_center_p = u0()

block_h = 15
block_l = 14
mid_block_levels = 4
out_block_levels = 3
mid_floor_h = block_h/mid_block_levels
out_floor_h = block_h/out_block_levels

mid_block_r = Parameter(35)
slab_thickness = 0.5
wall_thickness = 0.2
library_door = 2
door_width = 1
spiral_stairs_width = 2
glass_thickness = 0.05

column_r = 0.25
column_d = 2*column_r

railing_height = 1
n_pts_for_curv = 10

n_frames = 36
n_frames_for_frames = 36
frame_x = Parameter(3*block_l) #for more twists of the façade Parameter(2*block_l + 4)
frame_y = Parameter(3*block_h + 8)
x_divisions = 4
y_divisions = 7
skin_rotation = pi
global_dfi = 2pi/n_frames

ss_div = 2
panel_positions = 9
n_panels_per_strip(xyPdivision) = (xyPdivision + 1)*(ss_div + 1)
n_panel_frames = n_frames*(ss_div + 1)
big_core_w = 3
n_big_cores = 4

# FAMILIES _____________________________________________________________________

astana_beam_family = beam_family_element(default_beam_family(), profile=rectangular_profile(column_d, column_d))

#set_backend_family(beam_family, unity, unity_material_family("Materials/Concrete/Concrete2"))
#set_backend_family(column_family, unity, unity_material_family("Materials/Metal/Aluminum"))

beam_family_for_slab =
    let dz_slb = 2block_h/n_frames
        beam_family_element(default_beam_family(),
                            profile=top_aligned_rectangular_profile(width=slab_thickness,
                                                                    height=dz_slb))
    end
set_backend_family(beam_family_for_slab, unity, unity_material_family("Default/Materials/Concrete"))

beam_family_for_connections =
    beam_family_element(default_beam_family(), profile=top_aligned_rectangular_profile(width=column_d, height=column_d))

beam_family_for_secondary_structure =
    beam_family_element(astana_beam_family, profile=rectangular_profile(column_r/3, column_r/3))

glass_family = wall_family(thickness=glass_thickness)
set_backend_family(glass_family, unity, unity_material_family("Default/Materials/Glass"))

photopanel_family = panel_family(thickness=glass_thickness)
set_backend_family(photopanel_family, unity, unity_material_family("Default/Materials/Steel"))

default_slab_family(slab_family_element(default_slab_family(), thickness=slab_thickness))

default_column_family(column_family_element(default_column_family(), profile=rectangular_profile(column_d, column_d)))

facade_conect_fam = column_family_element(default_column_family(), profile=rectangular_profile(column_d, column_d))
set_backend_family(facade_conect_fam, unity, unity_material_family("Default/Materials/Aluminum"))

#set_backend_family(beam_family, unity, unity_material_family("Materials/Concrete/Concrete2"))
#set_backend_family(column_family, unity, unity_material_family("Materials/Metal/Aluminum"))
#panel_family = panel_family_element(default_panel_family(), thickness=glass_thickness)

# VARIATIONS ___________________________________________________________________

rot_base_point(p, fi) = p + vcyl(mid_block_r() + block_l/2, fi, 1.6*block_h)

ro_f = 0
psiA = 0
psiB = 0
psiC = 0
psiD = 0

define_dependent_variables!() =
  begin
    global ro_f = sqrt((frame_x()/2)^2 + (frame_y()/2)^2)
    global psiA = atan(frame_y()/2, frame_x()/2)
    global psiB = pi - atan(frame_y()/2, frame_x()/2)
    global psiC = pi + atan(frame_y()/2, frame_x()/2)
    global psiD = -atan(frame_y()/2, frame_x()/2)
  end

define_dependent_variables!()

experiment_1() =
  begin
    global astana_center_p = x(300)
    frame_x(3*block_h + 8)
    global x_divisions = 6
    global y_divisions = 6
    global skin_rotation = 2*pi
    define_dependent_variables!()
  end


experiment_2() =
  begin
    frame_x(2*block_l + 4)
    global x_divisions = 6
    global y_divisions = 6
    global skin_rotation = pi
    global astana_center_p = x(600)
    mid_block_r(60)
    global n_frames_for_frames = 20
    global x_divisions = 3
    global y_divisions = 5
    define_dependent_variables!()
  end

# UNITY ASSETS _________________________________________________________________

current_backend() == unity ?
  begin
    global people = [block("Prefabs/People/Person$(i)") for i in 1:6]
    global astana_bookshelf = block("Prefabs/BookShelves/AstanaBookshelf10")
    global astana_macbook = block("Prefabs/Macbook/Macbook")
    global astana_torch = block("Prefabs/Torch/Torch")
  end :
  nothing

# GROUND _______________________________________________________________________

sin_u_mul_v(n) =
  map_division((u, v) -> xyz(u, v, 5*sin(u*v/10000)),
               -200pi, 200pi, n,
               -200pi, 200pi, n)

ground() =
  with(current_material, get_material("Default/Materials/Grass")) do
    surface_grid(sin_u_mul_v(100))
    #with(current_material, get_material("Materials/Plaster/Plaster1")) do
        #surface_grid([[xyz(-1000,1000,0), xyz(1000,1000,-.1)], [xyz(-1000,-1000,0), xyz(1000,-1000,0)]])
  end
