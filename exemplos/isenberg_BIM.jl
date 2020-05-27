using Khepri
#using ScriptIt
# backend(autocad)
# backend(rhino)
backend(unreal)

### Script It
#Khepri.in_shape_collection(true)
##

no_doors = true

# Families _____________________________________________________________________

set_backend_family(default_wall_family(), unity, unity_material_family("Default/Materials/Plaster"))
set_backend_family(default_slab_family(), unity, unity_material_family("Default/Materials/Plaster"))
set_backend_family(default_column_family(), unity, unity_material_family("materials/metal/CopperBricks"))
set_backend_family(default_panel_family(), unity, unity_material_family("Default/Materials/GlassBlue"))
set_backend_family(default_curtain_wall_family().panel, unity, unity_material_family("Default/Materials/GlassBlue"))

set_backend_family(default_wall_family(), unreal, unreal_material_family("/Game/MS_ModernStuc/Surface/04_Stucco_2x2_M_termbdto/stuco.stuco"))
set_backend_family(default_slab_family(), unreal, unreal_material_family("/Game/MS_ModernStuc/Surface/04_Stucco_2x2_M_termbdto/stuco.stuco"))
set_backend_family(default_column_family(), unreal, unreal_material_family("/Game/MS_CleanBrick/Surface/07_Red_Brick_Wall_2x2_M_thmlaejg/RedBrickWall.RedBrickWall"))
set_backend_family(default_panel_family(), unreal, unreal_material_family("/Game/StarterContent/Materials/M_Glass.M_Glass")) # not blue
set_backend_family(default_curtain_wall_family().panel, unreal, unreal_material_family("/Game/StarterContent/Materials/M_Glass.M_Glass")) # not blue


copper_wall_fam = wall_family_element(default_wall_family())
set_backend_family(copper_wall_fam, unity, unity_material_family("materials/metal/CopperBricks"))

set_backend_family(copper_wall_fam, unreal, unreal_material_family("/Game/MS_CleanBrick/Surface/07_Red_Brick_Wall_2x2_M_thmlaejg/RedBrickWall.RedBrickWall"))

frame_width=0.1
frame_fam = column_family_element(default_column_family(), profile=rectangular_profile(frame_width, frame_width))
set_backend_family(frame_fam, unity, unity_material_family("Default/Materials/Steel"))

set_backend_family(frame_fam, unreal, unreal_material_family("/Game/StarterContent/Materials/M_Metal_Steel.M_Metal_Steel"))

pillar_fam = column_family_element(default_column_family(), profile=circular_profile(0.2))
set_backend_family(pillar_fam, unity, unity_material_family("Default/Materials/Plaster"))

set_backend_family(pillar_fam, unreal, unreal_material_family("/Game/MS_ModernStuc/Surface/04_Stucco_2x2_M_termbdto/stuco.stuco"))

door_fam = door_family_element(default_door_family())
set_backend_family(door_fam, unity, unity_material_family("Default/Materials/Glass"))

set_backend_family(door_fam, unreal, unreal_material_family("/Game/StarterContent/Materials/M_Glass.M_Glass"))

groud_fam = slab_family_element(default_slab_family())
set_backend_family(groud_fam, unity, unity_material_family("Default/Materials/Grass"))

set_backend_family(groud_fam, unreal, unreal_material_family("/Game/StarterContent/Materials/M_Ground_Grass.M_Ground_Grass"))

# Rhino material problem solved!
# Khepri.current_material(b::Khepri.RH)=nothing
# Khepri.current_material(arg, b::Khepri.RH)=nothing
# my_material(mat_path)=current_backend() == rhino ? current_material() : get_material(mat_path)

# DEFINITIONS __________________________________________________________________

pts_circle(center, r, alfa_init, alfa_end, n)=
    [center+vpol(r, alfa) for alfa in division(alfa_init, alfa_end, n)]

# SLABS

isenberg_slab(center, ri, re, alfa_init, alfa_proj, alfa_end, thick, n, is_first=false) =
    let pts_in = pts_circle(center#=+vz(thick)=#, ri, alfa_init, alfa_end, n)
        pts_out = pts_circle(center#=+vz(thick)=#, re, alfa_init, alfa_end, n)
        pts_out_proj = pts_circle(center#=+vz(thick)=#, re, alfa_init, alfa_proj, n)
        p_tang = center+vcyl(re, alfa_proj, 0#=+vz(thick)=#) # tangent point at the beginig of the projection
        amplitude = alfa_end-alfa_proj
        distance = amplitude*re/(pi/2) # rule of 3 for the distance
        v = vpol(distance, alfa_proj + amplitude) # normal the initial angle of the projection
        default_slab_family(slab_family_element(default_slab_family(), thickness=thick))
        is_first ?
            slab(closed_polygonal_path([pts_out_proj..., p_tang+v, pts_out[end], reverse(pts_in)...])) :
                slab(closed_polygonal_path([pts_out..., reverse(pts_in)...]))
    end

slabs(center, ri, re, alfa_init, alfa_proj, alfa_end, thick, floor_h, floors, n) =
    let h = floor_h#-thick/floors
        isenberg_slab(center, ri, re, alfa_init, alfa_proj, alfa_end, thick, n, true)
        for i in 1:floors
            isenberg_slab(center+vz(h*i), ri, re, alfa_init, alfa_proj, alfa_end, thick, n, false)
        end
    end

# BEAMS

bottom_aligned_rectangular_profile(Width::Real=1, Height::Real=1; width::Real=Width, height::Real=Height) =
  rectangular_path(xy(-width/2, 0), width, height)

isenberg_beam(p1, p2, dx, dy, alfa, bool=true) =
    let fam = bool ?
               column_family_element(default_column_family(), profile=rectangular_profile(dx, dy)) :
                column_family_element(default_column_family(), profile=bottom_aligned_rectangular_profile(dx, dy))
        free_column(p1, p2, alfa, family=fam,)
    end

small_beam(p1, p2, alfa) = isenberg_beam(p1, p2, beam_width, s_beam_lenght, alfa)
large_beam(p1, p2, dy) = isenberg_beam(p1, p2, beam_width, dy, pi, false)

outer_beams(center, h, r, alfa_init, alfa_end, n) =
    let pts_base = pts_circle(center, r, alfa_init, alfa_end, n)
        pts_top = pts_circle(center+vz(h), r, alfa_init, alfa_end, n)
        alfas = division(alfa_init, alfa_end, n)
        for (p,q,alfa) in zip(pts_base, pts_top, alfas)
            small_beam(p, q, alfa)
        end
    end

outer_beam_locs(center, h, r, alfa_init, alfa_end, d_min, d_max, n)=
    let proj_angle(dist, radius) = asin(dist/radius)
        p_tang = center+vpol(r, alfa_init) # tangent point at the beginig of the projection
        amplitude = alfa_end-alfa_init
        distance = amplitude*r/(pi/2) # rule of 3 for the distance
        v = vpol(1, alfa_init + amplitude) # normal the initial angle of the projection
        push = d_min/2+frame_width/2 # for the metal frame to be visible (not inside the beams)
        move_list(lst) = map(p -> p+v*(distance/n-push), lst)
        mv(lst, op) = map(p -> p+v*op(push), lst)
        v_psi(p, q) = π/2 -π/10 - sph_psi(q - p)

        pts_base_line = [p_tang+v*d for d in division(0, distance, n)]
        pts_top_proj = [center+vcyl(r, alfa_init+proj_angle(d, distance), h)
                            for d in division(0, distance, n)]

        # pts_glass = [p + vsph(dy, alfa_init, v_psi(p2, q2)) for (p, p2, q2, dy)
        #      in zip(pts_base_line, [pts_base_line[2:end]..., pts_base_line[end]],
        #             [pts_top_proj[2:end]..., pts_top_proj[end]],
        #             division(1e-9, d_max - d_min, n))]

        current_backend() == autocad || current_backend() == rhino ?
            pts_glass = [p+vz(dy) for (p,dy) in zip(pts_base_line, division(d_min, d_max*.9, n))] :
            pts_glass = [p + vsph(dy, alfa_init, v_psi(p, q)) for (p, q, dy)
                 in zip(pts_base_line, pts_top_proj, division(1e-9, d_max - d_min, n))]

        pts_base_line_mv = mv(pts_base_line, +)
        pts_top_proj_mv = mv(pts_top_proj, +)
        pts_glass_mv = mv(pts_glass, +)
        pts_base_move = move_list(pts_base_line)
        pts_top_move = move_list(pts_top_proj)
        pts_glass_move = move_list(pts_glass)
        pts_top_proj_next = mv(pts_top_proj, -)

        #RETURN:
        [pts_base_line, pts_base_line_mv, pts_base_move,
        pts_top_proj, pts_top_proj_mv, pts_top_move,
        pts_glass_mv, pts_glass_move, pts_top_proj_next]
    end

large_outer_beams_gen(list, d_min, d_max, n)=
    let pts_base_line = list[1][2:end]
        pts_top_proj = list[4][2:end]
        dys = division(d_min, d_max, n)
        for (p, q, dy) in zip(pts_base_line, pts_top_proj, dys)
            large_beam(p, q, dy)
        end
    end

large_outer_beams(center, h, r, alfa_init, alfa_end, d_min, d_max, n)=
    let lst = outer_beam_locs(center, h, r, alfa_init, alfa_end, d_min, d_max, n)
        large_outer_beams_gen(lst, d_min, d_max, n)
    end

# GLASS

isenberg_panel(pts)=
    begin
        panel(pts, family=default_panel_family())
        for (p,q) in zip(pts, [pts[2:end]...,pts[1]])
            free_column(p,q, family=frame_fam)
        end
    end

glass_line(center, h, r, alfa_init, alfa_end, n) =
    let pts_base = pts_circle(center, r, alfa_init, alfa_end, n)
        pts_top = pts_circle(center+vz(h), r, alfa_init, alfa_end, n)
        for (a, b, c, d) in zip(pts_base, pts_top, pts_top[2:end], pts_base[2:end])
            isenberg_panel([a, b, c, d])
        end
    end

straight_glass(center, r, alfa_init, alfa_end, floor_h, floors, n) =
    let pts_base = pts_circle(center-vz(center.z), r, alfa_init, alfa_end, n)
        amp = alfa_end-alfa_init
        if current_backend() == rhino
            for i in 0:floors-1
                glass_line(center+vz(floor_h*i), floor_h, r, alfa_init, alfa_end, n)
            end
        else
            fam = curtain_wall_family_element(default_curtain_wall_family(), max_panel_dx=amp*r/n, max_panel_dy=floor_h)
            curtain_wall(polygonal_path(pts_base), bottom_level=level(center.z), top_level=level(center.z+floor_h*floors), family=fam)
        end

    end

triangle_glass(center, h, r, alfa_init, alfa_end, d_min, d_max, n) =
    let p1 = center+vpol(r, alfa_end)
          p2 = p1+vz(h)
          p3 = outer_beam_locs(center, h, r, alfa_init, alfa_end, d_min, d_max, n)[1][end]
          v1 = (p3-p2)
          v2 = (p3-p1)
          v3 = (p1-p2)
          ps_cima(m) = [p2 + v1*d for d in division(0, 1, m, false)]
          ps_baixo(m) = [p1 + v2*d for d in division(0, 1, m, false)]
          ps_lado(m) = [p2 + v3*d for d in division(0, 1, m, false)]
       isenberg_panel([p1, p2, p3])
       for (p, q) in zip(ps_cima(n), ps_baixo(n))
          free_column(p, q, family=frame_fam)
       end
       for (p, q) in zip(ps_cima(6), ps_lado(6))
           distance(p, q)<1.0e-15 ? nothing : free_column(p, q, family=frame_fam)
       end
      end

large_beam_glass(center, h, r, alfa_init, alfa_end, d_min, d_max, n) =
    let beam_locs = outer_beam_locs(center, h, r, alfa_init, alfa_end, d_min, d_max, n)
        pts_base_line_mv = beam_locs[2][1:end-1]
        pts_top_proj_mv = beam_locs[5][1:end-1]
        pts_base_move = beam_locs[3][1:end-1]
        pts_top_move = beam_locs[6][1:end-1]
        pts_glass_mv = beam_locs[7][1:end-1]
        pts_glass_move = beam_locs[8][1:end-1]
        pts_top_proj_next = beam_locs[9][2:end]
        for (a,b,c,d,e,f,a2) in zip(pts_top_proj_mv, pts_top_move,
                                    pts_glass_move, pts_glass_mv,
                                    pts_base_line_mv, pts_base_move,
                                    pts_top_proj_next)
            isenberg_panel([a, b, c, d])
            isenberg_panel([c, d, e, f])
            isenberg_panel([a, b, a2])
        end
    end

# PILLARS

pillar_line(center, z, h, r, alfa_init, alfa_end, n)=
    for alfa in division(alfa_init, alfa_end, n, false)[2:end]
        current_backend() == autocad || current_backend() == rhino ?
            cylinder(center+vcyl(r, alfa, z), 0.2, center+vcyl(r, alfa, z+h)) :
            column(xy(center.x,center.y)+vpol(r, alfa), 0, level(z), level(z+h), pillar_fam)
    end

pillar_lines(center, z, h, inner_r, outer_r, alfa_init, alfa_end, n, m)=
    for r in division(inner_r, outer_r, m-1)
        pillar_line(center, z, h, r, alfa_init, alfa_end, n)
    end

pillars_z(center, inner_r, outer_r, alfa_init, alfa_end, n, m, floor_h, floors)=
    for z in division(0, floor_h*floors, floors, false)
        pillar_lines(center, z, floor_h, inner_r, outer_r, alfa_init, alfa_end, n, m)
    end

# outer WALLS

isenberg_wall(p1, p2, h, thick) =
    let vp = p2-p1
        wall([p1, p2], bottom_level=level(p1.z), top_level=level(h), family=copper_wall_fam)
    end

# INNER WALLS

wall_trans_set(center, z, alfa, inner_r, outer_r, corridor_w, h, p_offset, m_pillars, broken)=
    let p_in = center+vpol(inner_r, alfa)
        p_out = center+vpol(outer_r, alfa)
        v_in = vpol(1, alfa+pi)
        v_out = vpol(1, alfa)
        pillar_rs = division(inner_r+p_offset, outer_r-p_offset, m_pillars-1)
        p_wall_in = center +vpol(pillar_rs[2], alfa)
        p_wall_out = center +vpol(pillar_rs[3], alfa)
        p_wall_mid_in = p_wall_in+v_in*corridor_w
        p_wall_mid_out = p_wall_out+v_out*corridor_w

        with(default_level, level(z), default_level_to_level_height, h) do
            broken ?
                begin
                    wall([p_in, p_wall_mid_in])
                    wall([p_wall_in, p_wall_out])
                    wall([p_wall_mid_out, p_out])
                end :
                wall([p_out, p_wall_in])
        end
    end

wall_trans_floor(center, z, alfa_init, alfa_end, inner_r, outer_r, corridor_w, h, p_offset, n_pillars, m_pillars; broken=true)=
    for alfa in division(alfa_init, alfa_end, n_pillars, false)[2:end]
        wall_trans_set(center, z, alfa, inner_r, outer_r, corridor_w, h, p_offset, m_pillars, broken)
    end

curve_wall(center, z, alfa_init, alfa_end, r, h)=
    wall(arc_path(center, r, alfa_init, alfa_end-alfa_init), bottom_level=level(z), top_level=level(z+h))
    # wall(polygonal_path(pts_circle(center, r, alfa_init, alfa_end, 100)),
    #             bottom_level=level(z), top_level=level(z+h))

doors(w, pts) = no_doors ? w : foreach(pt->add_door(w, pt, door_fam), pts)
ang_dist(r, ang) = ang*r

curve_wall_broken(center, z, alfa_init, alfa_end, r, h, n_corridors, has_doors=false)=
    let amplitude = (alfa_end-alfa_init)/n_corridors
        alfas = division(alfa_init, alfa_end, n_corridors, false)
        mark1 = floor(Int, n_corridors*(1/3))
        mark2 = ceil(Int, n_corridors*(2/3))
        door_w = 1

        function curve_wall_with_doors(angs, n_doors)
            local_amp = angs[end]-angs[1]
            door_locs = [x(a*r+local_amp*r/n_doors-door_w*1.2)
                            for a in division(0, local_amp, n_doors, false)]
            has_doors ?
                doors(curve_wall(center, z, angs[1], angs[end], r, h), door_locs) :
                curve_wall(center, z, angs[1], angs[end], r, h)
        end

        g1 = alfas[2:mark1]
        g2 = alfas[mark1+1:mark2]
        g3 = alfas[mark2+1:end]
        curve_wall_with_doors(g1, length(g1)-1)
        curve_wall_with_doors(g2,length(g2)-1)
        curve_wall_with_doors(g3, length(g3)-1)
    end

curve_walls(center, z, alfa_init, alfa_end, inner_r, outer_r, corridor_w, h, n_pillars, m_pillars, wall_thick)=
    let pillar_rs = division(inner_r, outer_r, m_pillars-1)
        r_in = pillar_rs[2]
        r_out = pillar_rs[3]
        r_in_corridor = r_in-corridor_w
        r_out_corridor = r_out+corridor_w

        door_locs(r, angs) = [x(ang+wall_thick/2) for ang in angs]
        angs_in = division(0, ang_dist(r_in_corridor, alfa_end-alfa_init), n_pillars, false)
        angs_out = division(0, ang_dist(r_out_corridor, alfa_end-alfa_init), n_pillars, false)

        doors(curve_wall(center, z, alfa_init, alfa_end, r_in_corridor, h), door_locs(r_in_corridor, angs_in))
        curve_wall_broken(center, z, alfa_init, alfa_end, r_in, h, n_pillars, false)
        curve_wall_broken(center, z, alfa_init, alfa_end, r_out, h, n_pillars, true)
        doors(curve_wall(center, z, alfa_init, alfa_end, r_out_corridor, h), door_locs(r_out_corridor, angs_out))
    end

rooms_floor(center, z, alfa_init, alfa_end, inner_r, outer_r, corridor_w, h, p_offset, n_pillars, m_pillars, wall_thick)=
    begin
        wall_trans_floor(center, z, alfa_init, alfa_end, inner_r, outer_r, corridor_w, h, p_offset, n_pillars, m_pillars)
        curve_walls(center, z, alfa_init, alfa_end, inner_r+p_offset, outer_r-pillar_offset, corridor_w, h, n_pillars, m_pillars, wall_thick)
    end

floor_zero(center, z, alfa_init, alfa_proj, alfa_end, inner_r, outer_r, corridor_w, p_offset, n_pillars, m_pillars, h, wall_thick)=
    let pillar_rs = division(inner_r+p_offset, outer_r-pillar_offset, m_pillars-1)
        r_in = pillar_rs[2]
        door_w = 1

        amp_tot = alfa_end-alfa_init
        amp_circle = alfa_proj-alfa_init
        amp_room = amp_tot/n_pillars
        proportion = amp_circle/amp_tot
        n_rooms = floor(n_pillars*proportion)
        amp_wall_area = alfa_init+n_rooms*amp_room
        n_walls = floor(Int, proportion*n_pillars/2)

        ang_dists = division(0, ang_dist(r_in, n_rooms*amp_room), n_walls)
        door_locs_1 = [x(ang+wall_thick/2) for ang in ang_dists[1:end-1]]
        door_locs_2 = [x(ang-door_w-wall_thick/2) for ang in ang_dists[2:end]]

        wall_trans_floor(center, z, alfa_init, amp_wall_area, inner_r, outer_r, corridor_w, h, p_offset, n_walls, m_pillars, broken=false)
        doors(curve_wall(center, z, alfa_init, amp_wall_area, r_in, h), [door_locs_1..., door_locs_2...])
    end

floor_plans(center, alfa_init, alfa_end, inner_r, outer_r, corridor_w, h, floors, p_offset, n_pillars, m_pillars, wall_thick)=
    for z in division(h, h*floors, floors-1, false)
        rooms_floor(center, z, alfa_init, alfa_end, inner_r, outer_r, corridor_w, h, p_offset, n_pillars, m_pillars, wall_thick)
    end

# GROUND

ground() =
  slab(closed_polygonal_path([xyz(-1000,1000,-0.05), xyz(1000,1000,-0.05), xyz(1000,-1000,-0.05), xyz(-1000,-1000,-0.05)]), level(0), groud_fam)

# PARAMETERS ___________________________________________________________________

# building shape
b_center = u0()
floor_height = 3
n_floors = 3
inner_radius = 10
building_width = 15

# details
slab_thickness = .4
wall_thickness = .4
#glass_thick = 0.05
beam_width = .4
s_beam_lenght = .4
l_beam_lenght = 3.5

# n points, n beams
n_beams_total = 60
n_crv_pts = 100

# Pilars and inside walls
n_pillars = n_beams_total/3
m_pillars = 4
pillar_offset = 1
corridor_width = 2

# circle rotation
begin_circle = 0
begin_projection = pi

# GENEATE ______________________________________________________________________
delete_all_shapes()
clear_trace!()
empty!(Khepri.collected_shapes())
ground()

isenberg()=
    let b_height = floor_height*n_floors
        outer_radius = inner_radius + building_width
        n_small_beams = ceil(Int, n_beams_total*2/3)
        n_large_beams = ceil(Int, n_beams_total/4)
        end_circle = begin_projection + pi/2

        #slabs(b_center, inner_radius, outer_radius, begin_circle, begin_projection, end_circle, slab_thickness, floor_height, n_floors, n_crv_pts)
        #outer_beams(b_center, b_height, outer_radius, begin_circle, begin_projection, n_small_beams)
        #large_outer_beams(b_center, b_height, outer_radius, begin_projection, end_circle, beam_width, l_beam_lenght, n_large_beams)

        isenberg_wall(b_center+vpol(inner_radius, begin_circle), b_center+vpol(outer_radius, begin_circle), b_height, wall_thickness)
        isenberg_wall(b_center+vpol(inner_radius, end_circle), b_center+vpol(outer_radius, end_circle), b_height, wall_thickness)

        #pillars_z(b_center, inner_radius+pillar_offset, outer_radius-pillar_offset, begin_circle, end_circle, n_pillars, m_pillars, floor_height, n_floors)
        #floor_plans(b_center, begin_circle, end_circle, inner_radius, outer_radius, corridor_width, floor_height, n_floors, pillar_offset, n_pillars, m_pillars, wall_thickness)
        #floor_zero(b_center, 0, begin_circle, begin_projection, end_circle, inner_radius, outer_radius, corridor_width, pillar_offset, n_pillars, m_pillars, floor_height, wall_thickness)

        #straight_glass(b_center, inner_radius, begin_circle, end_circle, floor_height, n_floors, n_small_beams) # patio
        #straight_glass(b_center+vz(floor_height), outer_radius, begin_projection, end_circle, floor_height, n_floors-1, n_large_beams*2) # inside entrance
        #straight_glass(b_center, outer_radius, begin_circle, begin_projection, floor_height, n_floors, n_small_beams) # outside
        #triangle_glass(b_center, b_height, outer_radius, begin_projection, end_circle, beam_width, l_beam_lenght, n_large_beams)
        #large_beam_glass(b_center, b_height, outer_radius, begin_projection, end_circle, beam_width, l_beam_lenght, n_large_beams)
    end

isenberg()

#= VARIATIONS ___________________________________________________________________

b_center = x(90)
floor_height = 4
inner_radius = 15
building_width = 16
l_beam_lenght = 4.2
n_beams_total = 60
begin_circle = pi/4
begin_projection = pi-pi/4
isenberg()

b_center = x(160)
floor_height = 2.8
n_floors = 5
inner_radius = 10
building_width = 20
l_beam_lenght = 3.9
n_beams_total = 90
begin_circle = pi/4
begin_projection = pi/2
isenberg()

b_center = x(-80)
n_floors = 2
l_beam_lenght = 2.3
inner_radius = 12
building_width = 12
n_beams_total = 60
begin_circle = pi/10
begin_projection = 3pi/2
isenberg()
=#

# SET VIEW _____________________________________________________________________

# all 4
#set_view(xyz(46.199764251708984,-248.9051971435547,155.9777374267578), xyz(46.162139892578125,-247.9820556640625,155.59512329101562), 35.0)
#set_view(xyz(25.20522117614746,-290.70367431640625,0.6726011037826538), xyz(25.23943328857422,-289.7042541503906,0.6702404618263245), 35.0)
#set_view(xyz(39.174461364746094,-21.866865158081055,302.56634521484375), xyz(39.174461364746094,-21.866865158081055,301.56634521484375), 35.0)

# one
#set_view(xyz(-3.504779815673828,-76.60165405273438,51.335548400878906), xyz(-3.52087664604187,-75.75193786621094,50.80855178833008), 35.0)
#set_view(xyz(36.171138763427734,-46.27556228637695,55.54283905029297), xyz(35.66852569580078,-45.717838287353516,54.882293701171875), 35.0)
#set_view(xyz(41.169586181640625,-62.254329681396484,0.18631237745285034), xyz(40.569915771484375,-61.461875915527344,0.297719269990921), 35.0)
#set_view(xyz(14.000951766967773,-39.41598892211914,1.5413329601287842), xyz(13.29345703125,-38.73129653930664,1.716399908065796), 18)
#set_view(xyz(-29.070680618286133,-29.295852661132812,1.4947516918182373), xyz(-28.38509750366211,-28.59096908569336,1.6767290830612183), 18.0)
# set_view(xyz(-13.535767555236816,-24.18745994567871,0.5281195640563965), xyz(-14.007678985595703,-23.402803421020508,0.9301314353942871), 18.0)
# set_view(xyz(-13.325005531311035,-24.788440704345703,0.2543081045150757), xyz(-13.801456451416016,-24.022090911865234,0.685242235660553), 18.0)
# set_view(xyz(13.179197311401367,6.593484401702881,1.3748373985290527), xyz(12.205687522888184,6.3650078773498535,1.3660070896148682), 18.0)

#render_dir("C:\\Users\\Renata\\Google Drive\\PAPERS\\2020_CAADRIA_IAD")
#start_film("Isenberg_var")
#save_film_frame()
