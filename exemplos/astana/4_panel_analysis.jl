include("3_int_detail.jl")

# INDEX ________________________________________________________________________

# PANEL ANALISYS SETUP
# PHOTOVOLTAIC PANELS

# PANEL ANALISYS SETUP __________________________________________________________

panel_strip(op1, op2, op3, list1, list2, list10, list20) =
  let
    first_panel() = op1(list20[2], list20[1], list10[1])
    mid_panel_set() =
      begin
        op2(list1[1], list1[2], list2[3], list2[2])
        panel_strip(op1, op2, op3, list1[2:end], list2[2:end], list10, list20)
      end
    last_panel() = op3(list1[2], list2[2], list1[1])
    if length(list1) == 2
      first_panel()
      last_panel()
    else
      mid_panel_set()
    end
  end

panel_strips(op1, op2, op3, matrix) =
  map((l0, l1) -> panel_strip(op1, op2, op3, l0, l1, l0, l1),
      matrix[1:end-1],
      matrix[2:end])

façade_panels(op1, op2, op3, p) =
  begin
    liney = n_panels_per_strip(y_divisions)-1
    linex = n_panels_per_strip(x_divisions)-1
    let side_mtrx = facadeMATRIX(p, 5*column_r, linex, liney, n_panel_frames)
      panel_strips(op1, op2, op3, side_mtrx[1])
      panel_strips(op1, op2, op3, side_mtrx[2])
      panel_strips(op1, op2, op3, side_mtrx[3])
      panel_strips(op1, op2, op3, side_mtrx[4:end][1])
    end
  end

# Façade Analysis funtions %% RADIANCE BACK-END %%

scale2nine(rad_value) = #limited panel types
  max(ceil(rad_value/100*panel_positions), 0.1)

mid_point_func(colors_iter) =
  scale2nine(rgb_radiance(popfirst!(colors_iter)))

using DelimitedFiles
#using Iterators
colors = mapslices(
  a -> rgb(map(round, a)...),
  DelimitedFiles.readdlm(joinpath(@__DIR__, "Radiance.dat")),
  dims=2)

# PHOTOVOLTAIC PANELS __________________________________________________________

photovoltaic_panels(p) = let
  colors_iter = Iterators.Stateful(colors)
  with(default_panel_family, photopanel_family) do
    façade_panels(
      (p0, p1, p2) ->
        let f = .1*mid_point_func(colors_iter)
            pm = intermediate_loc(p1, p2, f)
          panel([p0, pm, p1])
        end,
      (p0, p1, p2, p3) ->
        let f = .1*mid_point_func(colors_iter)
            pm = intermediate_loc(p3, p1, f)
          panel([p0, pm, p3])
          panel([p3, p2, pm])
        end,
      (p0, p1, p2) ->
        let f = .1*mid_point_func(colors_iter)
            pm = intermediate_loc(p1, p0, f)
          panel([p2, p1, pm])
        end,
       p)
    end
  end
