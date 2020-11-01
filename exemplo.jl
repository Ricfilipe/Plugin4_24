using Khepri
backend(unreal)
Khepri.show_rpc(true)
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
