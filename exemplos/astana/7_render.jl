include("6_generate.jl")

# INDEX ________________________________________________________________________

# RENDER : films
# STATIC SHOTS FROM ARCHICAD

# RENDER _______________________________________________________________________

#render_dir("C:\\Users\\")
render_size(1920, 1080)
render_size(1280, 1024)
saving_film_frames(true)

# Render 9 Astanas

# set_view(
#     xyz(-145.3925018310547,-267.9966125488281,260.5293884277344),
#     xyz(-145.0211181640625,-267.31585693359375,259.89801025390625),
#     35.0)

astana12seq()=
  for i in 0:3
      global skin_rotation = i*pi
      start_film("try_skin_$(i)")
      println("skin = $(i)")
      for j in [35, 55, 75]
        mid_block_r(j)
        #define_dependent_variables!()
        println("radius = $(j)")
        save_film_frame()
        astana(p)
      end
  end

astana3seq()=
  for i in [1, 2, 4]
      global skin_rotation = i*pi
      start_film("3astana7_$(i)pi")
      define_dependent_variables!()
      save_film_frame()
      astana(p)
  end

#=
#model: with computer object!
start_film("1_Dolly-Forth")
Astana_Dolly_Zoom_Forth()

#model: with computer object!
start_film("9_Dolly-Back")
Astana_Dolly_Zoom_Back()

#model: with furniture objects
with(default_lens, 16) do
  start_film("4_Walktrough-Library")
  Astana_walkthrough()
end

#model:exterior corrected bars and no cores
with(default_lens, 35) do
  start_film("3_Tracking-Line")
  Astana_Tracking_Line()
end

#model:exterior corrected bars and no cores
with(default_lens, 20) do
  start_film("5_Tracking-Enter-patio")
  Astana_Tracking_Forth_Walk()
end

#model:complete model exterior
with(default_lens, 15) do
  start_film("6_Panning-Up")
  Astana_Panning_Up()
end

#model:complete model exterior
with(default_lens, 15) do
  start_film("8_Panning-Patio")
  Astana_Panning()
end

#model:complete model exterior
with(default_lens, 18) do
  start_film("2_Tracking-Cyl")
  Astana_Tracking_Cyl()
end

#model:complete model exterior
#não faz bem as variações - astana sempre igual
with(default_lens, 20) do
  start_film("2_Tracking-Variations")
  Astana_Tracking_Variations()
end

#model: interior, no panels, no objects
with(default_lens, 20) do
  start_film("7_Traking-Birds-eye")
  Astana_inner_birds_eye()
end

#model: Astana by night with torches
with(default_lens, 16) do
  start_film("8_Walktrough-Library-Middle")
  Astana_inner_ring_middle()
end


# STATIC SHOTS FROM ARCHICAD ___________________________________________________

angle_to_focal(angle) = 18/tan(angle*pi/360)

set_view(
  xyz(-41.78203913296,-7.324368791276,19.74240825132),
  xyz(-42.56558840859,18.9885191132,21.3953352149),
  angle_to_focal(75))
render_view("StaticView1")

set_view(
  xyz(-43.00898349903,6.964163567463,16.69321229606),
  xyz(-62.75923881372,-10.48798309768,15.66162193621),
  angle_to_focal(75))
 render_view("StaticView2")

set_view(
  xyz(-56.75900553374,7.390249453945,26.52272137057),
  xyz(-37.97574750597,-11.09864521499,25.49113101072),
  angle_to_focal(75))
render_view("StaticView3")

render_size(800, 600)
render_size(1280, 1024)
set_view(
  xyz(-13.42715563755,58.17353782728,25.80961477423),
  xyz(2.781373911722,37.37786557678,25.07628511615),
  angle_to_focal(75))
render_view("StaticView4")

=#
