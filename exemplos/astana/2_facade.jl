include("1_parts.jl")

# INDEX ________________________________________________________________________

# FAÇADE * FAÇADE * FAÇADE * FAÇADE * FAÇADE

# STEEL FRAMES
# STEEL BAYS
# FAÇADE MATRIX FOR SECONDARY ELEMENTS
# STEEL CONECTIONS
# NEW CONNECTIONS (ON GRID INTERSECTION POINTS)
# FAÇADE GLASS
# SECONDARY STRUCTURE


# STEEL FRAMES _________________________________________________________________

bar(pts) =
  map(beam, pts[1:end-1], pts[2:end])

rotated_bar(pts, angles) =
  map(beam, pts[1:end-1], pts[2:end], angles[1:end-1])

# STEEL FRAMES _________________________________________________________________

frames(p) =
  map((fi, psi) -> bar([rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiA),
                        rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiB),
                        rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiC),
                        rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiD),
                        rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiA)]),
      division(2*pi, 0, n_frames_for_frames, false),
      division(-(pi/2), -(pi/2) + skin_rotation, n_frames_for_frames, false))

#frames(xy(0,0))

# STEEL BAYS ___________________________________________________________________

points_in_between(p1, p2, n0, nv) =
  if nv == 0
    []
  else
    [xyz(p2.x - (p2.x - p1.x)/(n0 + 1)*nv, p2.y - (p2.y - p1.y)/(n0 + 1)*nv, p2.z - (p2.z - p1.z)/(n0 + 1)*nv),
     points_in_between(p1, p2, n0, nv - 1)...]
  end

bays_contour_points(p) =
  map((fi, psi) -> [rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiA),
                    rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiB),
                    rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiC),
                    rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiD)],
      division(2*pi, 0, n_frames_for_frames),
      division(-(pi/2), -(pi/2) + skin_rotation, n_frames_for_frames))

square_points = 4 + 2*x_divisions + 2*y_divisions

diagonal_track(f_points, n, count) =
  if count == square_points + 1
    []
  elseif n == length(f_points)
    [f_points[square_points], diagonal_track(f_points, 0, count + 1)...]
  else
    [f_points[n + 1], diagonal_track(f_points, n + 1, count + 1)...]
  end

f_points(p, fi, psi) =
  let (D, C, B, A) = (rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiD),
                      rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiC),
                      rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiB),
                      rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiA))
    [points_in_between(A, D, y_divisions, y_divisions)...,
     D,
     points_in_between(D, C, x_divisions, x_divisions)...,
     C,
     points_in_between(C, B, y_divisions, y_divisions)...,
     B,
     points_in_between(B, A, x_divisions, x_divisions)...,
     A]
  end

f_angles(psi) =
  division(psi, psi, square_points)

bays_points(p) =
  map((fi, psi, n) -> diagonal_track(f_points(p, fi, psi), n, 0),
      division(2*pi, 0, n_frames_for_frames),
      division(-(pi/2), -(pi/2) + skin_rotation, n_frames_for_frames),
      [collect(0:square_points)..., collect(0:square_points)...][1:n_frames_for_frames+1])

bays_angles(p) =
  map((psi, n) -> diagonal_track(f_angles(psi), n, 0),
      division(-(pi/2) + skin_rotation, -(pi/2), n_frames_for_frames),
      [collect(0:square_points)..., collect(0:square_points)...][1:n_frames_for_frames+1])

bay_contour_angles(p) =
  map(psi -> division(psi, psi, 4, false),
      division(-(pi/2) + skin_rotation, -(pi/2), n_frames_for_frames))

transpose_matrix(mtrx) =
  if mtrx[1] == []
    []
  else
    [map(l -> l[1], mtrx), transpose_matrix(map(l -> l[2:end], mtrx))...]
  end

bays(p) =
  begin
    map(rotated_bar,
        transpose_matrix(bays_contour_points(p)),
        transpose_matrix(bay_contour_angles(p)))
    map(rotated_bar,
        transpose_matrix(bays_points(p)),
        transpose_matrix(bays_angles(p)))
  end

#bays(xy(0,0))


# FAÇADE MATRIX FOR SECONDARY ELEMENTS _________________________________________

facadeMATRIX(p, rof, x_div, y_div, n_frame) =
  begin
    ro_f = rof + sqrt((frame_x()/2)^2 + (frame_y()/2)^2)
    cornerPT(fi, psi, psiPT) =
      rot_base_point(p, fi) + vsph(ro_f, fi, psi + psiPT)

    AB_side(fi, psi) =
      let (A, D) = (cornerPT(fi, psi, psiA), cornerPT(fi, psi, psiD))
        [A, points_in_between(A, D, y_div, y_div)..., D]
      end

    DC_side(fi, psi) =
      let (D, C) = (cornerPT(fi, psi, psiD), cornerPT(fi, psi, psiC))
        [D, points_in_between(D, C, x_div, x_div)..., C]
      end

    CB_side(fi, psi) =
      let (C, B) = (cornerPT(fi, psi, psiC), cornerPT(fi, psi, psiB))
        [C, points_in_between(C, B, y_div, y_div)..., B]
      end

    BA_side(fi, psi) =
      let (B, A) = (cornerPT(fi, psi, psiB), cornerPT(fi, psi, psiA))
        [B, points_in_between(B, A, x_div, x_div)..., A]
      end

    side_mtrx(side_func) =
      map((fi, psi) -> side_func(fi, psi),
          division(2*pi, 0, n_frame),
          division(-(pi/2), -(pi/2) + skin_rotation, n_frame))

    [side_mtrx(AB_side), side_mtrx(DC_side), side_mtrx(CB_side), side_mtrx(BA_side)]
  end

# STEEL CONECTIONS _____________________________________________________________

connections(p) =
  begin
    ro_f(x, y) = sqrt((x/2)^2 + (y/2)^2)
    psiA(x, y) = atan(y/2, x/2)
    psiB(x, y) = pi - atan(y/2, x/2)
    psiC(x, y) = pi + atan(y/2, x/2)
    psiD(x, y) = -atan(y/2, x/2)

    dfi = 2*pi/n_frames
    bb(dz, ri, fi, psi, _add_r, add_z, x, y, is_psi) =
      with(default_beam_family, beam_family_for_connections) do
        beam(p + vcyl(rfunc(ri) + _add_r, fi, dz + add_z),
             rot_base_point(p, fi) + vsph(ro_f(x, y), fi, psi + is_psi(x, y)))
      end

    in_vertical(dz, ri, fi, psi) =
      begin
        bb(dz, ri, fi, psi, 0, 0, block_l, frame_y(), psiD)
        bb(dz, ri, fi, psi, block_l, 0, frame_x(), 3*block_h, psiC)
        bb(dz, ri, fi, psi, 0, block_h, frame_x(), block_h, psiD)
      end

    out_vertical(dz, ri, fi, psi) =
      begin
        bb(dz, ri, fi, psi, block_l, block_h, block_l, frame_y(), psiB)
        bb(dz, ri, fi, psi, block_l, 0, frame_x(), block_h, psiB)
        bb(dz, ri, fi, psi, 0, block_h, frame_x(), 3*block_h, psiA)
      end

    in_horizontal(dz, ri, fi, psi) =
      begin
        bb(dz, ri, fi, psi, 0, block_h, block_l, frame_y(), psiD)
        bb(dz, ri, fi, psi, 0, 0, frame_x(), 3*block_h, psiC)
        bb(dz, ri, fi, psi, block_l, block_h, frame_x(), block_h, psiD)
      end

    out_horizontal(dz, ri, fi, psi) =
      begin
        bb(dz, ri, fi, psi, block_l, 0, block_l, frame_y(), psiB)
        bb(dz, ri, fi, psi, 0, 0, frame_x(), block_h, psiB)
        bb(dz, ri, fi, psi, block_l, block_h, frame_x(), 3*block_h, psiA)
      end

    middle_vertical(fi, psi) =
      with(default_beam_family, beam_family_for_connections) do
        beam(p + vcyl(mid_block_r(), fi, 2*block_h),
             rot_base_point(p, fi) + vsph(ro_f(frame_x(), block_h), fi, psi + psiA(frame_x(), block_h)))
        beam(p + vcyl(block_l + mid_block_r(), fi, block_h),
             rot_base_point(p, fi) + vsph(ro_f(frame_x(), block_h), fi, psi + psiC(frame_x(), block_h)))
      end

    middle_horizontal(fi, psi) =
      with(default_beam_family, beam_family_for_connections) do
        beam(p + vcyl(block_l + mid_block_r(), fi, 2*block_h),
             rot_base_point(p, fi) + vsph(ro_f(frame_x(), block_h), fi, psi + psiA(frame_x(), block_h)))
        beam(p + vcyl(mid_block_r(), fi, block_h),
             rot_base_point(p, fi) + vsph(ro_f(frame_x(), block_h), fi, psi + psiC(frame_x(), block_h)))
      end

    in_final(dz, ri, fi, psi) =
      begin
        bb(dz, ri, fi, psi, block_l, block_h, block_l, frame_y(), psiD)
        bb(dz, ri, fi, psi, 0, block_h, frame_x(), 3*block_h, psiC)
        bb(dz, ri, fi, psi, 0, 0, frame_x(), block_h, psiC)
      end

    out_final(dz, ri, fi, psi) =
      begin
        bb(dz, ri, fi, psi, 0, 0, block_l, frame_y(), psiB)
        bb(dz, ri, fi, psi, block_l, block_h, frame_x(), block_h, psiA)
        bb(dz, ri, fi, psi, block_l, 0, frame_x(), 3*block_h, psiA)
      end

    middle_final(fi, psi) =
      with(default_beam_family, beam_family_for_connections) do
        beam(p + vcyl(block_l + mid_block_r(), fi, 2*block_h),
             rot_base_point(p, fi) + vsph(ro_f(frame_x(), block_h), fi, psi + psiD(frame_x(), block_h)))
        beam(p + vcyl(mid_block_r(), fi, block_h),
             rot_base_point(p, fi) + vsph(ro_f(frame_x(), block_h), fi, psi + psiB(frame_x(), block_h)))
      end

    map((fi, dz, ri, psi) -> in_vertical(dz, ri, fi, psi),
        division(4*pi, 4*pi - 2*pi/3, n_frames/3, false),
        division(0, 2*block_h/3, n_frames/3, false),
        division(0 + dfi, 2*pi/3 + dfi, n_frames/3, false),
        division(-(pi/2), -(pi/2) + skin_rotation/3, n_frames/3, false))
    map((fi, dz, ri, psi) -> [out_vertical(dz, ri, fi, psi), middle_vertical(fi, psi)][end],
        division(2*pi, 2*pi - 2*pi/3, n_frames/3, false),
        division(2*block_h, block_h/3 + block_h, n_frames/3, false),
        division(2*pi + dfi, 2*pi + 2*pi/3 + dfi, n_frames/3, false),
        division(-(pi/2), -(pi/2) + skin_rotation/3, n_frames/3, false))
    map((fi, dz, ri, psi) -> in_horizontal(dz, ri, fi, psi),
        division(4*pi - 2*pi/3, 3*pi, n_frames/6, false),
        division(2*block_h/3, block_h, n_frames/6, false),
        division(2*pi/3 + dfi, pi + dfi, n_frames/6, false),
        division(-(pi/2) + skin_rotation/3, -(pi/2) + skin_rotation/2, n_frames/6, false))
    map((fi, dz, ri, psi) -> [out_horizontal(dz, ri, fi, psi), middle_horizontal(fi, psi)][end],
        division(2*pi - 2*pi/3, pi, n_frames/6, false),
        division(block_h/3 + block_h, block_h, n_frames/6, false),
        division(2*pi + 2*pi/3 + dfi, 3*pi + dfi, n_frames/6, false),
        division(-(pi/2) + skin_rotation/3, -(pi/2) + skin_rotation/2, n_frames/6, false))
    map((fi, dz, ri, psi) -> in_horizontal(dz, ri, fi, psi),
        division(3*pi, 2.5*pi, n_frames/4, false),
        division(block_h, 1.5*block_h, n_frames/4, false),
        division(pi + dfi, 1.5*pi + dfi, n_frames/4, false),
        division(-(pi/2) + skin_rotation/2, -(pi/2) + 3*skin_rotation/4, n_frames/4, false))
    map((fi, dz, ri, psi) -> [out_horizontal(dz, ri, fi, psi), middle_horizontal(fi, psi)][end],
        division(pi, 0.5*pi, n_frames/4, false),
        division(block_h, block_h/2, n_frames/4, false),
        division(3*pi + dfi, 3.5*pi + dfi, n_frames/4, false),
        division(-(pi/2) + skin_rotation/2, -(pi/2) + 3*skin_rotation/4, n_frames/4, false))
    map((fi, dz, ri, psi) -> in_final(dz, ri, fi, psi),
        division(2.5*pi, 2*pi, n_frames/4, false),
        division(1.5*block_h, 2*block_h, n_frames/4, false),
        division(1.5*pi + dfi, 2*pi + dfi, n_frames/4, false),
        division(-(pi/2) + 3*skin_rotation/4, -(pi/2) + skin_rotation, n_frames/4, false))
    map((fi, dz, ri, psi) -> [out_final(dz, ri, fi, psi), middle_final(fi, psi)][end],
        division(0.5*pi, 0, n_frames/4, false),
        division(block_h/2, 0, n_frames/4, false),
        division(3.5*pi + dfi, 4*pi + dfi, n_frames/4, false),
        division(-(pi/2) + 3*skin_rotation/4, -(pi/2) + skin_rotation, n_frames/4, false))
  end

#connections(xy(0,0))

# NEW CONNECTIONS (ON GRID INTERSECTION POINTS) ________________________________

nearest_point(p, pts) = pts[argmin(map(p1 -> distance(p, p1), pts))]
choose_closest(pts1, pts2) = map(i -> [i, nearest_point(i, pts2)], pts1)
flatten(a) = reduce(append!, a, init=[])

connection_point_inside(p) =
  begin
    corner_point(p, ri, fi, dz, plus_r, times_z) = p+vcyl(rfunc(ri)+plus_r, fi, dz+block_h*times_z)
    mid_point(p, fi, plus_r, times_z) = p+vcyl(mid_block_r()+plus_r, fi, block_h*times_z)

    corner_a(ri, fi, dz) = corner_point(p, ri, fi, dz, block_l, 1)
    corner_b(ri, fi, dz) = corner_point(p, ri, fi, dz, 0, 1)
    corner_c(ri, fi, dz) = corner_point(p, ri, fi, dz, 0, 0)
    corner_d(ri, fi, dz) = corner_point(p, ri, fi, dz, block_l, 0)

    mid_a(fi) = mid_point(p, fi, block_l, 2)
    mid_b(fi) = mid_point(p, fi, 0, 2)
    mid_c(fi) = mid_point(p, fi, 0, 1)
    mid_d(fi) = mid_point(p, fi, block_l, 1)

    #Block that starts on the bottom (+ middle points),
	   #first half,
     [flatten(map((dz, ri, fi) -> [corner_b(ri, fi, dz), corner_c(ri, fi, dz), corner_d(ri, fi, dz), mid_b(fi), mid_d(fi)],
         division(0, block_h, n_frames/2, false),
         division(global_dfi, pi+global_dfi, n_frames/2, false),
         division(4*pi, 3*pi, n_frames/2, false)))...,
     #;second half,
     flatten(map((dz, ri, fi) -> [corner_a(ri, fi, dz), corner_b(ri, fi, dz), corner_c(ri, fi, dz), mid_a(fi), mid_c(fi)],
         division(block_h, 2*block_h, n_frames/2, false),
         division(pi+global_dfi, 2*pi+global_dfi, n_frames/2, false),
         division(3*pi, 2*pi, n_frames/2, false)))...,
     #Block that starts on top,
     #first half,
     flatten(map((dz, ri, fi) -> [corner_a(ri, fi, dz), corner_b(ri, fi, dz), corner_d(ri, fi, dz)],
         division(2*block_h, block_h, n_frames/2, false),
         division(2*pi+global_dfi, 3*pi+global_dfi, n_frames/2, false),
         division(2*pi, pi, n_frames/2, false)))...,
     #second half,
     flatten(map((dz, ri, fi) -> [corner_a(ri, fi, dz), corner_c(ri, fi, dz), corner_d(ri, fi, dz)],
         division(block_h, 0, n_frames/2, false),
         division(3*pi+global_dfi, 4*pi+global_dfi, n_frames/2, false),
         division(pi, 0, n_frames/2, false)))...]
  end

beam2column(p1, p2) =
  begin
    isdif_almost5(a, b) = -10 < a-b < 10
    isdif_almost0(a, b) = -1 < a-b < 1

    current_backend() == "ArchiCAD" ?
      (isdif_almost5(p1.x, p2.x) && isdif_almost5(p1.y, p2.y) ?
         (isdif_almost0(p1.z, p2.z) ?
            beam(p1, p2) :
            (p1.z < p2.z ?
               column_two_points(p1, p2, width=column_d, depth=column_d) :
               column_two_points(p2, p1, width=column_d, depth=column_d))) :
         beam(p1, p2)) :
      free_column(p1, p2, family=facade_conect_fam)
  end

conection_pairs(p) =
  choose_closest(connection_point_inside(p),
                 #with frame corner points,
                flatten(flatten(facadeMATRIX(p, 0, x_divisions, y_divisions, n_frames_for_frames))),
                 #no frame corner points (only in between bay points),
                 #(flatten (facadeMATRIX4connections p 0 x-divisions y-divisions n-frames-for-frames))
                 )

new_conections(p) =
  let pairs = conection_pairs(p)
    with(default_beam_family, beam_family_for_connections) do
      for i in pairs
        beam2column(i[1], i[2])
      end
    end
  end

# FAÇADE GLASS _________________________________________________________________

facade_glass_strip(list1, list2, list10, list20, a) =
  if length(list1) == 1
    []
  else
    panel([list2[1], list2[2], list1[1]])
    panel([list1[1], list1[2], list2[2]])
    with(default_beam_family, beam_family_for_secondary_structure) do
      beam(list1[1], list2[2], a)
    end
    facade_glass_strip(list1[2:end], list2[2:end], list10, list20, a)
  end

facade_glass_strips(matrix, list_a) =
  map((l0, l1, a) -> facade_glass_strip(l0, l1, l0, l1, a),
      matrix[1:end-1],
      matrix[2:end],
      list_a[1:end-1])

glass_frame_angles = fill(division(-(pi/2) + skin_rotation, -(pi/2), n_frames), 4)

facade_glass(p) =
  map(facade_glass_strips,
      facadeMATRIX(p, -column_r, x_divisions, y_divisions, n_frames_for_frames),
      glass_frame_angles)

#facade_glass(xy(0,0))


# SECONDARY STRUCTURE __________________________________________________________

cross_list(func, listA, listB) =
  if listA == []
    []
  else
    func(listA[1], listB[1])
    cross_list(func, listA[2:end], listB[2:end])
  end

triangular_grig(A, B, C, nx, ny) =
  let (A_Cx, B_C, A_Cy, A_B) = (points_in_between(A, C, ny, ny), points_in_between(B, C, ny, ny), points_in_between(A, C, nx, nx), points_in_between(A, B, nx, nx))
    with(default_beam_family, beam_family_for_secondary_structure) do
      cross_list(beam, A_Cx, B_C)
      cross_list(beam, A_Cy, A_B)
    end
  end

square_grig(A, B, C, D, nx, ny) =
  let (A_B, C_D, A_C, B_D) = (points_in_between(A, B, ny, ny), points_in_between(C, D, ny, ny), points_in_between(A, C, nx, nx), points_in_between(B, D, nx, nx))
    with(default_beam_family, beam_family_for_secondary_structure) do
      cross_list(beam, A_B, C_D)
      cross_list(beam, A_C, B_D)
    end
  end

one_strip(list1, list2, list10, list20, nx, ny) =
  if length(list1) == 2
    triangular_grig(list10[1], list10[2], list20[1], nx, ny)
    triangular_grig(list2[2], list2[1], list1[2], nx, ny)
  else
    square_grig(list1[2], list1[3], list2[1], list2[2], nx, ny)
    one_strip(list1[2:end], list2[2:end], list10, list20, nx, ny)
  end

strips(matrix, nx, ny) =
  if length(matrix) == 1
    []
  else
    one_strip(matrix[2], matrix[1], matrix[2], matrix[1], nx, ny)
    strips(matrix[2:end], nx, ny)
  end

secondary_structure(p) =
  map(x -> strips(x, ss_div, ss_div),
      facadeMATRIX(p, column_r/2, x_divisions, y_divisions, n_frames_for_frames))

#secondary_structure(xy(0,0))
