using Khepri
backend(unreal)
delete_all_shapes()

### Plan: City of pagodas _______________________________________________

## one pagoda
column_offset = .3

function base_level_pagoda(sides, center, radius, height)
    #with(current_material, get_material("Default/Materials/Glass")) do
        #extrusion(regular_polygon(sides, center, radius), height)
        #regular_prism(sides, center, radius -column_offset, 0, height)
    #end
    with(current_material, get_material("Materials/wood/InteriorWood9")) do
            let alfa = 2pi/sides
                for i in 0:sides
                    cylinder(center +vpol(radius -column_offset, alfa*i), column_offset, height)
                end
            end
        end
    end

slab_level_pagoda(sides, center, radius_base, height)=
    with(current_material, get_material("Materials/wood/ExteriorWood3")) do
            regular_pyramid_frustum(sides, center, radius_base*1.2, 0, center+vz(height), radius_base)
            #loft([surface_regular_polygon(sides, center, radius_base*1.2), #radius telhado = 1.2*radius da base
            #     surface_regular_polygon(sides, center+vz(height), radius_base)])
        end

function level_pagoda(sides, center, radius, height)
    base_level_pagoda(sides, center, radius, height*2/3) #height base = 2/3 total height
    slab_level_pagoda(sides, center+vz(height*2/3), radius, height*1/3) #roof height = 1/3 total height
end

pagoda(stories, sides, center, radius_level_0, story_height, f_radius)=
    let f = f_radius/stories #f_radius = total reduction of the radius along the pagoda
        for i in 0:stories-1
            level_pagoda(sides,
                         center+vz(story_height*i),
                         radius_level_0 - f*i,
                         story_height)
        end
    end

## random pagoda
random_pagoda(center)=
    let stories = random_range(5, 15)
        sides = random_range(4, 10)
        radius = random_range(3, 10)
        story_height = random_range(3, 5)
        f_radius = radius/random_range(2, 3)
        pagoda(stories, sides, center, radius, story_height, f_radius)
    end

## pagoda city
pagoda_city(corner, streets, blocks)=
    for r in 0:streets-1
        for q in 0:blocks-1
            random_pagoda(corner + vxy(30*q, 30*r))
            #afastamento de 20(diametro máximo do pagoda)+10(de margem) entre blocks
        end
    end

function new_pagoda_city(p, x, y)
    delete_all_shapes()
    #with(current_material, get_material("Materials/color/Black")) do
    #with(current_material, get_material("Default/Materials/Grass")) do
    with(current_material, get_material("Materials/Plaster/Plaster1")) do
          #surface_grid([[xyz(-1000,1000,-10), xyz(1000,1000,-10)], [xyz(-1000,-1000,-10), xyz(1000,-1000,-10)]])
          surface_grid([[xyz(-9000,9000,0), xyz(9000,9000,0)], [xyz(-90000,-90000,0), xyz(9000,-9000,0)]])
        end
    pagoda_city(p, x, y)
end

### Generate ___________________________________________________________________

#new_pagoda_city(u0(), 3, 3)

# generate 3D text 1
# let p = u0()+vxyz(70, 0, 0)
#     rotate(scale(
#         text("pagoda_city(u0(), 3, 3)
#
#     pagoda_city(corner, streets, blocks)=
#             for r in 0:streets-1
#                 for q in 0:blocks-1
#                     random_pagoda(corner + vxy(30*q, 30*r))
#                 end
#             end", loc_from_o_vx_vy(p, vx(), vz()), 6),
#                 3, p),
#             -pi/3, p)
# end

# generate 3D text 2
# let p = u0()+vxyz(80, 0, 0)
#     rotate(scale(
#         text("
#         pagoda(stories, sides, center,
#                radius_level_0, story_height, f_radius)=
#                 let f = f_radius/stories
#                     for i in 0:stories-1
#                         level_pagoda(sides,
#                                      center+vz(story_height*i),
#                                      radius_level_0 - f*i,
#                                      story_height)
#                     end
#                 end"
#                 , loc_from_o_vx_vy(p, vx(), vz()), 6),
#                     3, p),
#                 -pi/3, p)
#     end

### Render _____________________________________________________________________

set_view(xyz(-27.890012741088867,-108.28427124023438,22.771474838256836),
        xyz(-27.377965927124023,-107.4253158569336,22.77420425415039),
        35.0)
render_dir("C:\\Users\\renata\\Google Drive\\PAPERS_TODO\\2019_eCAADe_LCVR")
save_film_frame(true)

view_from_groud_3()=
    set_view(xyz(-18.684776306152344,-68.34544372558594,1.0799999237060547),
            xyz(-18.254695892333984,-67.47134399414062,1.3057781457901),
            35.0)
view_from_above_3()=
    set_view(xyz(-67.46150207519531,-102.95145416259766,87.28515625),
            xyz(-66.92333984375,-102.20315551757812,86.89730072021484),
            35.0)
view_from_groud_10()=
    set_view(xyz(-13.750252723693848,-51.557281494140625,3.0199055671691895),
            xyz(-13.187567710876465,-50.752281188964844,3.207946300506592),
            35.0)
view_from_above_10()=
    set_view(xyz(-62.84168243408203,-211.86817932128906,175.76441955566406),
            xyz(-62.34902572631836,-211.1175994873047,175.3240509033203),
            35.0)

make_gif(p, x, y, n_frames)=
    for i in 0:n_frames
        new_pagoda_city(p, x, y)
        view_from_groud_10()
        save_film_frame()
        view_from_above_10()
        save_film_frame()
    end

start_film("pagoda_city_gif_10")
make_gif(u0(), 10, 10, 20)


### "Planeamento: Random Forest" _________________________________________________

## árvore random - Exercício 4.4.1 da sebenta
arvore(p_base, r_base, c_ramo, fi, psi, min_f_c, max_f_c, max_fi, min_psi, max_psi) =
    let p_slab = p_base + vsph(c_ramo, fi, psi)
        r_slab = r_base/2
        #ramo() = cone_frustum(p_base, r_base, p_slab, r_slab)
        #folha() = sphere(p_slab, 0.2)
        cone_frustum(p_base, r_base, p_slab, r_slab) # ramo
        if c_ramo < 2 || r_slab < 0.01
            nothing #folha()
        else
            arvore(p_slab, r_slab,
                   c_ramo*random_range(min_f_c, max_f_c),
                   fi + random(max_fi),
                   psi + random_range(min_psi, max_psi),
                   min_f_c, max_f_c, max_fi, min_psi, max_psi)
            arvore(p_slab, r_slab,
                   c_ramo*random_range(min_f_c, max_f_c),
                   fi - random(max_fi),
                   psi - random_range(min_psi, max_psi),
                   min_f_c, max_f_c, max_fi, min_psi, max_psi)
        end
    end

arvore_random(p)=
    let r_base = random_range(0.5, 1)
        c_ramo = random_range(10.0, 20.0)
        fi = 0
        psi = 0
        min_f_c = 0.6
        max_f_c = 0.9
        max_fi = pi
        min_psi = pi/16
        max_psi = pi/2
        arvore(p, r_base, c_ramo, fi, psi, min_f_c, max_f_c, max_fi, min_psi, max_psi)
    end

## floresta random
floresta(p, l, c, n)=
    for i in 0:n
        arvore_random(p+vxy(random(c), random(l)))
    end

floresta(p, l, c, n)=
    if n==0
        nothing
    else
        arvore_random(p+vxy(random(c), random(l)))
        floresta(p, l, c, n-1)
    end

"test" #floresta(u0(), 100, 200, 20)
