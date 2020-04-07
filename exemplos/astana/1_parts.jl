include("0_parameters.jl")

# INDEX ________________________________________________________________________

# LIVING BLOCK SLABS
# -- round middle slabs
# -- slab-piece definition
# -- outer top and bottom slabs
# -- outer middle slabs

# SLAB BEAMS
# COLUMNS AND CROSSBEAMS

# LIBRARY WALLS
# -- torches for Game of Thrones theme presentation

# CORES
# ALL AROUND SPIRAL STAIRS


# LIVING BLOCK SLABS ___________________________________________________________

#round middle slabs

mid_block_slab(p, z) =
  slab(circular_path(p, mid_block_r() + block_l), level(z),
       openings=[circular_path(p, mid_block_r())])


mid_block_slabs(p) =
  map(z -> mid_block_slab(p, z),
      division(block_h, 2*block_h, mid_block_levels))

# slab-piece definition

block_slab(p, fi0, r0, r1, z, in_div_out) =
  begin
      all = l -> l
      cdr = l -> l[2:end]
    dfi = 2*pi/n_frames
    blksld(p, _add_r1, _add_r2, xr1, xr2, is_f1, is_f2, func) =
      with(default_level, level(z)) do
        slab([func(map((fi, ri) -> p + vpol(xr1*ri + _add_r1, fi),
                           division(fi0, fi0 + dfi, n_pts_for_curv, is_f1),
                           division(r0, r1, n_pts_for_curv, is_f1)))...,
              map((fi, ri) -> p + vpol(xr2*ri + _add_r2, fi),
                           division(fi0 + dfi, fi0, n_pts_for_curv, is_f2),
                           division(r1, r0, n_pts_for_curv, is_f2))...])
      end

    if in_div_out == 1 && block_h - 3 < z < 3 + block_h*2 && fi0 > 2*pi
      if fi0 == 2*pi + dfi
        blksld(p, 0, mid_block_r(), 1, 0, true, false, all)
      else
        blksld(p, 0, mid_block_r(), 1, 0, true, true, all)
      end
    elseif in_div_out == 1 && block_h - 3 < z < 3 + block_h*2 && fi0 > 0
      if fi0 <= dfi
        blksld(p, mid_block_r() + block_l, block_l, 0, 1, true, true, cdr)
      else
        blksld(p, mid_block_r() + block_l, block_l, 0, 1, true, true, all)
      end
    elseif in_div_out == 0 && block_h < z < 2*block_h && fi0 > 2*pi
      if fi0 == 2*pi + dfi
        blksld(p, 0, mid_block_r(), 1, 0, true, false, all)
      else
        blksld(p, 0, mid_block_r(), 1, 0, true, true, all)
      end
    elseif in_div_out == 0 && block_h < z < 2*block_h && fi0 > 0
      if fi0 <= dfi
        blksld(p, mid_block_r() + block_l, block_l, 0, 1, true, true, cdr)
      else
        blksld(p, mid_block_r() + block_l, block_l, 0, 1, true, true, all) #AML
      end
    else
      blksld(p, 0, block_l, 1, 1, true, true, all)
    end
  end


# outer top and bottom slabs

rfunc(ri) =
  mid_block_r() - block_l*sin(0.5*ri)

out_block_out_slabs(p) =
  begin
    map((dz, ri1, ri2, fi0) -> [block_slab(p, fi0, rfunc(ri2), rfunc(ri1), dz, 0),
                                block_slab(p, fi0, rfunc(ri2), rfunc(ri1), dz + block_h, 0)][end],
        division(0, 2*block_h, n_frames, false),
        division(0, 2*pi, n_frames, false),
        division(2*pi/n_frames, 2*pi + 2*pi/n_frames, n_frames, false),
        division(4*pi, 2*pi, n_frames, false))
    map((dz, ri1, ri2, fi0) -> [block_slab(p, fi0, rfunc(ri2), rfunc(ri1), dz, 0),
                                block_slab(p, fi0, rfunc(ri2), rfunc(ri1), dz + block_h, 0)][end],
        division(2*block_h, 0, n_frames, false),
        division(2*pi, 4*pi, n_frames, false),
        division(2*pi + 2*pi/n_frames, 4*pi + 2*pi/n_frames, n_frames, false),
        division(2*pi, 0, n_frames, false))
  end


# outer middle slabs
out_block_in_slabs(p, fi0, fir, z0) =
  let zmid = block_h/mid_block_levels
      dz = block_h/(n_frames/2)
      dfi = pi/(n_frames/2)
      r = rfunc(fir)
      r1 = rfunc(fir + dfi)
      r2 = rfunc(fir + 2*dfi)
      r3 = rfunc(fir + 3*dfi)
    if fi0 > 37*pi/18
      block_slab(p, fi0, r1, r, z0 + zmid, 1)
      block_slab(p, fi0, r1, r, z0 + 3*zmid, 1)
      block_slab(p, fi0 - dfi, r2, r1, z0 + dz + dz + dz + zmid, 1)
      block_slab(p, fi0 - dfi, r2, r1, z0 + 3*zmid, 1)
      block_slab(p, fi0 - dfi - dfi, r3, r2, z0 + dz + dz + dz + zmid, 1)
      block_slab(p, fi0 - dfi - dfi, r3, r2, z0 + 3*zmid, 1)
      out_block_in_slabs(p, fi0 - 3*dfi, fir + 3*dfi, z0 + 3*dz)
    elseif fi0 > dfi + 0
      block_slab(p, fi0, r1, r, z0 + zmid, 1)
      block_slab(p, fi0, r1, r, z0 + 3*zmid, 1)
      block_slab(p, fi0 - dfi, r2, r1, z0 + zmid, 1)
      block_slab(p, fi0 - dfi, r2, r1, z0 + 3*zmid - 3*dz, 1)
      block_slab(p, fi0 - dfi - dfi, r3, r2, z0 + zmid, 1)
      block_slab(p, fi0 - dfi - dfi, r3, r2, z0 + 3*zmid - 3*dz, 1)
      out_block_in_slabs(p, fi0 - 3*dfi, fir + 3*dfi, z0 - 3*dz)
    else
      true
    end
  end


# SLAB BEAMS ___________________________________________________________________

slab_beam(p, fi0, ri, z) =
  begin
    dz_slb = (2*block_h)/n_frames
    dfi(ri) =
      atan(slab_thickness/(2*ri))

    let (p1i, p2i, p3i, p1o, p2o, p4o) = (p + vcyl(ri, fi0 - dfi(ri), z + dz_slb),
                                          p + vcyl(ri + block_l, fi0 - dfi(ri + block_l), z + dz_slb),
                                          p + vcyl(mid_block_r(), fi0 - dfi(mid_block_r()), z + dz_slb),
                                          p + vcyl(ri, fi0 + dfi(ri), z), p + vcyl(ri + block_l, fi0 + dfi(ri + block_l), z),
                                          p + vcyl(mid_block_r() + block_l, fi0 + dfi(mid_block_r() + block_l), z))
      with(default_beam_family, beam_family_for_slab) do
        if fi0 == 4*pi && z == block_h - slab_thickness
          true
        elseif p3i.x == p1i.x
          true
        elseif p4o.x == p2o.x
          true
        elseif block_h < z < 2*block_h && fi0 > 2*pi
          beam(p3i, p1i)
        elseif block_h < z < 2*block_h && fi0 > 0
          beam(p4o, p2o)
        elseif fi0 > 2*pi
          beam(p2i, p1i)
        else
          beam(p1o, p2o)
        end
      end
    end
  end

out_slab_beams(p) =
  begin
    map((dz, ri, fi0) -> [slab_beam(p, fi0, rfunc(ri), dz),
                          slab_beam(p, fi0, rfunc(ri), dz + block_h)][end],
        division(-slab_thickness, 2*block_h - slab_thickness, n_frames, false),
        division(2*pi/n_frames, 2*pi + 2*pi/n_frames, n_frames, false),
        division(4*pi, 2*pi, n_frames, false))
    map((dz, ri, fi0) -> [slab_beam(p, fi0, rfunc(ri), dz),
                          slab_beam(p, fi0, rfunc(ri), dz + block_h)][end],
        division(2*block_h - slab_thickness, -slab_thickness, n_frames, false),
        division(2*pi + 2*pi/n_frames, 4*pi + 2*pi/n_frames, n_frames, false),
        division(2*pi, 0, n_frames, false))
  end


# COLUMNS AND CROSSBEAMS _______________________________________________________

column_rotation(p, fi, r, z, h) =
  let (p1, p2) = (p + vcyl(r, fi, z), p + vcyl(r, fi, z + h))
    if h == 0
      []
    else
      free_column(p1, p2, angle=fi)
    end
  end

mid_columns(p) =
  map(fi -> [column_rotation(p, fi, mid_block_r() + 0.25, block_h, block_h - slab_thickness),
            column_rotation(p, fi, mid_block_r() + block_l + -0.25, block_h, block_h - slab_thickness)][end],
      division(0, 2*pi, n_frames, true))

cross_beams(p) =
  let (dfi, ri, ro, h0, h1) = (2*pi/n_frames,
                                mid_block_r() + 0.25,
                                mid_block_r() + block_l + -0.25,
                                block_h,
                                2*block_h - slab_thickness)
    let is_trim = true
      map(fi -> [free_column(p + vcyl(ri, fi, h0), p + vcyl(ri, fi + dfi, h1)),
                 free_column(p + vcyl(ri, fi + 2*dfi, h0), p + vcyl(ri, fi + dfi, h1)),
                 free_column(p + vcyl(ro, fi, h0), p + vcyl(ro, fi + dfi, h1)),
                 free_column(p + vcyl(ro, fi + 2*dfi, h0), p + vcyl(ro, fi + dfi, h1))][end],
          division(0, 2*pi, n_frames/2, true))
    end
  end

out_columns(p) =
  let dz = block_h/(n_frames/2)
    map((z, ri, fi0) -> [column_rotation(p, fi0, rfunc(ri) + column_r, z - slab_thickness, slab_thickness + block_h + dz),
                        column_rotation(p, fi0, rfunc(ri) + block_l + -column_r, block_h, -(block_h + -z + slab_thickness))][end],
        division(0, block_h, n_frames/2, false),
        division(pi/(n_frames/2), pi + pi/(n_frames/2), n_frames/2, false),
        division(4*pi, 3*pi, n_frames/2, false))
    map((z, ri, fi0) -> [column_rotation(p, fi0, rfunc(ri) + column_r, z - slab_thickness, slab_thickness + block_h + dz),
                        column_rotation(p, fi0, rfunc(ri) + block_l + -column_r, 2*block_h, z + dz + -block_h)][end],
        division(block_h, 2*block_h, n_frames/2, false),
        division(pi + pi/(n_frames/2), 2*pi + pi/(n_frames/2), n_frames/2, false),
        division(3*pi, 2*pi, n_frames/2, false))
    map((z, ri, fi0) -> [column_rotation(p, fi0, rfunc(ri) + block_l + -column_r, z - dz - slab_thickness, slab_thickness + block_h + dz),
                        column_rotation(p, fi0, rfunc(ri) + column_r, 2*block_h, z - block_h)][end],
        division(2*block_h, block_h, n_frames/2, false),
        division(2*pi + pi/(n_frames/2), 3*pi + pi/(n_frames/2), n_frames/2, false),
        division(2*pi, pi, n_frames/2, false))
    map((z, ri, fi0) -> [column_rotation(p, fi0, rfunc(ri) + block_l + -column_r, z - dz - slab_thickness, slab_thickness + block_h + dz),
                        column_rotation(p, fi0, rfunc(ri) + column_r, block_h, -(block_h + -z + slab_thickness + dz))][end],
        division(block_h, 0, n_frames/2, false),
        division(3*pi + pi/(n_frames/2), 4*pi + pi/(n_frames/2), n_frames/2, false),
        division(pi, 0, n_frames/2, false))
  end

# LIBRARY WALLS ________________________________________________________________

book_wall(p, fi, z) =
  let (p1, p2, p3, p4, pa, pb) = (
    p + vpol(mid_block_r() + library_door, fi),
    p + vpol(mid_block_r() + block_l/2 + -(library_door/2), fi),
    p + vpol(mid_block_r() + block_l/2 + library_door/2, fi),
    p + vpol(mid_block_r() + block_l + -library_door, fi),
    u0() + vx(-(wall_thickness/2)),
    u0() + vxy(wall_thickness/2, block_h/4))
    with(default_level_to_level_height, block_h/4) do
      with(default_level, level(z)) do
        wall([p1, p2])
        wall([p3, p4])
      end
    end
  end

book_walls(p) =
  map(z -> map(fi0 -> book_wall(p, fi0, z), division(0, 2*pi, n_frames, false)),
      division(block_h, 2*block_h, mid_block_levels, false))

# normals
normal_quadrangulo(p0, p1, p2, p3) =
    normal_poligono([p1 - p0, p2 - p1, p3 - p2, p0 - p3])

normal_poligono(vs) =
    unitized(produtos_cruzados([vs..., vs[1]]))

produtos_cruzados(vs) =
    if length(vs) == 1
        vxyz(0, 0, 0)
    else
        cross(vs[1], vs[2]) + produtos_cruzados(vs[2:end])
    end

# torches for Game of Thrones theme presentation

torch_set(p, fi, z) =
	let (p1, p2, p3, p4) = (
          p + vz(man_h) + vpol(mid_block_r() + library_door, fi),
          p + vz(man_h) + vpol(mid_block_r() + block_l/2 + -(library_door/2), fi),
          p + vz(man_h) + vpol(mid_block_r() + block_l/2 + library_door/2, fi),
          p + vz(man_h) + vpol(mid_block_r() + block_l + -library_door, fi))
		(v2, v3) = (
			normal_quadrangulo(p, p2, p2+vz(1), p+vz(1)),
		 	normal_quadrangulo(p, p3, p3+vz(1), p+vz(1)))
		rotate(
		block_instance(astana_torch, loc_from_o_phi(add_z(p2, z), fi), 1)
			   ,-pi/8, p2+vz(z), v2)
		rotate(
		block_instance(astana_torch, loc_from_o_phi(add_z(p3, z), fi), 1)
			   ,pi/8, p3+vz(z), v3)
  end

torches(p) =
	let z = block_h/4+block_h
    #map(z ->
        map(fi0 -> torch_set(p, fi0, z), division(0, 2*pi, n_frames, false))#,
        #division(block_h, 2*block_h, mid_block_levels, false))
	end


# CORES ________________________________________________________________________

add1door(wall, loc, flip_x, flip_y) =
  add_door(wall, loc, default_door_family())

lift_frame(p, length, height, out_thickness, in_thickness) =
  let d = out_thickness/2
    with(default_level_to_level_height, block_h/4) do
      wall([add_xy(p, 0, d), add_xy(p, length, d)])
    end
    let p = add_x(p, length/5)
        length = 3/5*length
        door_l = (length - 2*d)/2
        space = 0.003
        height = 2.3
      with(current_material,
        current_backend() == unity ?
          get_material("Default/Materials/Aluminum") :
          current_material()) do
        box(add_y(p, -d), d, d, height-d)
        box(add_xy(p, length-d, -d), d, d, height-d)
        box(add_yz(p, -d, height-d), length, d, d)
        box(add_xy(p, d, -d/2), door_l-space, in_thickness, height-d)
        box(add_xy(p, d + door_l+space, -d/2), door_l-space, in_thickness, height-d)
      end
    end
  end

#=
astana_lift = current_backend() in [autocad, unity] ?
      block("AstanaLift",
        translating_current_cs(0, 0) do
          collecting_shapes() do
            lift_frame(x(0), 3, 3, 0.2, 0.1)
          end
        end) :
        "Not used"
=#

cores(p) =
  begin
    ro_in = mid_block_r() + library_door
    ro_out = mid_block_r() + block_l + -library_door
    dfi = 2*atan(big_core_w/2/ro_in)
    square_w = (block_l - library_door - library_door)/4
    core_elevator(ploc, level, alfa, pwidth1, pwidth2, pdepth1, pdepth2) =
      if current_backend() == archicad
        object("Elevator 18", ploc, level=level, angle=alfa,
            properties=["shaft_inner_width", distance(pwidth1, pwidth2) - 1.5*wall_thickness,
                        "shaft_inner_depth", distance(pdepth1, pdepth2) - 1.5*wall_thickness,
                        "car_inner_width", distance(pwidth1, pwidth2) - 3.5*wall_thickness,
                        "car_inner_depth", distance(pdepth1, pdepth2) - 3*wall_thickness,
                        "bShowElevatorShaft", 1,
                        "bTopSlab", 0,
                        "bPitSlab", 0,
                        "bPenthouse", 0,
                        "bPith", 0,
                        "bShowElevatoeWall", 0])
      else
        lift_frame(add_x(loc_from_o_phi(add_z(ploc, level.height), alfa), wall_thickness/2),
                   distance(pwidth1, pwidth2) - 0*wall_thickness,
                   default_level_to_level_height(),
                   1.0*wall_thickness,
                   0.1*wall_thickness)
        #block_instance(astana_lift, loc_from_o_phi(add_z(ploc, level.height), alfa), 1)
      end

  big_core(p, fi, z) =
      let (p1_left, p1_right, p2_left, p2_right, p3_left, p3_right, p4_left, p4_right, p5_left, p5_right) = (p + vpol(ro_out, fi + dfi), p + vpol(ro_out, fi - dfi), p + vpol(ro_out - square_w, fi + dfi), p + vpol(ro_out - square_w, fi - dfi), p + vpol(ro_out - 2*square_w, fi + dfi), p + vpol(ro_out - 2*square_w, fi - dfi), p + vpol(ro_in + square_w, fi + dfi), p + vpol(ro_in + square_w, fi - dfi), p + vpol(ro_in, fi + dfi), p + vpol(ro_in, fi - dfi))
        let (p1, p2, p3, p4, p5) = (points_in_between(p1_left, p1_right, 1, 1)[1], points_in_between(p2_left, p2_right, 1, 1)[1], points_in_between(p3_left, p3_right, 1, 1)[1], points_in_between(p4_left, p4_right, 1, 1)[1], points_in_between(p5_left, p5_right, 1, 1)[1])
          with(default_level_to_level_height, block_h/4) do
            with(default_level, level(z)) do
              wally = wall([p1_left, p1_right])
#              add1door(wally, x(distance(p1_left, p1) + door_width/2 + wall_thickness), false, false)
              add1door(wally, x(distance(p1_left, p1) + wall_thickness), false, false)
              wall([p2_left, p2])
              wall([p3_left, p3_right])
              wall([p4_left, p4_right])
              wally = wall([p5_left, p5_right])
#              add1door(wally, x(distance(p5_left, p5) + door_width/2 + wall_thickness), true, true)
#              add1door(wally, x(distance(p5_left, p5) - door_width/2 - wall_thickness), true, false)
              add1door(wally, x(distance(p5_left, p5) + wall_thickness), true, true)
              add1door(wally, x(distance(p5_left, p5) - door_width - wall_thickness), true, false)
              wall([p3_left, p5_left])
              wall([p1_right, p5_right])
              wall([p1, p5])
            end
          end
          core_elevator(p1_left, level(z), fi - pi, p1_left, p2_left, p1_left, p1)
          core_elevator(p2_left, level(z), fi - pi, p2_left, p3_left, p2_left, p2)
          if z == 2*block_h
            with(default_level, level(z + block_h/4)) do
              slab([p1_left, p1_right, p5_right, p5_left])
            end
          else
            []
          end
        end
      end

    small_core(p, fi, z) =
      begin
        book_wall_w = (block_l - 3*library_door)/2
        dsi = atan((2*square_w)/(ro_out - book_wall_w))
        let (p1, p1_left, p2, p2_left, p2_left_middle, p3, p3_left, p3_left_middle) = (p + vpol(ro_out, fi), p + vpol(ro_out, fi + dsi), p + vpol(ro_out - book_wall_w/2, fi), p + vpol(ro_out - book_wall_w/2, fi + dsi), p + vpol(ro_out - book_wall_w/2, fi + dsi/2), p + vpol(ro_out - book_wall_w, fi), p + vpol(ro_out - book_wall_w, fi + dsi), p + vpol(ro_out - book_wall_w, fi + dsi/2))
          with(default_level_to_level_height, block_h/4) do
            with(default_level, level(z)) do
              wall([p1, p1_left])
              wall([p2, p2_left])
              wally = wall([p1_left, p3_left])
              add1door(wally, x(distance(p1_left, p2_left) - door_width/2 - wall_thickness), true, true)
              wall([p1, p3])
              wall([p2_left_middle, p3_left_middle])
            end
          end
          core_elevator(p3_left, level(z), fi - pi/2 - -dfi, p3_left, p3_left_middle, p3_left_middle, p2_left_middle)
          core_elevator(p3_left_middle, level(z), fi - pi/2, p3_left_middle, p3, p3_left_middle, p2_left_middle)
          if z == 3*block_h
            with(default_level, level(z + block_h/4)) do
              slab([p1, p3, p3_left, p1_left])
            end
          else
            []
          end
        end
      end

    underground_core(p, fi, z) =
      let (p1_left, p1_right, p2_left, p2_right, p3_left, p3_right, p4_left, p4_right) = (p + vpol(ro_out, fi + dfi), p + vpol(ro_out, fi - dfi), p + vpol(ro_out - square_w, fi + dfi), p + vpol(ro_out - square_w, fi - dfi), p + vpol(ro_out - 2*square_w, fi + dfi), p + vpol(ro_out - 2*square_w, fi - dfi), p + vpol(ro_in + square_w, fi + dfi), p + vpol(ro_in + square_w, fi - dfi))
        let (p1, p2, p3, p4) = (points_in_between(p1_left, p1_right, 1, 1)[1], points_in_between(p2_left, p2_right, 1, 1)[1], points_in_between(p3_left, p3_right, 1, 1)[1], points_in_between(p4_left, p4_right, 1, 1)[1])
          with(default_level_to_level_height, block_h/4) do
            with(default_level, level(z)) do
              wally = wall([p1_left, p1_right])
              add1door(wally, x(distance(p1_left, p1) + door_width/2 + wall_thickness), false, false)
              wall([p2_left, p2])
              wall([p3_left, p3_right])
              wall([p4_left, p4_right])
              wall([p3_left, p4_left])
              wall([p1_right, p4_right])
              wall([p1, p4])
            end
          end
          core_elevator(p1_left, level(z), fi - pi, p1_left, p2_left, p1_left, p1)
          core_elevator(p2_left, level(z), fi - pi, p2_left, p3_left, p2_left, p2)
        end
      end

    map(z -> small_core(p, 0, z),
        division(0, 3*block_h, 3*mid_block_levels))
    map(fi -> map(z -> big_core(p, fi, z), division(block_h, 2*block_h, mid_block_levels)),
        division(10*pi/6, 2*pi/6, n_big_cores - 1))
    map(fi -> map(z -> underground_core(p, fi, z), division(0, block_h, mid_block_levels)),
        division(10*pi/6, 2*pi/6, n_big_cores - 1))
  end

# ALL AROUND SPIRAL STAIRS _____________________________________________________

steplist(p, fi0, r0, r1) =
  let dfi = 2*pi/(6*n_frames)
    [map((fi, ri) -> p + vpol(ri, fi), division(fi0, fi0 + dfi, 4), division(r1, r0, 4))...,
     map((fi, ri) -> p + vpol(ri + spiral_stairs_width, fi), division(fi0 + dfi, fi0, 4), division(r0, r1, 4))...]
  end

six_step(p, fi0, fir, z_slb, _case) =
  let (dfi, z_stp, r0, r1, r2, r3, r4, r5, r6, rfora) = (2*pi/(6*n_frames), block_h/(n_frames*3), rfunc(fir), rfunc(fir + 2*pi/(6*n_frames)), rfunc(fir + 2*2*pi/(6*n_frames)), rfunc(fir + 3*2*pi/(6*n_frames)), rfunc(fir + 4*2*pi/(6*n_frames)), rfunc(fir + 5*2*pi/(6*n_frames)), rfunc(fir + 6*2*pi/(6*n_frames)), block_l - spiral_stairs_width)
    step_slab(pts, z, tk) =
      with(default_slab_family, slab_family_element(default_slab_family(), thickness=tk)) do
        with(default_level, level(z)) do
          slab(pts)
        end
      end

    if _case == 1
      step_slab(steplist(p, fi0 - 2*dfi, r1, r2), z_slb + 1*z_stp, 1*z_stp)
      step_slab(steplist(p, fi0 - 3*dfi, r2, r3), z_slb + 2*z_stp, 2*z_stp)
      step_slab(steplist(p, fi0 - 4*dfi, r3, r4), z_slb + 3*z_stp, 3*z_stp)
      step_slab(steplist(p, fi0 - 5*dfi, r4, r5), z_slb + 4*z_stp, 4*z_stp)
      step_slab(steplist(p, fi0 - 6*dfi, r5, r6), z_slb + 5*z_stp, 5*z_stp)
    else
      step_slab(steplist(p, fi0 - 1*dfi, rfora + r0, rfora + r1), z_slb + 5*z_stp, 5*z_stp)
      step_slab(steplist(p, fi0 - 2*dfi, rfora + r1, rfora + r2), z_slb + 4*z_stp, 4*z_stp)
      step_slab(steplist(p, fi0 - 3*dfi, rfora + r2, rfora + r3), z_slb + 3*z_stp, 3*z_stp)
      step_slab(steplist(p, fi0 - 4*dfi, rfora + r3, rfora + r4), z_slb + 2*z_stp, 2*z_stp)
      step_slab(steplist(p, fi0 - 5*dfi, rfora + r4, rfora + r5), z_slb + 1*z_stp, 1*z_stp)
    end
  end
spiral_steps(p) =
  let (dz_slab, dr_slab) = (block_h/(n_frames/2), 2*pi/n_frames)
    map((dz, fi0, fir) -> [six_step(p, fi0, fir, dz, 1), six_step(p, fi0, fir, dz + block_h, 1)][end],
        division(0, 2*block_h, n_frames, false),
        division(4*pi + dr_slab, 2*pi + dr_slab, n_frames, false),
        division(0, 2*pi, n_frames, false))
    map((dz, fi0, fir) -> [six_step(p, fi0, fir, dz, 2), six_step(p, fi0, fir, dz + block_h, 2)][end],
        division(2*block_h - dz_slab, -dz_slab, n_frames, false),
        division(2*pi, 0, n_frames, false),
        division(dr_slab + 2*pi, dr_slab + 4*pi, n_frames, false))
  end
