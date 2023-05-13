import bpy
from bpy import context
from bpy_extras.io_utils import axis_conversion

filepath = bpy.path.abspath("//") + "myscene.scene"
folderpath = bpy.path.abspath("//") + "meshes/"

# Deselect all objects
bpy.ops.object.select_all(action='DESELECT')

export_scale = 1.0

with open(filepath, 'w') as f:
    
    f.write("#MESH MODEL\n")
         
    for object in context.visible_objects:    
        if object.type != 'MESH':
            continue
        
        name = object.data.name + ".obj"

        # Store original mat        
        original_mat = object.matrix_world.copy()
        
        # Set identity to export in 0,0,0 and no rotations
        object.matrix_world.identity()
        
        # Export OBJ
        object.select_set(True)
        bpy.ops.export_scene.obj(filepath=folderpath+name, filter_glob="*.obj;*.mtl", use_selection=True, use_triangles=True, 
            global_scale=export_scale, axis_forward='Y', axis_up='Z')
        object.select_set(False)

        # Restore transform
        object.matrix_world = original_mat.copy()
                
        # Export object in scene file
        f.write("meshes/" + name + " ")

        round_val = 5

        # Export matrix data (Change Y and Z axis and scale translation by global scale)
        
        correction_matrix = axis_conversion(to_forward='-Z', to_up='Y',).to_4x4()
        
        original_mat = correction_matrix @ original_mat 

        f.write( str( round(original_mat[0][0], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[1][0], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[2][0], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[3][0], round_val) ) )
        f.write(",")
        
        f.write( str( round(original_mat[0][1], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[1][1], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[2][1], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[3][1], round_val) ) )
        f.write(",")
        
        f.write( str( round(original_mat[0][2], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[1][2], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[2][2], round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[3][2], round_val) ) )
        f.write(",")
        
        f.write( str( round(original_mat[0][3] * export_scale, round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[1][3] * export_scale, round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[2][3] * export_scale, round_val) ) )
        f.write(",")
        f.write( str( round(original_mat[3][3], round_val) ) )
        
        f.write("\n")