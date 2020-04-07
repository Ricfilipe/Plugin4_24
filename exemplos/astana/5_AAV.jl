include("4_panel_analysis.jl")

# INDEX ________________________________________________________________________

# ALGORITHMIC ARCHITECTURAL VISUALIZATION (AAV)

# RENDER SETTINGS
# -- Camera paths

# ASTANA PANNING
# -- Panning left - patio
# -- Panning Up - patio

# ASTANA TRACKING
# -- Spiral - around the building
# -- Spiral variations
# -- Linear - paralel to the entrance to the patio
# -- Linear - entering the patio
# -- Over the façade

# ASTANA WALKTROUGHS
# -- mega-path inside
# -- mega-path outside

# ASTANA ZOOM
# RENDER

#__RENDER SETTINGS____________________________________________________________________

frame_rate_pi = 200
transit_rate = 40
curv_rate = 20
man_h = 1.5
dfi = 2*pi/n_frames

# Camera paths

linear_path(p1, p2, frames) =
  let v = p2 - p1
    map_division(t -> p1 + v*t, 0, 1, frames)
  end

rotation_path(center, r, start_phi, stop_phi, h, frames) =
  map(fi -> center+vcyl(r, fi, h),
      division(start_phi, stop_phi, frames))

spiral_path(center, r, r_delta, start_phi, stop_phi, h, h_delta, frames) =
  map((fi, r, h) -> center+vcyl(r, fi, h),
      division(start_phi, stop_phi, frames),
      division(r, r*r_delta, frames),
      division(h, h*h_delta, frames))


# ASTANA PANNING _______________________________________________________________

# Panning left - patio
Astana_Panning() =
  let (first_fi, last_fi, isexterior) = (pi, pi/2, 1)
    panning_path = rotation_path(astana_center_p+vz(man_h),
                                 mid_block_r(),
                                 first_fi,
                                 last_fi,
                                 2.5*block_h,
                                 frame_rate_pi/2)
    panning(astana_center_p+vz(man_h), panning_path)
  end

# Panning Up - patio
Astana_Panning_Up() =
  let isexterior = 1
      p1 = astana_center_p+vcyl(mid_block_r(), 3.6*pi/4+pi, man_h)
      p2 = astana_center_p+vcyl(mid_block_r(), 3.6*pi/4+pi, 2.5*block_h)
      upward_panning_path = linear_path(p1, p2, frame_rate_pi)
      panning(astana_center_p+vz(man_h), upward_panning_path)
  end



# ASTANA TRACKING ______________________________________________________________

# Spiral - around the building
Astana_Tracking_Cyl() =
  let (first_fi, last_fi, isexterior) = (0, 4*pi, 1)
    cylindrical_tracking_path = spiral_path(astana_center_p,
                                            mid_block_r(),
                                            4,
                                            first_fi,
                                            last_fi,
                                            5*block_h,
                                            1//5,
                                            4*frame_rate_pi)
    track_still_target(cylindrical_tracking_path, astana_center_p+vz(block_h))
  end

# Spiral variations
Astana_Tracking_Variations() =
  let (first_fi, last_fi, isexterior, frames) = (2*pi, 0, 1, 2*frame_rate_pi)
    cylindrical_tracking_path = spiral_path(astana_center_p,
                                            mid_block_r()*5,
                                            1//3,
                                            first_fi,
                                            last_fi,
                                            8*block_h,
                                            0.8,
                                            frames)
    for (r, x, y, cam) in zip(division(120, 30, frames),
                              division(2*block_l+4, 3*block_h+8, frames),
                              division(3*block_h+8, 2*block_h-4, frames),
                              cylindrical_tracking_path)
      let (mid_block_r, frame_x, frame_y) = (r, x, y)
        #delete_all_shapes()
        #ground()
        astana(p)
        set_view(cam, astana_center_p+vz(-20), default_lens())
        save_film_frame()
      end
    end
  end

# Linear - paralel to the entrance to the patio
Astana_Tracking_Line() =
  let isexterior = 1
      p1 = astana_center_p+vcyl(2.2*mid_block_r(), 7//8*pi, man_h)
      p2 = astana_center_p+vcyl(2.2*mid_block_r(), 1*pi, man_h)
      camera = p1+vx(-20)
      linear_tracking_path = linear_path(p1, p2, frame_rate_pi*1//4)
      track_moving_target(camera, linear_tracking_path)
  end

# Linear - entering the patio
Astana_Tracking_Forth_Walk() =
  let isexterior = 1
      p1 = astana_center_p+vcyl(2.2*mid_block_r(), 3.6*pi/4, man_h)
      p2 = astana_center_p+vcyl(0, 3.6*pi/4+pi, man_h)
      p3 = astana_center_p+vcyl(mid_block_r(), 3.6*pi/4+pi, man_h)
      linear_tracking_path = linear_path(p1, p2, frame_rate_pi*1//4)
      track_still_target(linear_tracking_path, p3)
  end


# Over the façade
Astana_Tracking_Façade() =
  let isexterior = 1
      façade_path =
           map((fi, psi) -> rot_base_point(p, fi)+vsph(1+ro_f, fi, psi+pi/2), division(2*pi, -2*pi, 2*n_frames_for_frames), division(-(pi/2), -(pi/2)+2*skin_rotation, 2*n_frames_for_frames)),
           #allong the smaller edge)
    #alllong the bigger edge
    spline(façade_path)
  end
 #show camera path with a spline in CAD only


# ASTANA WALKTROUGHS ___________________________________________________________

clock_wise_inner_ring = map((fi, dz, ri) -> astana_center_p+vcyl(rfunc(ri)+2*man_h, fi, dz),
                            division(4*pi+dfi, 3*pi+dfi, frame_rate_pi),
                            division(2*block_h/4, 1.8*man_h+block_h/4+block_h, frame_rate_pi),
                            division(0, pi, frame_rate_pi))

transition_inner_middle = map((dz, ri) -> astana_center_p+vcyl(ri, 3*pi+dfi, dz),
                              division(1.8*man_h+block_h/4+block_h, man_h+block_h/4+block_h, transit_rate),
                              division(rfunc(pi)+2*man_h, 1.37*mid_block_r(), transit_rate))

mid_out_value = (7*pi)/2+dfi

anti_clock_middle_ring = map(fi -> astana_center_p+vcyl(mid_block_r()+library_door/2, fi, man_h+block_h/4+block_h),
                             division(3*pi+dfi, mid_out_value, frame_rate_pi/2))
transition_middle_outer = map(ri -> astana_center_p+vcyl(ri, mid_out_value, man_h+block_h/4+block_h),
                              division(mid_block_r()+library_door/2, rfunc((5*pi)/2)+block_l, transit_rate))
anti_clock_outer_space_up = map((fi, dz, ri) -> astana_center_p+vcyl(ri, fi, dz),
                                division(mid_out_value, 4*pi+dfi, frame_rate_pi/2),
                                division(man_h+block_h/4+block_h, 3*block_h, frame_rate_pi/2),
                                division(rfunc((5*pi)/2)+block_l, block_l*1.2+rfunc(2*pi), frame_rate_pi/2))
anti_clock_outer_space_down = map((fi, dz, ri) -> astana_center_p+vcyl(ri, fi, dz),
                                  division(4*pi+dfi, 4.5*pi+dfi, frame_rate_pi/2),
                                  division(3*block_h, 2.2*block_h, frame_rate_pi/2),
                                  division(block_l*1.2+rfunc(2*pi), mid_block_r()+block_l, frame_rate_pi/2))

mega_path_inside = vcat(clock_wise_inner_ring[round(Int, (length(clock_wise_inner_ring)-1)/2+1):end],
                        #remove drop for complete sequence,
                        transition_inner_middle,
                        anti_clock_middle_ring)

mega_path_outside = [anti_clock_outer_space_up...,
                     anti_clock_outer_space_down...]

Astana_walkthrough() =
    walkthrough(mega_path_inside, 20)

Astana_inner_birds_eye() =
    walkthrough(mega_path_outside, 20)

mid_middle_ring = map(fi -> astana_center_p+vcyl(mid_block_r()+block_l/2, fi, man_h+block_h/4+block_h),
                                 division(0, 2pi, 2*frame_rate_pi))
Astana_inner_ring_middle() =
    walkthrough(mid_middle_ring, 20)


# ASTANA ZOOM __________________________________________________________________

computer(loc, ang) =
  #current_backend_name() == "ArchiCAD" ?
  #object("MacBookPro 18", loc, level=level(loc.z), angle=ang, properties=["angle", ang]) :
  rotate(
        block_instance(astana_macbook, loc, 1),
        ang, loc)
  #rotate(box(loc, 0.393, 0.267, 0.25), ang, loc)

#computer(xyz(-4, -40, 23.25), 2*pi/3) # archiCAD
#computer(xyz(-4.1, -39.9, 23.14), -pi/3) # unity

Astana_Dolly_Zoom_Back() =
  dolly_effect_back(0.08,
                    xyz(-3.46, -39.04, 23.4),
                    xyz(-4, -40, 23.25),
                    16,
                    60)

Astana_Dolly_Zoom_Forth() =
  dolly_effect_forth(0.06,
                     xyz(-0.3669, -37.3, 23.9),
                     xyz(-4, -40, 23.3),
                     120,
                     60)
