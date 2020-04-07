include("2_facade.jl")

# INDEX ________________________________________________________________________

# IN-BLOCKS STAIRCASES
# RAILINGS
# GLASS WALL

# BOOK SHELVES
# TABLES
# LIGHTS
# PEOPLE

# IN-BLOCKS STAIRCASES _________________________________________________________

stp_rise = 0.18
stp_run = 0.29
stair_width = 1.5
stair_h = 3*block_h/(n_frames/2)
zmid = block_h/mid_block_levels
n_steps = floor(stair_h/stp_rise)
dfu3 = 2*pi/(n_frames/3)

stair_cases(p) =
  begin
    dz = block_h/(n_frames/2)
    dfi = pi/(n_frames/2)
    stair_BIM(p, fi0, z, rmin, rmax, alfa) =
      stairs("Stair Spiral 18",
             p,
             bottom_offset=z,
             angle=fi0,
             height=n_steps*stp_rise,
             use_xy_fix_size=true,
             x_ratio=2*rmax,
             properties=["nRisers", n_steps,
                         "angle", alfa,
                         "swelldia", 2*rmin,
                         "treadThickness", 0,
                         "nosing", 0,
                         "stairSlabThk", 0.2,
                         "ust", 0.2,
                         "stairBaseType_m", 1,
                         "rightRailType_m", 0,
                         "leftRailType_m", 0,
                         "bmat", "Paint - Light Gray"])

    slab_set_BIM_3(p, fi3, z3, rmin, rmax) =
      begin
        alfa_stair = 2*atan((stp_run*n_steps)/(2*rmin))
        up(r1, r2, r3, r4) =
          begin
            stair_BIM(p, fi3 + alfa_stair, z3 + zmid + -stp_rise + 0.02, r1, r2, -alfa_stair)
            stair_BIM(p, fi3 + -2*dfi + alfa_stair, z3 + 3*zmid + -stp_rise + 0.02, r3, r4, -alfa_stair)
          end

        down(r1, r2, r3, r4) =
          begin
            stair_BIM(p, fi3 - alfa_stair, z3 + 3*zmid + -3*dz + -stp_rise + 0.02, r1, r2, alfa_stair)
            stair_BIM(p, fi3 - 2*dfi - alfa_stair, z3 + zmid + -3*dz + -stp_rise + 0.02, r3, r4, alfa_stair)
          end

        if fi3 == 4*pi
          up(rmax, rmax + stair_width, rmin, rmax)
        elseif fi3 > dfu3 + 2*pi
          up(rmin, rmax, rmin, rmax)
        elseif fi3 == dfu3 + 2*pi
          up(rmin, rmax, rmax, rmax + stair_width)
        elseif fi3 == 2*pi
          down(rmin - stair_width, rmin, rmin, rmax)
        elseif fi3 > dfi + dfu3
          down(rmin, rmax, rmin, rmax)
        else
          down(rmin, rmax, rmin - stair_width, rmin)
        end
      end

    stair_CAD(p, fi0, z, n, rmin, rmax, up_div_down) =
      begin
        alfa_step = 2*atan(stp_run/(2*rmin))
        let (p1, p2, p3, p4, p5, p6) = (p + vcyl(rmin, fi0, z), p + vcyl(rmax, fi0, z), p + vcyl(rmax, alfa_step + fi0, z), p + vcyl(rmin, alfa_step + fi0, z), p + vcyl(rmax, fi0 - alfa_step, z), p + vcyl(rmin, fi0 - alfa_step, z))
          if up_div_down == 0
            if n == 0
              true
            else
              irregular_prism([p1, p2, p3, p4], 2*stp_rise)
              stair_CAD(p, fi0 + alfa_step, z - stp_rise, n - 1, rmin, rmax, 0)
            end
          else
            if n == 0
              true
            else
              irregular_prism([p1, p2, p5, p6], 2*stp_rise)
              stair_CAD(p, fi0 - alfa_step, z - stp_rise, n - 1, rmin, rmax, 1)
            end
          end
        end
      end

    slab_set_CAD_3(p, fi3, z3, rmin, rmax) =
      begin
        up(r1, r2, r3, r4) =
          begin
            stair_CAD(p, fi3, z3 + stair_h + zmid + -4.5*stp_rise, n_steps, r1, r2, 0)
            stair_CAD(p, fi3 - dfi - dfi, z3 + stair_h + 3*zmid + -4.5*stp_rise, n_steps, r3, r4, 0)
          end

        down(r1, r2, r3, r4) =
          begin
            stair_CAD(p, fi3, z3 + 3*zmid + -4.5*stp_rise, n_steps, r1, r2, 1)
            stair_CAD(p, fi3 - dfi - dfi, z3 + zmid + -4.5*stp_rise, n_steps, r3, r4, 1)
          end

        if fi3 == 4*pi
          up(rmax, rmax + stair_width, rmin, rmax)
        elseif fi3 > dfu3 + 2*pi
          up(rmin, rmax, rmin, rmax)
        elseif fi3 == dfu3 + 2*pi
          up(rmin, rmax, rmax, rmax + stair_width)
        elseif fi3 == 2*pi
          down(rmin - stair_width, rmin, rmin, rmax)
        elseif fi3 > dfi + dfu3
          down(rmin, rmax, rmin, rmax)
        else
          down(rmin, rmax, rmin - stair_width, rmin)
        end
      end

    let (rmin_in, rmax_in, rmin_out, rmax_out, slab_set_3) =
      (mid_block_r() - stair_width, mid_block_r(), mid_block_r() + block_l, mid_block_r() + block_l + stair_width, current_backend() == archicad ? slab_set_BIM_3 : slab_set_CAD_3)
      map((fi3, z3) -> slab_set_3(p, fi3, z3 + slab_thickness + -stp_rise, rmin_in, rmax_in),
          division(4*pi, 2*pi, n_frames/3, false),
          division(0, 2*block_h, n_frames/3, false))
      map((fi3, z3) -> slab_set_3(p, fi3, z3 + slab_thickness + -stp_rise, rmin_out, rmax_out),
          division(2*pi, 0, n_frames/3, false),
          division(2*block_h, 0, n_frames/3, false))
    end
  end

# RAILINGS _____________________________________________________________________

vertical_panel(p1, p2, h1, h2) =
  if p1.x == p2.x || p1.y == p2.y
    true
  else
    let (p3, p4) = (p1 + vz(h1), p2 + vz(h2))
      panel([p1, p2, p4, p3])
    end
  end

rect_rail(p, fi0, ri, z) =
  let (p1i, p2i, p3i, p1o, p2o, p4o) = (p + vcyl(ri + spiral_stairs_width, fi0, z),
                                        p + vcyl(ri + block_l, fi0, z),
                                        p + vcyl(mid_block_r(), fi0, z),
                                        p + vcyl(ri, fi0, z),
                                        p + vcyl(ri + block_l + -spiral_stairs_width, fi0, z),
                                        p + vcyl(mid_block_r() + block_l, fi0, z))
    if fi0 == 4*pi && z == block_h
      false
    elseif block_h < z < 2*block_h && p3i.y == p1i.y
      false
    elseif block_h < z < 2*block_h && p4o.y == p2o.y
      false
    elseif block_h < z < 2*block_h && fi0 > 2*pi
      vertical_panel(p3i, p1i, railing_height, railing_height)
    elseif block_h < z < 2*block_h && fi0 > 0
      vertical_panel(p4o, p2o, railing_height, railing_height)
    elseif fi0 == 2*pi + 2*pi/n_frames + 2*pi/n_frames && z <= 2*block_h
      false
    elseif fi0 == 2*pi + 2*pi/n_frames && z <= 2*block_h
      false
    elseif fi0 == 2*pi && z <= 2*block_h
      false
    elseif fi0 > 2*pi
      vertical_panel(p2i, p1i, railing_height, railing_height)
    else
      vertical_panel(p1o, p2o, railing_height, railing_height)
    end
  end

vertical_panels(pts, h1, h2) =
  map((p1, p2, h) -> vertical_panel(p1, p2, h, h + (h2 - h1)/length(pts)),
      pts[1:end-1],
      pts[2:end],
      division(h1, h2, length(pts))[1:end-2])

curv_rail(p, fi0, r1, r2, z0, z1, z2) =
  begin
    dfi = 2*pi/n_frames
    vertical_panels(map((fi, ri) -> p + vcyl(ri, fi, z0),
                        division(fi0, fi0 + dfi, n_pts_for_curv),
                        division(r1, r2, n_pts_for_curv)),
                    z1 + railing_height, z2 + railing_height)
  end

out_block_out_rails(p) =
  begin
    dzslab = block_h/(n_frames/2)
    dfi = 2*pi/n_frames
    rl(dz1, dz2, ra1, ra2, rb1, rb2, fi1, fi2, _add_rfu, z0, z1, z2) =
      map((dz, ri1, ri2, fi0) ->
             if fi0 == 4*pi
                 curv_rail(p, fi0, rfunc(ri2) + _add_rfu, rfunc(ri1) + _add_rfu, dz + z0, z1, z1)
             elseif fi0 == 2*pi
                 curv_rail(p, fi0, rfunc(ri2) + _add_rfu, rfunc(ri1) + _add_rfu, dz + z0, z1, z1)
             elseif fi0 == pi && z0 == 0 && _add_rfu == 0
                 false
             else
                 curv_rail(p, fi0, rfunc(ri2) + _add_rfu, rfunc(ri1) + _add_rfu, dz + z0, z1, z2)
             end,
          division(dz1, dz2, n_frames/2, false),
          division(ra1, ra2, n_frames/2, false),
          division(rb1, rb2, n_frames/2, false),
          division(fi1, fi2, n_frames/2, false))

    rl(0, block_h, 0, pi, dfi, pi + dfi, 4*pi, 3*pi, 0, 0, dzslab, 0)
    rl(0, block_h, 0, pi, dfi, pi + dfi, 4*pi, 3*pi, block_l, 0, dzslab, 0)
    rl(0, block_h, 0, pi, dfi, pi + dfi, 4*pi, 3*pi, 0, block_h, dzslab, 0)
    rl(block_h, 2*block_h, pi, 2*pi, pi + dfi, 2*pi + dfi, 3*pi, 2*pi, 0, 0, dzslab, 0)
    rl(block_h, 2*block_h, pi, 2*pi, pi + dfi, 2*pi + dfi, 3*pi, 2*pi, 0, block_h, dzslab, 0)
    rl(block_h, 2*block_h, pi, 2*pi, pi + dfi, 2*pi + dfi, 3*pi, 2*pi, block_l, block_h, dzslab, 0)
    rl(2*block_h, block_h, 2*pi, 3*pi, 2*pi + dfi, 3*pi + dfi, 2*pi, pi, block_l, 0, 0, dzslab)
    rl(2*block_h, block_h, 2*pi, 3*pi, 2*pi + dfi, 3*pi + dfi, 2*pi, pi, 0, block_h, 0, dzslab)
    rl(2*block_h, block_h, 2*pi, 3*pi, 2*pi + dfi, 3*pi + dfi, 2*pi, pi, block_l, block_h, 0, dzslab)
    rl(block_h, 0, 3*pi, 4*pi, 3*pi + dfi, 4*pi + dfi, pi, 0, 0, 0, 0, dzslab)
    rl(block_h, 0, 3*pi, 4*pi, 3*pi + dfi, 4*pi + dfi, pi, 0, block_l, 0, 0, dzslab)
    rl(block_h, 0, 3*pi, 4*pi, 3*pi + dfi, 4*pi + dfi, pi, 0, block_l, block_h, 0, dzslab)
  end

rail3(p, fi3, z3, fir, St_rmin, St_rmax) =
  begin
    dz = block_h/(n_frames/2)
    rail2(p, fi0, z, r, St_rmin, St_rmax) =
      begin
        rail_u(ra, rb) =
          let (p1, p2, p3, p4) = (p + vcyl(ra, fi0, z + 3*dz), p + vcyl(St_rmin, fi0, z + 3*dz), p + vcyl(St_rmax, fi0, z + 3*dz), p + vcyl(rb, fi0, z + 3*dz))
            vertical_panel(p1, p2, railing_height, railing_height)
            vertical_panel(p3, p4, railing_height, railing_height)
          end

        rail_d(ra, rb) =
          let (p1, p4) = (p + vcyl(ra, fi0, z), p + vcyl(rb, fi0, z))
            vertical_panel(p1, p4, railing_height, railing_height)
          end

        if fi0 >= 2*pi && block_h - 3 < z < 3 + block_h*2
          rail_d(r, mid_block_r())
        elseif fi0 < 2*pi && block_h - 3 < z < 3 + block_h*2
          rail_d(mid_block_r() + block_l, r + block_l)
        else
          rail_d(r, r + block_l)
        end
        if fi0 >= 2*pi && block_h - 3 < z + 3*dz < 3 + block_h*2
          rail_u(r, mid_block_r())
        elseif fi0 < 2*pi && block_h - 3 < z + 3*dz < 3 + block_h*2
          rail_u(mid_block_r() + block_l, r + block_l)
        else
          rail_u(r, r + block_l)
        end
      end

    let (dfi, r, r1, r2, r3) = (pi/(n_frames/2), rfunc(fir), rfunc(fir + pi/(n_frames/2)), rfunc(fir + 2*pi/(n_frames/2)), rfunc(fir + 3*pi/(n_frames/2)))
      up(St_rmin, St_rmax, St_rmin2, St_rmax2) =
        begin
          rail2(p, fi3, z3 + zmid, r1, St_rmin, St_rmax)
          rail2(p, fi3 - 2*dfi, z3 + 3*zmid, r3, St_rmin2, St_rmax2)
        end

      down(St_rmin, St_rmax, St_rmin2, St_rmax2) =
        begin
          rail2(p, fi3, z3 + 3*zmid + -3*dz, r1, St_rmin, St_rmax)
          rail2(p, fi3 - 2*dfi, z3 + zmid + -3*dz, r3, St_rmin2, St_rmax2)
        end

      if fi3 == 4*pi
        up(St_rmax, St_rmax + stair_width, St_rmin, St_rmax)
      elseif fi3 > dfu3 + 2*pi
        up(St_rmin, St_rmax, St_rmin, St_rmax)
      elseif fi3 == dfu3 + 2*pi
        up(St_rmin, St_rmax, St_rmax, St_rmax + stair_width)
      elseif fi3 == 2*pi
        down(St_rmin - stair_width, St_rmin, St_rmin, St_rmax)
      elseif fi3 > dfi + dfu3
        down(St_rmin, St_rmax, St_rmin, St_rmax)
      else
        down(St_rmin, St_rmax, St_rmin - stair_width, St_rmin)
      end
    end
  end

in_block_rect_rails(p) =
  let (St_rmin_in, St_rmax_in, St_rmin_out, St_rmax_out) = (mid_block_r() - stair_width, mid_block_r(),
                                                            mid_block_r() + block_l,
                                                            mid_block_r() + block_l + stair_width)
    map((fi3, z3, fir) -> rail3(p, fi3, z3, fir, St_rmin_in, St_rmax_in),
        division(4*pi, 2*pi, n_frames/3, false),
        division(0, 2*block_h, n_frames/3, false),
        division(0, 2*pi, n_frames/3, false))
    map((fi3, z3, fir) -> rail3(p, fi3, z3, fir, St_rmin_out, St_rmax_out),
        division(2*pi, 0, n_frames/3, false),
        division(2*block_h, 0, n_frames/3, false),
        division(2*pi, 4*pi, n_frames/3, false))
  end

in_block_curv_rails(p, fi0, fir, z0) =
  begin
    curv_rail_in(p, fi, r1, r2, z) =
      if fi0 >= 2*pi && block_h - 3 <= z <= block_h
        curv_rail(p, fi, r1, r2, z, 0, 0)
        curv_rail(p, fi, mid_block_r(), mid_block_r(), z, 0, 0)
      elseif fi0 >= 2*pi && block_h <= z <= block_h*2
        curv_rail(p, fi, r1, r2, z, 0, 0)
      elseif fi0 >= 2*pi && block_h*2 <= z <= 3 + block_h*2
        curv_rail(p, fi, r1, r2, z, 0, 0)
        curv_rail(p, fi, mid_block_r(), mid_block_r(), z, 0, 0)
      elseif fi0 < 2*pi && block_h*2 <= z <= 3 + block_h*2
        curv_rail(p, fi, mid_block_r() + block_l, mid_block_r() + block_l, z, 0, 0)
        curv_rail(p, fi, r1 + block_l, r2 + block_l, z, 0, 0)
      elseif fi0 < 2*pi && block_h <= z <= block_h*2
        curv_rail(p, fi, r1 + block_l, r2 + block_l, z, 0, 0)
      elseif fi0 < 2*pi && block_h - 3 <= z <= block_h
        curv_rail(p, fi, mid_block_r() + block_l, mid_block_r() + block_l, z, 0, 0)
        curv_rail(p, fi, r1 + block_l, r2 + block_l, z, 0, 0)
      else
        curv_rail(p, fi, r1, r2, z, 0, 0)
        curv_rail(p, fi, r1 + block_l, r2 + block_l, z, 0, 0)
      end

    let (zmid, dz, dfi, r, r1, r2, r3) = (block_h/mid_block_levels,
                                          block_h/(n_frames/2),
                                          pi/(n_frames/2),
                                          rfunc(fir), rfunc(fir + pi/(n_frames/2)),
                                          rfunc(fir + 2*pi/(n_frames/2)),
                                          rfunc(fir + 3*pi/(n_frames/2)))
      if fi0 > 37*pi/18
        curv_rail_in(p, fi0, r1, r, z0 + zmid)
        curv_rail_in(p, fi0, r1, r, z0 + 3*zmid)
        curv_rail_in(p, fi0 - dfi, r2, r1, z0 + dz + dz + dz + zmid)
        curv_rail_in(p, fi0 - dfi, r2, r1, z0 + 3*zmid)
        curv_rail_in(p, fi0 - dfi - dfi, r3, r2, z0 + dz + dz + dz + zmid)
        curv_rail_in(p, fi0 - dfi - dfi, r3, r2, z0 + 3*zmid)
        in_block_curv_rails(p, fi0 - 3*dfi, fir + 3*dfi, z0 + 3*dz)
      elseif fi0 > dfi + 0
        curv_rail_in(p, fi0, r1, r, z0 + zmid)
        curv_rail_in(p, fi0, r1, r, z0 + 3*zmid)
        curv_rail_in(p, fi0 - dfi, r2, r1, z0 + zmid)
        curv_rail_in(p, fi0 - dfi, r2, r1, z0 + 3*zmid - 3*dz)
        curv_rail_in(p, fi0 - dfi - dfi, r3, r2, z0 + zmid)
        curv_rail_in(p, fi0 - dfi - dfi, r3, r2, z0 + 3*zmid - 3*dz)
        in_block_curv_rails(p, fi0 - 3*dfi, fir + 3*dfi, z0 - 3*dz)
      else
        true
      end
    end
  end

raillings(p) =
  begin
    dz_slb = (2*block_h)/n_frames
    map((dz, ri, fi0) -> [rect_rail(p, fi0, rfunc(ri), dz), rect_rail(p, fi0, rfunc(ri), dz + block_h)][end],
        division(dz_slb, 2*block_h + dz_slb, n_frames, false),
        division(2*pi/n_frames, 2*pi + 2*pi/n_frames, n_frames, false),
        division(4*pi, 2*pi, n_frames, false))
    map((dz, ri, fi0) -> [rect_rail(p, fi0, rfunc(ri), dz), rect_rail(p, fi0, rfunc(ri), dz + block_h)][end],
        division(2*block_h, 0, n_frames, false),
        division(2*pi + 2*pi/n_frames, 4*pi + 2*pi/n_frames, n_frames, false),
        division(2*pi, 0, n_frames, false))
    out_block_out_rails(p)
    in_block_rect_rails(p)
    in_block_curv_rails(p, 4*pi, 0, 0)
    map(fi -> fi == pi ? true : [curv_rail(p, fi, mid_block_r(), mid_block_r(), 2*block_h, 0, 0), curv_rail(p, fi, mid_block_r() + block_l, mid_block_r() + block_l, 2*block_h, 0, 0)][end],
        division(2*pi, 0, n_frames, false))
  end

equalf(x, y) = abs(x-y) < 1e-1

# GLASS WALLS __________________________________________________________________

n_curtain_panels = n_pts_for_curv/2
n_curtain_lines = 2
glass_dist = 0.1

glass_wall_BIM(p, fi, z, r, in_div_out) =
  let
    dfi = 2*pi/n_frames
    dz = (2*block_h - block_h)/4
    curtain_h = (block_h/4 - slab_thickness)
    if z == block_h && (fi == 3*pi || fi == pi)
      []
    elseif z == block_h + dz && (fi == 3.5*pi || fi == 3.5*pi - dfi || fi == 3.5*pi - 2*dfi || 0.5*pi <= fi <= 0.5*pi + 2*dfi || fi == 3*pi || fi == 3*pi + dfi || fi == 3*pi + 2*dfi || fi == pi || fi == pi - dfi || fi == pi - 2*dfi)
      []
    elseif z == block_h + 2*dz && (fi == 3.5*pi || fi == 2.5*pi || fi == 1.5*pi || fi == 0.5*pi)
      []
    elseif z == block_h + 3*dz && (fi == 2.5*pi || fi == 2.5*pi + dfi || fi == 2.5*pi + 2*dfi || fi == 1.5*pi || fi == 1.5*pi - dfi || fi == 1.5*pi - 2*dfi || fi == 3*pi || fi == 3*pi - dfi || fi == 3*pi - 2*dfi || fi == pi || fi == pi + dfi || fi == pi + 2*dfi)
      []
    else
      fam = curtain_wall_family_element(default_curtain_wall_family(), max_panel_dy=curtain_h/n_curtain_lines)
      with(default_level, level(z), default_level_to_level_height, curtain_h) do
        curtain_wall(
        # in_div_out == 1 ?
        #   map(fii -> p + vpol(r-0.1, fii), division(fi, fi + dfi, n_curtain_panels+1)) :
        #   map(fii -> p + vpol(r+0.1, fii), division(fi + dfi, fi, n_curtain_panels+1)),
        # family=fam)
          in_div_out == 1 ? arc_path(p, r-0.1, fi, dfi) : arc_path(p, r+0.1, fi + dfi, -dfi),
          family=fam)
      end
    end
  end

glass_walls(p) =
  begin
    map(fi -> map(z -> glass_wall_BIM(p, fi, z, mid_block_r() + glass_dist, 0),
                    division(block_h, 2*block_h, mid_block_levels, false)),
        division(4*pi, 2*pi, n_frames, false))
    map(fi -> map(z -> glass_wall_BIM(p, fi, z, mid_block_r() + block_l + -glass_dist, 1),
                    division(block_h, 2*block_h, mid_block_levels, false)),
        division(2*pi, 0, n_frames, false))
  end


# BOOK SHELVES _________________________________________________________________

books(ploc, level, height, fi) =
  let width = random_range(library_door, 1.9*library_door)
    "No books placed"
  end

n_shelves = 6
bs_height = block_h/4 - slab_thickness

book(corner_pivot, height, cover_length, spine_width, cover_material, paper_material) =
  let cover_thickness = 0.003
    with(current_material, cover_material) do
        box(corner_pivot,
            corner_pivot + vxyz(cover_thickness, -cover_length +cover_thickness, height +cover_thickness))
        box(corner_pivot + vy(-cover_length +cover_thickness),
            corner_pivot + vxyz(spine_width, -cover_length, height +cover_thickness))
        box(corner_pivot + vx(spine_width -cover_thickness),
            corner_pivot + vxyz(spine_width, -cover_length +cover_thickness, height +cover_thickness))
    end
    with(current_material, paper_material) do
        box(corner_pivot + vx(cover_thickness),
            corner_pivot + vxyz(spine_width -cover_thickness,
                                -cover_length +cover_thickness,
                                height))
    end
  end

tilted_book(corner_pivot, height, cover_length, spine_width, angle, axis)=
    let cover_thickness = 0.003
        rotate(
            union(
                #with cover material
                box(corner_pivot,
                    corner_pivot + vxyz(cover_thickness, -cover_length +cover_thickness, height +cover_thickness)),
                box(corner_pivot + vy(-cover_length +cover_thickness),
                    corner_pivot + vxyz(spine_width, -cover_length, height +cover_thickness)),
                box(corner_pivot + vx(spine_width -cover_thickness),
                    corner_pivot + vxyz(spine_width, -cover_length +cover_thickness, height +cover_thickness)),
                #with white material = plaster?
                box(corner_pivot + vx(cover_thickness),
                    corner_pivot + vxyz(spine_width -cover_thickness,
                                        -cover_length +cover_thickness,
                                        height))),
            angle,
            corner_pivot,
            axis)
    end

random_element(lst) = lst[random_range(1, length(lst))]

book_cover_materials = [
  #"BeachFootsteps",
  #"BeachSand",
  "BeachShells",
  #"BeigeFabric",
  "BlackLeather",
  "BlackWhiteMarble",
  #"BlackWood",
  #"BlueWrappingPlastic",
  "BrownFabric",
  "BrownLeather",
  #"BrownPinkLeather",
  "CalgaryMarble",
  "Cardboard",
  "ColorfulFabric",
  #"ConcreteTiles",
  "CraftPaper",
  "DirtyCraftPaper",
  "GrassMarguerites",
  #"GreenFabric",
  #"GreenGrass",
  #"GreenGrassRough",
  "GreenLeather",
  #"MetalGalvanized",
  "MulticolorPebbles",
  "OldBrownLeather",
  #"PatchworkTiles",
  #"Pebbles",
  "PineNeedles",
  "Plaster",
  #"RedBrick",
  #"RedFabric",
  "RedMarble",
  #"RedMirror",
  "RedMosaic",
  "RedStucco",
  "StoneWall",
  "StuccoPlaster",
  "TrianglesFabric",
  "WhiteBrick",
  "White_Polyrattan",
  "WhitePorcelain"]

book_patterns = "Prefabs/BookShelves/Dependencies/Patterns/"

bookset(corner_pivot, n_books) =
  if n_books==0
    nothing
  else
    let height = random_range(.2, .5)
        width = random_range(.02, .08)
        length = random_range(.1, .3)
        material = get_material(book_patterns * random_element(book_cover_materials))
      book(corner_pivot, height, length, width, material, get_material(book_patterns * "Pages"))
      bookset(corner_pivot + vx(width), n_books-1)
    end
  end

tilt_bookset(corner_pivot, n_books)=
    let height = random_range(.3, .15)
        width = random_range(.06, .02)
        length = random_range(.2, .1)
        rotation = random(10)
        angle = random(-pi/4)
        axis_index = random_range(1,3)
        book_move = [height*sin(-angle), length*sin(-angle)]
        if n_books==0
            nothing
        elseif rotation < 2
            tilted_book(corner_pivot + vx(width*.05 + book_move[axis_index]), height, length, width, angle, [vy(), vz()][axis_index])
            tilt_bookset(corner_pivot + vx(width + book_move[axis_index]), n_books-1)
        else
            book(corner_pivot, height, length, width)
            tilt_bookset(corner_pivot + vx(width), n_books-1)
        end
    end

#book_sets = [block("Prefabs/Books/BookSet$(i)") for i in 9:9]

bookshelf(p, len, width, height, out_thickness, in_thickness, nx, nz) =
    let inner_height = height - 2*out_thickness
        inner_length = len - 2*out_thickness
        inner_sep = inner_length/nx - in_thickness/2
        top = add_z(p, height - out_thickness)
        inner_p = add_xz(p, out_thickness, out_thickness)
        # outer frame
        box(add_y(p, width-in_thickness), len, in_thickness, height)
        box(p, len, width, out_thickness)
        box(top, len, width, out_thickness)
        box(add_z(p, out_thickness), out_thickness, width, inner_height)
        box(add_xz(p, len - out_thickness, out_thickness), out_thickness, width, inner_height)
        # inner frame
        for x in division(0, inner_length, nx)[1:end-1]
          for z in division(0, inner_height, nz)[1:end-1]
            for xx in division(0, inner_length/nx, 1, false)
              bookset(add_xyz(inner_p, x + xx + in_thickness/2, width-in_thickness, z), 8)
#              block_instance(
#                book_sets[random_range(1, length(book_sets)+1)],
#                add_xyz(inner_p, x + xx + in_thickness/2, width-in_thickness, z),
#                0)
            end
          end
        end
        for x in division(0, inner_length, nx)[2:end-1]
            box(add_x(inner_p, x - in_thickness/2), in_thickness, width, inner_height)
        end
        for z in division(0, inner_height, nz)[2:end-1]
            box(add_z(inner_p, z - in_thickness/2), inner_length, width, in_thickness)
        end
    end


astana_bookshelf =
  current_backend() in [autocad, unity] ?
    block("AstanaBookshelf",
      with(current_material,
           current_backend() == unity ?
             get_material("Materials/Wood/Wood1") :
             current_material()) do
        translating_current_cs(0, 0) do
          collecting_shapes() do
            bookshelf(u0(),
                      (block_l - 3*library_door)/2,
                      0.425,
                      bs_height,
                      0.05,
                      0.03,
                      8,
                      n_shelves)
        end
      end
    end) :
    "Not used"

book_set(pbook, fi, level, height, shv_no) =
  if shv_no <= 0
    "Lots of books"
  else
    books(pbook, level, height, fi)
    book_set(pbook, fi, level, height + (bs_height - 0.12)/n_shelves, shv_no - 1)
  end

book_shelf(pshv, pbook, level, fi) =
  if current_backend() == archicad
    object("Book Shelf 01 18",
           pshv,
           level=level,
           x_ratio=(block_l - 3*library_door)/2 - 0.65,
           angle=fi,
           properties=["B", 4,
                       "zzyzx", bs_height,
                       "vpn", 9,
                       "shn", n_shelves - 1,
                       "spmat", "Paint - Cobalt Blue",
                       "bpmat", "Paint - Pale Blue",
                       "vpmat", "Paint - Cobalt Blue",
                       "shmat", "Paint - Cobalt Blue",
                       "gs_cornice_type_m", 0,
                       "gs_leg_type_m", "Style 4",
                       "gs_leg_mat", "Paint - Cobalt Blue"])
    book_set(pbook, fi, level, 0.12, n_shelves)
  else
    block_instance(astana_bookshelf, loc_from_o_phi(add_z(pshv, level.height), fi), 1)
  end

person(p, level, fi) =
  block_instance(random_element(people), loc_from_o_phi(add_z(p, level.height), fi), 1)

dfii = 2*pi/n_frames

almost_equal(a, b) =
  let (a1, a2) = (a - 1e-06, a + 1e-06)
    a1 < b < a2
  end

any_in(value, list) = any(v -> almost_equal(value, v), list)

zero_list = [0, division(10*pi/6, 2*pi/6, n_big_cores - 1)...]
half_add_list = [dfii, division(dfii + 10*pi/6, dfii + 2*pi/6, n_big_cores - 1)...]
half_list = division(10*pi/6 - dfii, 2*pi/6 - dfii, n_big_cores - 1)

book_shelf_def_2(p, fi, z) =
  begin
    alfa_shelf = atan(0.4/mid_block_r())
    alfa_half_wall = atan(wall_thickness/2/mid_block_r())
    let (p1, p1_b, p3, p3_b, p2_1, p2_1_b, p4_1, p4_1_b) = (p + vpol(mid_block_r() + library_door, fi - alfa_shelf - alfa_half_wall), p + vpol(mid_block_r() + library_door*1.1, fi - 2*alfa_half_wall), p + vpol(mid_block_r() + block_l/2 + library_door/2, fi - alfa_shelf - alfa_half_wall/4), p + vpol(mid_block_r() + block_l/2 + library_door/2*1.1, fi - 2*alfa_half_wall), p + vpol(mid_block_r() + block_l/2 + -(library_door/2), fi + alfa_shelf + alfa_half_wall/2), p + vpol(mid_block_r() + block_l/2 + -(library_door/2)*1.1, fi + 2*alfa_half_wall), p + vpol(mid_block_r() + block_l + -library_door, fi + alfa_shelf), p + vpol(mid_block_r() + block_l + -(library_door*1.1), fi + 2*alfa_half_wall))
      if any_in(fi, zero_list)
        []
      elseif any_in(fi, half_list)
        book_shelf(p1, p1_b, level(z), fi)
        book_shelf(p3, p3_b, level(z), fi)
      elseif any_in(fi, half_add_list)
        book_shelf(p2_1, p2_1_b, level(z), pi + fi)
        book_shelf(p4_1, p4_1_b, level(z), pi + fi)
      else
        book_shelf(p1, p1_b, level(z), fi)
        book_shelf(p3, p3_b, level(z), fi)
        book_shelf(p2_1, p2_1_b, level(z), pi + fi)
        book_shelf(p4_1, p4_1_b, level(z), pi + fi)
      end
    end
  end

book_shelves(p) =
  map(z -> map(fi0 -> book_shelf_def_2(p, fi0, z), division(0, 2*pi, n_frames, false)),
      division(block_h, 2*block_h, mid_block_levels, false))


# TABLES _______________________________________________________________________

square_table(p, level, fi) =
  if current_backend() == archicad
    object("Dining Table Rectangle 18", p, level=level, angle=pi/2 + fi, properties=["iLeftSideType", 0, "RightSide", 0, "gs_top_mat", "Paint - Cobalt Blue", "gs_leg_mat", "Paint - Glossy White", "gs_frame_mat", "Paint - Glossy White", "gs_back_mat", "Paint - Glossy White", "gs_seat_mat", "Paint - Cobalt Blue"])
  else
    table_and_chairs(p, pi/2 + fi, level)
  end

table_set(p, fi, level) =
  begin
    fu = dfii/3
    space = (block_l - 3*library_door)/8
    radius = [library_door + space, library_door + 3*space, 2*library_door + 5*space, 2*library_door + 7*space]
    if any_in(fi, zero_list) || any_in(fi, half_list)
      []
    else
      map(r -> [square_table(p + vpol(mid_block_r() + r, fi + fu), level, fi + fu),
                square_table(p + vpol(mid_block_r() + r, fi + 2*fu), level, fi + 2*fu)][end],
          radius)
    end
  end

tables(p) =
  map(z -> map(fi0 -> table_set(p, fi0, level(z)), division(0, 2*pi, n_frames, false)),
      division(block_h, 2*block_h, mid_block_levels, false))

# LIGHTS _______________________________________________________________________

lights(p) =
  let dfi = 2*pi/n_frames
  map(z ->
    map(fi ->
      map(r ->
        with(default_level, level(z)) do
          pointlight(p+vcyl(r, fi, block_h/mid_block_levels-slab_thickness-1.6), range=12, intensity=0.005)
        end,
        division(mid_block_r(), mid_block_r()+block_l, 4)[2:2:end]),
    division(dfi/2, 2*pi+dfi/2, n_frames, false)),
  division(block_h, 2*block_h, mid_block_levels, false))
end

# PEOPLE _______________________________________________________________________

#=
place_people(p) =
  map(z -> map(fi0 -> place_person(p, fi0, level(z)), division(0, 2*pi, n_frames, false)),
            division(block_h, 2*block_h, mid_block_levels, false))

place_person(p, fi, level) =
  begin
    fu = dfii/2
    space = (block_l - 3*library_door)/8
    radius = [library_door + space, library_door + 3*space, 2*library_door + 5*space, 2*library_door + 7*space]
    map(r -> random(1.0) < 0.3 ? person(p + vpol(mid_block_r() + r, fi + fu), level, random_range(0, 2pi)) : nothing, #fi + fu),
        radius)
  end

=#

place_people(p) =
  let (r1, r2, r3) = (mid_block_r()+library_door/2, mid_block_r()+block_l/2, mid_block_r()+block_l-library_door/2)
    map(r -> map(fi -> map(z -> random(1.0) < 0.05 ?
                                    person(p+vpol(r, fi), level(z), random(2pi)) :
                                    nothing,
                           division(block_h, 2*block_h, mid_block_levels)),
                 division(0, 2*pi, 5*n_frames, false)),
        [r1, r2, r3])
  end
