include("5_AAV.jl")

# GENERATE _____________________________________________________________________

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

# fast_unity()

astana() =
    #with(default_beam_family, astana_beam_family) do
    begin
        delete_all_shapes()
        #@with_layer "Mid Block Slabs" mid_block_slabs(astana_center_p)
        #@with_layer "Out Block Out Slabs" out_block_out_slabs(astana_center_p)
        #@with_layer "Out Blocks in Slabs" out_block_in_slabs(astana_center_p, 4pi, 0, 0)
        # @with_layer "Spiral Steps" spiral_steps(astana_center_p)       #ALL AROUND SPIRAL STAIRS
    #COLUMNS AND BEAMS
        #@with_layer "Out Slab Beams" out_slab_beams(astana_center_p)
        #@with_layer "Mid Columns" mid_columns(astana_center_p)
        #@with_layer "Cross Beams" cross_beams(astana_center_p)
        #@with_layer "Out Columns" out_columns(astana_center_p)
    #WALLS
        #@with_layer "Block Walls" book_walls(astana_center_p)
         #@with_layer "Cores" cores(astana_center_p)               #WALLS & DOORS & ELEVATORS & TOP SLABS
            # AutoCAD -- Error
        #@with_layer "Glass Walls" glass_walls(astana_center_p)   #CURTAIN WALL
    #STEEL FAÇADE
        #@with_layer "Frames" frames(astana_center_p)
        #@with_layer "Connections" connections(astana_center_p)
        #@with_layer "Connections" new_conections(astana_center_p)
        #@with_layer "Bays" bays(astana_center_p)
        #@with_layer "Secondary Structure" secondary_structure(astana_center_p)
    #MORPHS
        #@with_layer "Facade Glass" facade_glass(astana_center_p)      #FAÇADE GLASS PANELS
        #@with_layer "Photovoltaic panels" photovoltaic_panels(astana_center_p)
        #@with_layer "Raillings" raillings(astana_center_p)
    #OBJECTS
        #with(current_material, get_material("Default/Materials/Concrete")) do
         # @with_layer "Stair Cases" stair_cases(astana_center_p)
        #end
        #@with_layer "Book Shelves" book_shelves(astana_center_p)
        #@with_layer "Tables" tables(astana_center_p)
        #@with_layer "Lights" lights(astana_center_p)
        #@with_layer "People" place_people(astana_center_p)
        #@with_layer "Torches" torches(astana_center_p)
    #GROUND
        # ground()
    end

astana()
