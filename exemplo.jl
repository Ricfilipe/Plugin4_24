using Khepri
backend(unreal)
box()
box(loc_from_o_phi(xyz(1,0,0),pi/4),1,2,3)
box(loc_from_o_vx_vy(xyz(1,0,0),vz(),vy()),1,2,3)
box(loc_from_o_vz(xyz(1,0,0),vxy(1,1)),1,2,3)

right_cuboid()
cylinder()
cylinder()
cylinder()
regular_pyramid()
irregular_pyramid_frustum()
regular_pyramid_frustum()
delete_all_shapes()
current_layer(0)
slab()
panel()
chair()

Khepri.show_rpc(true)
Khepri.step_rpc(true)
get_material("/Game/MS_CleanBrick/Surface/07_Red_Brick_Wall_2x2_M_thmlaejg/RedBrickWall.RedBrickWall")

start_film("1_Dolly-Forth")
render_size(1920, 1080)
set_view(xyz(-3.504779815673828,-76.60165405273438,51.335548400878906), xyz(-3.52087664604187,-75.75193786621094,50.80855178833008), 35.0)
save_film_frame()


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
space_city(x(0), 1.5, 0.1, 0.01, 2.0, 3)
