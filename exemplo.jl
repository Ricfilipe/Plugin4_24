using Khepri
backend(unreal)
#backend(unity)
#Khepri.show_rpc(true)
Khepri.step_rpc(true)
box()
sphere()
right_cuboid()
cylinder()
regular_pyramid()
irregular_pyramid_frustum()
regular_pyramid_frustum()
delete_all_shapes()
current_layer(0)
slab()
subtract(box(),cylinder())
chair()


get_material("/Game/StarterContent/Materials/M_Wood_Walnut.M_Wood_Walnut")


box()
box()
box()
box()
box()
box()
box()

macro with_layer(name, expr)
  quote
    let  l =  1 #create_layer($name)
      # with(current_layer, l) do
        print($(rpad(name, 20)), "->")
        @time $expr
      # end
      #Khepri.canonicalize_layer(l, unity)
    end
  end
end

space_city(p, r, α, β, δ, n) =
  begin
    box(p-vxyz(r*α/2,r*α/2,r*α/2), r*α)
    if n == 0
      true
    else
      r2 = r*δ
      mpx = p-vx(r2)
      ppx = p+vx(r2)
      mpy = p-vy(r2)
      ppy = p+vy(r2)
      mpz = p-vz(r2)
      ppz = p+vz(r2)
      cylinder(mpx, r*β, ppx)
      cylinder(mpy, r*β, ppy)
      cylinder(mpz, r*β, ppz)
      space_city(mpx, r2, α, β, δ, n-1)
      space_city(ppx, r2, α, β, δ, n-1)
      space_city(mpy, r2, α, β, δ, n-1)
      space_city(ppy, r2, α, β, δ, n-1)
      space_city(mpz, r2, α, β, δ, n-1)
      space_city(ppz, r2, α, β, δ, n-1)
    end
  end


delete_all_shapes()
@with_layer "Exponential model n=1:" space_city(x(0), 1.5, 0.1, 0.01, 2.0, 4)

render_size(1920, 1080)
render_dir("E:\\Unreal\\TesteImages")
start_film("Isenberg_var")
set_view(xyz(-3.504779815673828,-76.60165405273438,51.335548400878906), xyz(-3.52087664604187,-75.75193786621094,50.80855178833008), 35.0)
save_film_frame()
save_film_frame()
set_view(xyz(36.171138763427734,-46.27556228637695,55.54283905029297), xyz(35.66852569580078,-45.717838287353516,54.882293701171875), 35.0)

save_film_frame()


set_view(xyz(-276.38482422,63.3264209,28.8424585), xyz(-256.11056641,58.74675293,28.22946533), 35.0) #profile View
set_view(xyz(89.01051758,162.17709961,462.73421875), xyz(86.47416992,157.81921875,450.25464844), 35.0) #TOP view FOV 35

set_view(xyz(-30.96139648,-30.06761963,27.84841797), xyz(-26.71310791,-32.54011719,27.55648438), 14.46) #Library VIEW FOV 70
set_view(xyz(37.4464209,-37.27714844,39.55707031), xyz(32.97944824,-39.19171387,38.34541748), 14.46) #inncer VIEW FOV 70
set_view(xyz(39.70828369,-10.99890869,10.25535278), xyz(37.61859863,-9.98874451,9.65082275), 14.46) #inncer to Patio VIEW FOV 70
set_view(xyz(-27.34956543,2.94103088,34.03177979), xyz(-25.03907959,-1.88117615,32.27385742), 14.46) #stairs to Patio VIEW FOV 70
