using Khepri

# Backend Mess__________________________________________________________________

#backend(autocad)
backend(unity)
#backend(rhino)
#set_length_unit("m")
delete_all_shapes()

#current_material(get_material("Materials/Wood/DarkWood"))

quadrangle_face(p0, p1, p2, p3; family=default_panel_family()) =
  if current_backend() == unity
    panel([p0, p1, p2, p3], family=family)
  else
    surface_polygon([p0, p1, p2, p3])
  end

sort_z(p0, p1, p2, p3)=
    p0.z > p1.z ?
      sort_z(p1, p0, p2, p3) :
      p1.z > p2.z ?
        sort_z(p0, p2, p1, p3) :
          p2.z > p3.z ?
            sort_z(p0, p1, p3, p2) :
            [p0, p1, p2, p3]


wall_4pts(p0, p1, p2, p3; family=default_wall_family()) =
    if current_backend() == unity
      let pts = sort_z(p0, p1, p2, p3)
        with(default_level_to_level_height, pts[end].z-pts[1].z) do
          wall([pts[1], pts[2]], family=family)
        end
      end
    else
        surface_polygon([p0, p1, p2, p3])
    end

#tests
#with(default_level, level(0)) do wall([x(10), x(40)][1], [x(10), x(40)][2], family=default_wall_family())  end
#min(xz(0, 30).z, xz(0,31).z, xz(20, 31).z, xz(20,30).z)
#wall(lowest(xz(0, 1), xz(0,2), xz(20, 2), xz(20,1))[2], lowest(xz(0, 1), xz(0,2), xz(20, 2), xz(20,1))[1])

c01_panel(pts; family=default_panel_family()) =
  if current_backend() == unity
    panel(pts, family=family)
  else
    surface_polygon(pts)
  end

# Material Layers_______________________________________________________________

# Materiais definidos através de layers
#mosaicos_chao = create_layer("Mosaicos Chao")
#reboco = create_layer("Reboco")
#glass = create_layer("Glass")
#portas = create_layer("Portas")
#intervention = create_layer("Intervention")
#la = create_layer("La de Rocha")
#mat_secretaria = create_layer("Secretarias")
#audience = create_layer("Alunos")

default_slab_family(current_backend() == revit ? default_slab_family() :
                    slab_family_element(default_slab_family(), thickness=0.02))

set_backend_family(default_panel_family(), unity, unity_material_family("Materials/Plaster/Plaster1"))
set_backend_family(default_wall_family(), unity, unity_material_family("Materials/Plaster/Plaster1"))
set_backend_family(default_slab_family(), unity, unity_material_family("Materials/Plaster/Plaster1"))

c01_glass_panel_family = panel_family_element(default_panel_family())
c01_wood_panel_family = panel_family_element(default_panel_family())
c01_floor_slab_family = slab_family_element(default_slab_family())
c01_plaster_panel_family = panel_family_element(default_panel_family())
#c01_wall_panel_family = panel_family_element(default_panel_family())
set_backend_family(c01_glass_panel_family, unity, unity_material_family("Materials/Glass/Glass"))
set_backend_family(c01_wood_panel_family, unity, unity_material_family("Materials/Wood/Wood1"))
set_backend_family(c01_floor_slab_family, unity, unity_material_family("prefabs/bookshelves/dependencies/Patterns/RedMarbleTile"))
set_backend_family(c01_plaster_panel_family, unity, unity_material_family("Materials/Plaster/Plaster1"))


#bogus_layer = Parameter(la)

# Room Dimentions (C01)_________________________________________________________

# Dimensões gerais da sala
C = 14.8 # Comprimento
L = 8.5 # Largura
H = 3.32 # Altura

# Room Geometry_________________________________________________________________

# Floor
import Base.floor
floor(p, C, L) =
  #with(bogus_layer,mosaicos_chao) do
    slab([p, p + vx(L), p + vxy(L, C), p + vy(C)], family=c01_floor_slab_family)
  #end

# Ceiling
import Base.ceil
ceil(p, C, L, H) =
  #with(bogus_layer,reboco) do
    box(p + vxyz(L, C, H), p + vz(H+0.1))
      #=quadrangle_face(p + vz(H),
                      p + vyz(C, H),
                      p + vxyz(L, C, H),
                      p + vxz(L, H))=#
  #end

# Windows_______________________________________________________________________

# Window Dimentions
l_wnd = 1.13 # Largura
h_wnd = 1.505 # Altura
depth_wnd = 0.545 # Profundidade

# Window Cove
l_wnd_cove = 1.16 # Largura
h_wnd_cove = 1.615 # Altura

# Difference between window and window cove
wnd_delta_l = (l_wnd_cove - l_wnd)/2 # Largura
wnd_delta_h = h_wnd_cove - h_wnd # Altura

window_pane(p) =
  #with(bogus_layer, glass) do
    c01_panel([p, p + vy(l_wnd), p + vyz(l_wnd, h_wnd), p + vz(h_wnd)], family=c01_glass_panel_family)
  #end

window_cove(p) =
  #with(bogus_layer,reboco) do
  begin
      quadrangle_face(p,
                      p + vy(l_wnd_cove),
                      p + vxyz(-depth_wnd, l_wnd_cove - wnd_delta_l, wnd_delta_h),
                      p + vxyz(-depth_wnd, wnd_delta_l, wnd_delta_h))
                      #family=c01_plaster_panel_family)
      quadrangle_face(p,
                      p + vxyz(-depth_wnd, wnd_delta_l, wnd_delta_h),
                      p + vxyz(-depth_wnd, wnd_delta_l, h_wnd_cove),
                      p + vz(h_wnd_cove))
                      #family=c01_plaster_panel_family)
      quadrangle_face(p + vz(h_wnd_cove),
                      p + vxyz(-depth_wnd, wnd_delta_l, h_wnd_cove),
                      p + vxyz(-depth_wnd, l_wnd_cove - wnd_delta_l, h_wnd_cove),
                      p + vyz(l_wnd_cove, h_wnd_cove))
                      #family=c01_plaster_panel_family)
      quadrangle_face(p + vy(l_wnd_cove),
                      p + vyz(l_wnd_cove, h_wnd_cove),
                      p + vxyz(-depth_wnd, l_wnd_cove - wnd_delta_l, h_wnd_cove),
                      p + vxyz(-depth_wnd, l_wnd_cove - wnd_delta_l, wnd_delta_h))
                      #family=c01_plaster_panel_family)
  end
  #end

window_c01(p) =
  begin
    window_cove(p)
    window_pane(p + vxyz(-depth_wnd, wnd_delta_l, wnd_delta_h))
  end

#(window (x 0))

# Doors_________________________________________________________________________

# Front Door (1) Dimentions
h_door = 2.28 # Altura
l_door1 = 1.365 # Largura
depth_dr1 = 0.365 # Profundidade

# Front Door (1) Cove
l_dr1_cove = 1.38 # Largura

# Difference between door1 and door1 cove
dr1_delta_l = (l_dr1_cove - l_door1)/2 # Largura

door1_pane(p) =
  #with(bogus_layer, portas) do
    quadrangle_face(p,
                    p + vz(h_door),
                    p + vxz(l_door1, h_door),
                    p + vx(l_door1))
#  end

door1_cove_floor(p) =
  #with(bogus_layer,mosaicos_chao) do
    quadrangle_face(p,
                    p + vxy(dr1_delta_l, -depth_dr1),
                    p + vxy(l_dr1_cove - dr1_delta_l, -depth_dr1),
                    p + vx(l_dr1_cove),
                    family=c01_plaster_panel_family)
#  end

door1_cove(p) =
  #with(bogus_layer,reboco) do
  begin
      quadrangle_face(p,
                      p + vz(h_door),
                      p + vxyz(dr1_delta_l, -depth_dr1, h_door),
                      p + vxy(dr1_delta_l, -depth_dr1))
      quadrangle_face(p + vz(h_door),
                      p + vxz(l_dr1_cove, h_door),
                      p + vxyz(l_dr1_cove - dr1_delta_l, -depth_dr1, h_door),
                      p + vxyz(dr1_delta_l, -depth_dr1, h_door))
      quadrangle_face(p + vx(l_dr1_cove),
                      p + vxy(l_dr1_cove - dr1_delta_l, -depth_dr1),
                      p + vxyz(l_dr1_cove - dr1_delta_l, -depth_dr1, h_door),
                      p + vxz(l_dr1_cove, h_door))
  end
  #end

door1(p) =
  begin
    door1_cove(p)
    door1_cove_floor(p)
    door1_pane(p + vxy(dr1_delta_l, -depth_dr1))
  end

#(door1 (x 0))

# Side Door (2) Dimentions
l_door2 = 1.38 # Largura
depth_dr2 = 0.535 # Profundidade

# Side Door (2) Cove
l_dr2_cove = 1.4 # Largura

# Difference between door2 and door2 cove
dr2_delta_l = (l_dr2_cove - l_door2)/2 # Largura

door2_pane(p) =
  #with(bogus_layer,portas) do
    quadrangle_face(p,
                    p + vz(h_door),
                    p + vyz(l_door2, h_door),
                    p + vy(l_door2))
  #end

door2_cove_floor(p) =
  #with(bogus_layer,mosaicos_chao) do
    quadrangle_face(p,
                    p + vxy(depth_dr2, dr2_delta_l),
                    p + vxy(depth_dr2, l_dr2_cove - dr2_delta_l),
                    p + vy(l_dr2_cove),
                    family=c01_plaster_panel_family)
  #end

door2_cove(p) =
  #with(bogus_layer,reboco) do
  begin
      quadrangle_face(p,
                      p + vz(h_door),
                      p + vxyz(depth_dr2, dr2_delta_l, h_door),
                      p + vxy(depth_dr2, dr2_delta_l))
      quadrangle_face(p + vz(h_door),
                      p + vyz(l_dr2_cove, h_door),
                      p + vxyz(depth_dr2, l_dr2_cove - dr2_delta_l, h_door),
                      p + vxyz(depth_dr2, dr2_delta_l, h_door))
      quadrangle_face(p + vy(l_dr2_cove),
                      p + vxy(depth_dr2, l_dr2_cove - dr2_delta_l),
                      p + vxyz(depth_dr2, l_dr2_cove - dr2_delta_l, h_door),
                      p + vyz(l_dr2_cove, h_door))
  end
  #end

door2(p) =
  begin
    door2_cove(p)
    door2_cove_floor(p)
    door2_pane(p + vxy(depth_dr2, dr2_delta_l))
  end

#(door2 (x 0))

# Walls_________________________________________________________________________

# Front wall, where the whiteboard is located.
whiteboard_wall(p, C, L, H) =
  #with(bogus_layer,reboco) do
    wall_4pts(p + vy(C),
                    p + vxy(L, C),
                    p + vxyz(L, C, H),
                    p + vyz(C, H))
  #end

# Back Wall, where the entrance door is located
# Door 1 Location in the wall
dr1_loc = L - 0.6 - l_dr1_cove

entrance_door_wall(p, C, L, H) =
  let d_aux = dr1_loc + l_dr1_cove
    #with(bogus_layer,reboco) do
        wall_4pts(p + vz(h_door),
                        p + vz(H),
                        p + vxz(L, H),
                        p + vxz(L, h_door))
        #print(min(cz(p + vz(h_door)), cz(p + vz(H)), cz(p + vxz(L, H)), cz(p + vxz(L, h_door))))
        #print(max(cz(p + vz(h_door)), cz(p + vz(H)), cz(p + vxz(L, H)), cz(p + vxz(L, h_door))))
        wall_4pts(p,
                        p + vz(h_door),
                        p + vxz(dr1_loc, h_door),
                        p + vx(dr1_loc))
        wall_4pts(p + vx(d_aux),
                        p + vxz(d_aux, h_door),
                        p + vxz(L, h_door),
                        p + vx(L))
    #end
  end

# Side Door Wall, where the side door is located
# Door 2 Location in the wall
dr2_loc = C - 4.85 - l_dr2_cove

side_door_wall(p, C, L, H) =
  let d_aux = dr2_loc + l_dr2_cove
    #with(bogus_layer,reboco) do
        wall_4pts(p + vxz(L, h_door),
                        p + vxz(L, H),
                        p + vxyz(L, C, H),
                        p + vxyz(L, C, h_door))
        wall_4pts(p + vx(L),
                        p + vxz(L, h_door),
                        p + vxyz(L, dr2_loc, h_door),
                        p + vxy(L, dr2_loc))
        wall_4pts(p + vxy(L, d_aux),
                        p + vxyz(L, d_aux, h_door),
                        p + vxyz(L, C, h_door),
                        p + vxy(L, C))
    #end
  end


# Windows Wall, where the windows are located
# Windows Locations in the wall
wnd1_loc = 1.075
wnd2_loc = wnd1_loc + l_wnd_cove + 2.64
wnd3_loc = wnd2_loc + l_wnd_cove + 2.64
wnd4_loc = wnd3_loc + l_wnd_cove + 2.64
h_wnd_sill = 1.23
h_wnd_lintel = h_wnd_sill + h_wnd_cove

windows_wall(p, C, L, H) =
  #with(bogus_layer,reboco) do
  begin
      wall_4pts(p,
                      p + vy(C),
                      p + vyz(C, h_wnd_sill),
                      p + vz(h_wnd_sill))
      wall_4pts(p + vz(h_wnd_lintel),
                      p + vyz(C, h_wnd_lintel),
                      p + vyz(C, H),
                      p + vz(H))
      wall_4pts(p + vz(h_wnd_sill),
                      p + vyz(wnd1_loc, h_wnd_sill),
                      p + vyz(wnd1_loc, h_wnd_lintel),
                      p + vz(h_wnd_lintel))
      wall_4pts(p + vyz(wnd1_loc + l_wnd_cove, h_wnd_sill),
                      p + vyz(wnd2_loc, h_wnd_sill),
                      p + vyz(wnd2_loc, h_wnd_lintel),
                      p + vyz(wnd1_loc + l_wnd_cove, h_wnd_lintel))
      wall_4pts(p + vyz(wnd2_loc + l_wnd_cove, h_wnd_sill),
                      p + vyz(wnd3_loc, h_wnd_sill),
                      p + vyz(wnd3_loc, h_wnd_lintel),
                      p + vyz(wnd2_loc + l_wnd_cove, h_wnd_lintel))
      wall_4pts(p + vyz(wnd3_loc + l_wnd_cove, h_wnd_sill),
                      p + vyz(wnd4_loc, h_wnd_sill),
                      p + vyz(wnd4_loc, h_wnd_lintel),
                      p + vyz(wnd3_loc + l_wnd_cove, h_wnd_lintel))
      wall_4pts(p + vyz(wnd4_loc + l_wnd_cove, h_wnd_sill),
                      p + vyz(C, h_wnd_sill),
                      p + vyz(C, h_wnd_lintel),
                      p + vyz(wnd4_loc + l_wnd_cove, h_wnd_lintel))
  end
  #end


# Caixas de Instalações (junto ao tecto)________________________________________

# Caixa de Instalações Grande (1) -> Dimensões
c_ci1 = 3.7 # Comprimento
l_ci1 = 7.835 # Largura
h_ci1 = H - 1.005 # Altura

caixa1(p, C, L, H) =
  #with(bogus_layer,reboco) do
    box(p + vxyz(L, C, H), p + vxyz(l_ci1, c_ci1, h_ci1))
      #=quadrangle_face(p + vxyz(l_ci1, C, H),
                      p + vxyz(l_ci1, C, h_ci1),
                      p + vxyz(l_ci1, c_ci1, h_ci1),
                      p + vxyz(l_ci1, c_ci1, H))
      quadrangle_face(p + vxyz(l_ci1, C, h_ci1),
                      p + vxyz(L, C, h_ci1),
                      p + vxyz(L, c_ci1, h_ci1),
                      p + vxyz(l_ci1, c_ci1, h_ci1))
      quadrangle_face(p + vxyz(l_ci1, c_ci1, H),
                      p + vxyz(l_ci1, c_ci1, h_ci1),
                      p + vxyz(L, c_ci1, h_ci1),
                      p + vxyz(L, c_ci1, H))=#
  #end

# Caixa de Instalações Pequena (2) -> Dimensões
c_ci2 = c_ci1 # Comprimento
l_ci2 = L - 0.425 # Largura
h_ci2 = H - 0.895 # Altura

caixa2(p, C, L, H) =
  #with(bogus_layer,reboco) do
    box(p + vxyz(l_ci2, c_ci2, h_ci2), p + vxz(L, H))
      #=quadrangle_face(p + vxyz(l_ci2, c_ci2, h_ci2),
                      p + vxyz(L, c_ci2, h_ci2),
                      p + vxz(L, h_ci2),
                      p + vxz(l_ci2, h_ci2))
      quadrangle_face(p + vxyz(l_ci2, c_ci2, h_ci2),
                      p + vxz(l_ci2, h_ci2),
                      p + vxz(l_ci2, H),
                      p + vxyz(l_ci2, c_ci2, H))=#
  #end

# Vigas_________________________________________________________________________

# Vigas Altas -> Dimensões
l_va = 0.355 # Largura
h_va = H - 0.565 # Altura

viga_alta(p, C, L, H) =
  #with(bogus_layer,reboco) do
    box(p + vz(H), p + vxyz(L, l_va, h_va))
      #=quadrangle_face(p + vz(H),
                      p + vz(h_va),
                      p + vxz(L, h_va),
                      p + vxz(L, H))
      quadrangle_face(p + vz(h_va),
                      p + vyz(l_va, h_va),
                      p + vxyz(L, l_va, h_va),
                      p + vxz(L, h_va))
      quadrangle_face(p + vxyz(L, l_va, H),
                      p + vxyz(L, l_va, h_va),
                      p + vyz(l_va, h_va),
                      p + vyz(l_va, H))=#
  #end

# Localização das vigas altas no tecto
va_y1 = 3.35 # Posição 1ª Viga
va_y2 = 7.155 # Posição 2ª Viga
va_y3 = 10.945 # Posição 3ª Viga

vigas_altas(p, C, L, H) =
  begin
    viga_alta(p + vy(va_y1), C, L, H)
    viga_alta(p + vy(va_y2), C, L, H)
    viga_alta(p + vy(va_y3), C, L, H)
  end

# Vigas Baixas -> Dimensões
l_vb = 0.247 # Largura
h_vb = H - 0.3 # Altura

viga_baixa(p, C, L, H) =
  #with(bogus_layer,reboco) do
    box(p + vyz(C, H), p + vxz(l_vb, h_vb))
      #=quadrangle_face(p + vyz(C, H),
                      p + vyz(C, h_vb),
                      p + vz(h_vb),
                      p + vz(H))
      quadrangle_face(p + vz(h_vb),
                      p + vyz(C, h_vb),
                      p + vxyz(l_vb, C, h_vb),
                      p + vxz(l_vb, h_vb))
      quadrangle_face(p + vxz(l_vb, H),
                      p + vxz(l_vb, h_vb),
                      p + vxyz(l_vb, C, h_vb),
                      p + vxyz(l_vb, C, H))=#
  #end


# Localização das vigas baixas no tecto
vb_x1 = 2.675 # Posição 1ª Viga
vb_x2 = 5.855 # Posição 2ª Viga

vigas_baixas(p, C, L, H) =
  begin
    viga_baixa(p + vx(vb_x1), C, L, H)
    viga_baixa(p + vx(vb_x2), C, L, H)
  end

# Secretárias e alunos________________________________________________________

# Dimensões
c_sec = 0.605 # Comprimento
l_sec = 0.505 # Largura
h_sec = 0.745 # Altura do Tampo

avanco_tras = 1.5 #1.3 # Avanços Secretárias

# Número de Secretárias
n_sec_L = 7
n_sec_C = 11 #12

tampo_secretaria(p) = quadrangle_face(p, p + vx(c_sec), p + vxy(c_sec, l_sec), p + vy(l_sec))

student_table_family = table_chair_family_element(
    default_table_chair_family(),
    table_family=table_family_element(default_table_family(),
        length=c_sec,
        width=l_sec,
        height=h_sec),
    chairs_top=0,
    chairs_bottom=0,
    chairs_right=0,
    chairs_left=1)

# Unity objects
school_table = block("Prefabs/TablesChairs/SchoolTableChair/SchoolTableChair")
people = [block("Prefabs/People/Person$(i)") for i in 1:6]
random_element(lst) = lst[random_range(1, length(lst))]

secretaria(p, x, y) =
  if current_backend() == unity
    p_sec = p + vxyz(x+c_sec/2, y+l_sec/2, 0)
    p_pep = p + vxyz(x-c_sec/4, y, 0)
    block_instance(school_table, p_sec, 1)
    random(10) == 11 ? # == 0 ?
      rotate(block_instance(random_element(people), p_pep, random_range(0.9,1.1)), pi/2, p_pep) :
        nothing
  else
    "table_and_chairs(p + vxyz(x+c_sec/2, y+l_sec/2, 0), family=student_table_family)"
  end

secretarias(p, C, L, H) =
    #with(bogus_layer,mat_secretaria) do
        map_division((x, y) -> secretaria(p, x, y),
               c_sec,
               L - 2*c_sec,
               n_sec_L-1,
               avanco_tras,
               C - 1.5*avanco_tras,
               n_sec_C-1)
    #end

# Luzes_________________________________________________________________________

# Amplitude da sinusoide Intervenção
a = 0.07
min_depth = Parameter(-a)
max_depth = Parameter(a)

light(p) = pointlight(p, range=5, intensity=0.003)

lights(p, C, L, H)=
  map_division((x, y) -> light(p+vxyz(x, y, h_va-a)),
               L/6,
               L-L/6,
               3,
               C/12,
               C-C/12,
               7)

# Quadro_________________________________________________________________________

blackboard = block("Prefabs/Blackboards/Blackboard1/Blackboard1")
board(p, C, L, H) =
  let p_board = p + vxyz(L/4, C-0.1, 1)
    rotate(block_instance(blackboard, p_board, .6), pi, p_board)
  end

# INTERVENTION _________________________________________________________________

######### Parametros Atractor

frequency = Parameter(15)

# Teste 1
#attractors_loc = [xy(2, 1), xy(6, 6), xy(0.5, 9), xy(4, 14)]
#attractors_radius = Parameter([4.5, 4.7, 3.8, 5.2])

# Teste 2
#attractors_loc = [xy(8, 0), xy(2, 1), xy(2.5, 4.75), xy(6, 6), xy(1.5, 8), xy(4.5, 10), xy(8.5, 11), xy(4, 14)]
#attractors_radius = Parameter([4.3, 4.1, 3.2, 4, 3.6, 3.3, 3.4, 4.5]) #3.1 a 4.5

# Teste 3 <- PADRÃO ESCOLHIDO
attractors_loc = Parameter([xy(2, 0), xy(6, 4), xy(1, 7), xy(6.2, 9.5), xy(4, 14)])
attractors_radius = Parameter([4.3, 3.8, 4.4, 3.7, 4.5]) #3.1 a 4.5

#= Imagens para o paper
for i in [xy(2, 0), xy(6, 4), xy(1, 7), xy(6.2, 9.5), xy(4, 14)]
  sphere(i+vz(H), 0.2)
end
attractors_loc = Parameter([xy(2, 0), xy(6, 4), #=xy(1, 7),=# xy(6.2, 9.5), xy(4, 14), xy(L/3+0.5, 5)])
attractors_radius = Parameter([4.3, 3.8, #=4.4,=# 3.7, 4.5, 4.1]) #3.1 a 4.5
#place person in loc
at_l=xy(L/3+0.5, 5) #xy(1, 7) #2 boy, 4 girl
rotate(block_instance(people[4], at_l, random_range(0.9,1.1)), pi/2+pi, at_l)
=#

######### Sinusoide

#=
pontos_atractor(p, a, vec, i0, i1, n_pts) =
    map_division(i0, i1, n_pts) do x
        let dists = map(attractor -> distance(p + vec*x, attractor), attractors_loc)
            min_dist = min(dists...)
            p + vec*x + vz(min(max_depth(), max(min_depth(), sin(min_dist/0.075)*a)))
        end
    end
=#

pontos_atractor(p, a, vec, i0, i1, n_pts) =
    map_division(i0, i1, n_pts) do x
        let cur_p = p + vec*x
            #dists = map(attractor -> distance(cur_p, attractor), attractors_loc())
            dists = distance.(cur_p, attractors_loc())
            #dists = dists[dists .< attractors_radius()]
            dists = attractors_radius() .- dists
            ampls = map(dists) do dist
                        dist < 0 ? 0 : sin(dist*frequency())*a/sqrt(1+dist) #α 1/√dist
                    end
            final_ampl = reduce(+, ampls, init=0)     #Soma de amplitudes para efeitos de interferências
            cur_p + vz(min(max_depth(), max(min_depth(), final_ampl)))  # Min e Max depth põem a curva dentro dos limites
        end
    end

# Pontos da curva de cima do Perfil
pts_above(p, a, vec, h, i0, i1, n_pts) =
    let pts = pontos_atractor(p, a, vec, i0, i1, n_pts),
        max_height = p.z + 1.5h ,  # Certifica-se que a curva de cima nunca excede um determinado limite
        limit_height = p -> p-vz(p.z)+vz(min(max_height, p.z + h + a/2))
      map(limit_height, pts)
    end

# Pontos intermédios (entre pts e pts_above) para as ranhuras
pontos_intermedios(p, a, vec, h, i0, i1, n_pts) =
    let pts = pontos_atractor(p, a, vec, i0, i1, n_pts)
      [pt2+(pt1-pt2)/2 for (pt1, pt2) in zip(pts_above(p, a, vec, h, i0, i1, n_pts), pts)]
    end

perfil_atractor(p, a, vec, h, i0, i1, n_pts) =
  let pts = pontos_atractor(p, a, vec, i0, i1, n_pts),
      pts2 = pts_above(p, a, vec, h, i0, i1, n_pts),
      profile = closed_path_sequence(
                  open_spline_path(pts),
                  open_polygonal_path([pts[end], pts2[end]]),
                  open_spline_path(reverse(pts2)),
                  open_polygonal_path([pts2[1], pts[1]]))
    #surface_path(profile)
    c01_panel(path_vertices(profile), family=c01_wood_panel_family)
  end

#spline(pontos_intermedios(u0(), 1, vx(1), 0.15, 0, 3, 50))

#=
perfil_atractor(u0(), 10, vx(1), 0.1, 0, 10, 30)
perfil_atractor(u0(), 10, vy(1), 0.1, 0, 10, 30)
perfil_atractor(u0(), 10, vxy(1,1), 0.1, 0, 10, 30)
=#

ranhura(p, vec, e_ranhura, h_ranhura) =
  if current_backend() == unity
    empty_shape()
  else
    surface_polygon(p + vec*-(e_ranhura/2),
                    p + vec*e_ranhura/2,
                    p + vec*e_ranhura/2 + vz(h_ranhura),
                    p + vec*-(e_ranhura/2) + vz(h_ranhura))
  end

#ranhura(u0(), vx(1), 0.01, 0.2)

ranhuras(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, n_ranhuras) =
  map(pt -> ranhura(pt, vec, e_ranhura, h_ranhura*5),
    pontos_intermedios(p, a, vec, h, i0, i1, n_ranhuras)[1:end-1])

#ranhuras(u0(), (h_vb - h_va)*1//3, vx(1), 0.005, 0.13, 0.06, 0, 3, 10)

perfil_com_ranhuras(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, space_between, n_pts, n_ranhuras) =
    let rs = ranhuras(p + vec*space_between/2, a, vec, e_ranhura, h, h_ranhura, i0, i1, n_ranhuras)
        subtraction(perfil_atractor(p, a, vec, h, i0, i1, n_pts),
                union(rs))
    end


# Perfil Tipo A (Perfil de Suporte)
# Altura h_ranhura positiva para todas as ranhuras
perfil_tipo_A(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, space_between, n_pts, n_ranhuras) =
    perfil_com_ranhuras(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, space_between, n_pts, n_ranhuras)

# Perfil Tipo B
# Altura h_ranhura negativa nas ranhuras em contacto com os perfis de suporte (tipo A), e positiva para as restantes ranhuras
ranhuras_B(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, n_ranhuras) =
  [ranhura(pt, vec, e_ranhura, h_ranhura*5*orient)
  for (pt, orient) in zip(pontos_intermedios(p, a, vec, h, i0, i1, n_ranhuras)[1:end-1], [-1, map(x -> x/x, division(1, n_ranhuras-2, n_ranhuras-2, false)) ..., -1])]

#=
perfil_tipo_B(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, space_between, n_pts, n_ranhuras) =
    let rs = ranhuras_B(p + vec*space_between/2, a, vec, e_ranhura, h, h_ranhura, i0, i1, n_ranhuras)
        subtraction(perfil_atractor(p, a, vec, h, i0, i1, n_pts),
                union(rs))
    end
=#

perfil_tipo_B(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, space_between, n_pts, n_ranhuras) =
    let rs = ranhuras_B(p + vec*space_between/2, a, vec, e_ranhura, h, h_ranhura, i0, i1, n_ranhuras)
        foldr((a,b)->subtraction(b,a),
            rs,
            init=perfil_atractor(p, a, vec, h, i0, i1, n_pts))
    end

# Perfil Tipo C
# Altura h_ranhura negativa nas ranhuras dos perfis das extremidades (perfis tipo B), e positiva para as restantes ranhuras
perfil_tipo_C(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, space_between, n_pts, n_ranhuras) =
    perfil_tipo_B(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, space_between, n_pts, n_ranhuras)

# Perfil Tipo D (Topo)
# Altura h_ranhura negativa para todas as ranhuras
perfil_tipo_D(p, a, vec, e_ranhura, h, h_ranhura, i0, i1, space_between, n_pts, n_ranhuras) =
    perfil_com_ranhuras(p, a, vec, e_ranhura, h, -h_ranhura, i0, i1, space_between, n_pts, n_ranhuras)


#perfil_tipo_C(u0(), (h_vb - h_va)*1//3, vx(1), 0.003, 0.13, 0.06, 0, 10*0.15, 0.15, 10, 10)

perfil_n_pts = 100

perfis_atractor_with_support(p, a, vec0, vec1, e_ranhura, h, h_ranhura, i0, i1, t0, t1, space_between, n_perfis_x, n_perfis_y) =
  #with(bogus_layer,intervention) do
      let ys = division(t0, t1, n_perfis_y, true)
          # Primeiro Perfil de Suporte (Perfis Tipo A)
          [perfil_tipo_A(p + vec1*(ys[1]+space_between/2), a, vec0, e_ranhura, h, h_ranhura, i0, i1, space_between, perfil_n_pts, n_perfis_x),
          # Perfis Intermédios (Perfis Tipo C)
          map(y -> perfil_tipo_C(p + vec1*(y+space_between/2), a, vec0, e_ranhura, h, h_ranhura, i0, i1, space_between, perfil_n_pts, n_perfis_x),
          ys[2:end-2]) ...,
          # Último Perfil de Suporte (Perfis Tipo A)
          perfil_tipo_A(p + vec1*(ys[end]-space_between/2), a, vec0, e_ranhura, h, h_ranhura, i0, i1, space_between, perfil_n_pts, n_perfis_x)]
      end
  #end

perfis_atractor_without_support(p, a, vec0, vec1, e_ranhura, h, h_ranhura, i0, i1, t0, t1, space_between, n_perfis_x, n_perfis_y) =
    #with(bogus_layer,intervention) do
        let xs = division(i0, i1, n_perfis_x, true)
            # Primeiro Perfil da Extremidade (Perfis Tipo B)
            [perfil_tipo_B(p + vec1*(xs[1]+space_between/2), a, vec0, e_ranhura, h, h_ranhura, t0, t1, space_between, perfil_n_pts, n_perfis_y),
            # Perfis de Topo (Perfis Tipo D)
            map(x -> perfil_tipo_D(p + vec1*(x+space_between/2), a, vec0, e_ranhura, h, h_ranhura, t0, t1, space_between, perfil_n_pts, n_perfis_y),
            xs[2:end-2]) ...,
            # Último Perfil da Extremidade (Perfis Tipo B)
            perfil_tipo_B(p + vec1*(xs[end]-space_between/2), a, vec0, e_ranhura, h, h_ranhura, t0, t1, space_between, perfil_n_pts, n_perfis_y)]
        end
    #end

painel(p, a, h, h_ranhura, i0, i1, t0, t1, space_between, n_perfis_x, n_perfis_y) =
  let
    #Espessura dos perfis = Espessura das ranhuras
    e_perfil = 0.005
    [perfis_atractor_with_support(p,
                                  a,
                                  vx(1),
                                  vy(1),
                                  e_perfil,
                                  h,
                                  h_ranhura,
                                  i0,
                                  i1,
                                  t0,
                                  t1,
                                  space_between,
                                  n_perfis_x,
                                  n_perfis_y),
    perfis_atractor_without_support(p,
                                    a,
                                    vy(1),
                                    vx(1),
                                    e_perfil,
                                    h,
                                    h_ranhura,
                                    i0,
                                    i1,
                                    t0,
                                    t1,
                                    space_between,
                                    n_perfis_x,
                                    n_perfis_y)]
  end

#=
paineis(p, a, h, i0, i1, t0, tf, n_perfis_x, n_perfis_y, n_paineis_x, n_paineis_y) =
    let folga = 0.005
        (x_div, y_div) = (division(i0, i1, n_paineis_x, true), division(t0, tf, n_paineis_y, true))
        map((x0, x1) -> map((y0, y1) -> painel(p, a, h, h, x0 + folga, x1 - folga, y0 + folga, y1 - folga, n_perfis_x, n_perfis_y), y_div[1:end-1], y_div[2:end]),
            x_div[1:end-1],
            x_div[2:end])
    end
#paineis(u0(), (h_vb - h_va)*1//3, 0.13, 0, 3, 0, 5, 10, 10, 2, 5)
=#

paineis(p, a, h, xi, xf, yi, yf, space_between, n_perfis_x, n_perfis_y, n_paineis_x, n_paineis_y) =
    let folga = 0.005
        len_x = space_between * n_perfis_x
        len_y = space_between * n_perfis_y
        len_panels_x = len_x*n_paineis_x + folga*(n_paineis_x - 1)
        len_panels_y = len_y*n_paineis_y + folga*(n_paineis_y - 1)
        p_x = (xf-xi-len_panels_x)/2
        p_y = (yf-yi-len_panels_y)/2
        (x_div, y_div) = (division(xi, len_panels_x + folga, n_paineis_x, true), division(yi, len_panels_y+folga, n_paineis_y, true))
        if xf-xi < len_panels_x
            error("Panels size exceeds available space in x direction")
        elseif yf-yi < len_panels_y
            error("Panels size exceeds available space in y direction")
        else
            map((x0, x1) -> map((y0, y1) -> painel(p + vxy(p_x, p_y), a, h, h, x0, x1-folga, y0, y1-folga, space_between, n_perfis_x, n_perfis_y), y_div[1:end-1], y_div[2:end]),
                x_div[1:end-1],
                x_div[2:end])
        end
    end

#painel(u0(), 0.07, 0.13, 0.06, 0, 0.15*7, 0, 0.15*5, 0.15, 7, 5)
#painel(u0(), 0.07, 0.13, 0.06, 0, 0.15*10, 0, 0.15*10, 0.15, 10, 10)
#paineis(u0(), (h_vb - h_va)*1//3, 0.13, 0, 8.5, 0, 4, 0.15, 5, 3, 4, 2)

paineis_sala(p, a, h, space_between, n_perfis_x, n_perfis_y, n_paineis_x, n_paineis_y) =
  let
    #Altura da Intervenção:
    h_aux = h_va + a/2
    #Paineis
    [paineis(z(h_aux),
            a,
            h,
            0,
            l_ci1,
            0,
            va_y1,
            space_between,
            n_perfis_x,
            n_perfis_y,
            n_paineis_x,
            n_paineis_y)...,
    paineis(yz(va_y1 + l_va, h_aux),
            a,
            h,
            0,
            l_ci1,
            0,
            va_y2 - va_y1 - l_va,
            space_between,
            n_perfis_x,
            n_perfis_y,
            n_paineis_x,
            n_paineis_y)...,
    paineis(yz(va_y2 + l_va, h_aux),
            a,
            h,
            0,
            l_ci1,
            0,
            va_y3 - va_y2 - l_va,
            space_between,
            n_perfis_x,
            n_perfis_y,
            n_paineis_x,
            n_paineis_y)...,
    paineis(yz(va_y3 + l_va, h_aux),
            a,
            h,
            0,
            l_ci1,
            0,
            C - va_y3 - l_va,
            space_between,
            n_perfis_x,
            n_perfis_y,
            n_paineis_x,
            n_paineis_y)...]
  end

# GENERATE ROOM ________________________________________________________________

# Função que gera a sala inteira.
room(p) =
  begin
    whiteboard_wall(p, C, L, H)
    entrance_door_wall(p, C, L, H)
    door1(p + vx(dr1_loc))
    side_door_wall(p, C, L, H)
    door2(p + vxy(L, dr2_loc))
    windows_wall(p, C, L, H)
    window_c01(p + vyz(wnd1_loc, h_wnd_sill))
    window_c01(p + vyz(wnd2_loc, h_wnd_sill))
    window_c01(p + vyz(wnd3_loc, h_wnd_sill))
    window_c01(p + vyz(wnd4_loc, h_wnd_sill))
    floor(p, C, L)
    ceil(p, C, L, H)
    lights(p, C, L, H)
    caixa1(p, C, L, H)
    caixa2(p, C, L, H)
    vigas_altas(p, C, L, H)
    vigas_baixas(p, C, L, H)
    secretarias(p, C, L, H)
    board(p, C, L, H)
  end

ground() =
  with(current_material, get_material("Materials/Plaster/Plaster1")) do
    surface_grid([[xyz(-1000,1000,0), xyz(1000,1000,-.1)], [xyz(-1000,-1000,0), xyz(1000,-1000,0)]])
  end

function c01()
  #=@time=# room(u0())
  #=@time=# paineis_sala(u0(), a, 0.13, 0.15, 10, 10, 5, 2)
  #paineis_sala(u0(), a, 0.13, 0.15, 8, 7, 6, 3)
end

original = current_layer()
c01()
#set_view(xyz(7.79,0.66,1.68), xyz(7.31,1.54,1.70), 18.0) #door view
#set_view(xyz(7.79,0.66,1.10), xyz(7.31,1.54,1.20), 18.0) #door view_pep height
#set_view(xyz(9.06,5.65,8.86), xyz(8.46,5.82,8.08), 18.0) #top view
#set_view(xyz(L/2, C/2, 21), xyz(L/2, C/2,20), 18.0) #top view
#set_view(xyz(3.0,14.8,1.7), xyz(3.5,-3.3,2.0), 18.0) #from the board
#set_view(xyz(0.68,2.06,1.67), xyz(1.30,2.83,1.53), 18.0) #back left
set_view(xyz(L/2,0.01,1.6), xyz(L/2,2,1.7), 18.0) #middle back wall
#render_view("add_top_no_int")

#=
freq10 = create_layer("Freq10")
with(current_layer, freq10, frequency, 10) do
  c01() end

for i in 1:30
  switch_to_layer(original)
  Khepri.interrupt_processing(connection(unity))
  switch_to_layer(freq10)
  Khepri.interrupt_processing(connection(unity))
end=#

# LCVR user test _______________________________________________________________

######## remove and generate new attractors based on user's location
## Refazer lista de atractores em função da localização do utilizador
sort_nearest(p, lst) = sort!(lst, by = q -> distance(p, q))
remove_attractor(array, i) = deleteat!(array, i) #imperative
add_attractor(loc, array) = append!([loc], array) #imperative
#remove_clossest_attractor(sorted_array) = sorted_array[2:end] #functional
#add_attractor(loc, array) = [loc, array...] #functional
project_xy(p) = xyz(p.x, p.y, 0)

#count new iterations
let state = 0
    global counter() = (state += 1)
end

# Will accumulate new project variations in layers
new_layer_vec = []
fixed_radius = Parameter(4.1)
user_loc() = get_view()[1]

change_attractors(func, arg1, arg2)=
  let new_layer = create_layer("Option_$(counter())")
      append!(new_layer_vec, new_layer)
      attractors_loc(func(arg1, arg2)) #imperative
      attractors_radius([fixed_radius() for i in attractors_loc()])
      with(current_layer, new_layer) do; c01() end
      switch_to_layer(new_layer)
  end

c01_remove() =
  let sorted_attractors = sort_nearest(user_loc(), attractors_loc())
    change_attractors(remove_attractor, sorted_attractors, 1)
  end
c01_add() =  change_attractors(add_attractor, project_xy(user_loc()), attractors_loc())

# Available commands to use in VR
#c01_remove()
#c01_add()
see_layer(n) = n == 0 ? switch_to_layer(original) : switch_to_layer(new_layer_vec[n])

# Planificação _________________________________________________________________

# Objecto(s) para planificar
# paineis_finais = paineis_sala(u0(), 0.07, 0.13, 0.15, 10, 10, 5, 2)

#=
project_to_world(surf) =
    transform(union(surface_boundary(surf)), inverse_transformation(frame_at(surf, 0, 0)))

lasercutting = create_layer("Laser Cutting")

planify(ss, panel_width, panel_height, panel_sep) =
    with(current_layer, lasercutting) do
        let panel_idx = 0
            x = 0
            for s in ss
                s1 = project_to_world(s)
                s1 = rotate(s1, pi/2)
                bb = bounding_box(s1)
                s_width = bb[2].x - bb[1].x
                s_height = bb[2].y - bb[1].y
                if x + s_width > panel_width
                    panel_idx += 1
                    x = 0
                end
                move(s1, vxy(panel_idx*(panel_width + panel_sep) + x - bb[1].x, -bb[1].y))
                x += s_width
                #delete_shape(s)
            end
        end
    end


planify(vcat(vcat(vcat(paineis_finais...)...)...), 3, 1, 0.2)
=#

#=
map_division(0, C, 6) do x
    map_division(0, L, 4) do y
        ieslight(xyz(x,y,3.0), vz(-1), pi/6, pi/3)
    end
end
=#

#=
iesfile = joinpath(pwd(), "LEDtube.ies")
ieslight(iesfile, xyz(1,2,3))

iesfile = "C:/Users/aml/Dropbox/AML/Projects/KhepriJulia/LEDtube.ies"
map_division(0, C, 6) do x
    map_division(0, L, 4) do y
        ieslight(iesfile, xyz(x,y,3.0))
    end
end
=#

# Render________________________________________________________________________

#=
render_size(1920,1080)
render_size(960,540)
render_quality(1)
render_exposure(0)
for ev in -2:0.5:2
    with(render_exposure, ev) do
        render_view("IntervencaoC01-EV$(ev)")
    end
end
with(render_exposure, +1.5) do
    set_view(xyz(7.29,-0.35,1.6), xyz(5.1,10.3,2.0), 22)
    render_view("IntervencaoC01Porta")
end
rendering_with(
    width=960, height=540,
    exposure=+1.5,
    quality=-0.3) do
    set_view(xyz(7.29,-0.35,1.6), xyz(5.1,10.3,2.0), 22)
    render_view("IntervencaoC01DoorLowQuality")
end=#

# Films_________________________________________________________________________
#=
rendering_with(width=1920, height=1080, exposure=+1.5, quality=-0.4) do
    start_film("InterventionC01WalkBetter3")
    for t in division(0, 1, 80)
        let p = intermediate_loc(xyz(7.29,-0.35,1.6), xyz(1.0,14.5,1.6), t)
            set_view(p, xyz(5.5,9.0,2.5), 22)
            save_film_frame()
        end
    end
end
=#
#=
rendering_with(width=1920, height=1080, exposure=+1.5, quality=-0.4) do
    start_film("InterventionC01Test3200Unity")
    for t in division(0, 1, 10)
        let p = intermediate_loc(xyz(7.29,-0.35,1.6), xyz(1.0,14.5,1.6), t)
            set_view(p, xyz(5.5,9.0,2.5), 22)
            save_film_frame()
        end
    end
end
=#
#=
begin
    start_film("Waves2")
    for c in division(3,6,10)
        delete_all_shapes()
        with(attractors_radius, [c,c,c,c,c]) do
            room(u0())
            paineis_sala(u0(), (h_vb - h_va)*1//3, 0.13, 0.15, 10, 10, 5, 2)
        end
        set_view(xyz(3.0,14.8,1.7), xyz(3.5,-3.3,2.0), 30.0)
        #set_view(xyz(7.79,0.66,1.68), xyz(7.31,1.54,1.70), 18.0) #door view
        save_film_frame()
    end
end


rendering_with(width=1920, height=1080, exposure=+1.5, quality=-0.4) do
           start_film("InterventionC01Test3200Unity")
           for t in division(0, 1, 200)
               let p = intermediate_loc(xyz(7.29,-0.35,1.6), xyz(1.0,14.5,1.6), t)
                   set_view(p, xyz(5.5,9.0,2.5), 22)
                   save_film_frame()
               end
           end
       end
=#

#sun rotation x40 1 y87 z-16
