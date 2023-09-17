import os
import bpy
import math
import mathutils
import functools

MAPNAME = 'undefined'

def get_vertex_data(obj):
    obj.data.calc_loop_triangles()
    vertex_uv_indices = {}
    uvs = list(map(lambda x: tuple(x.uv), obj.data.uv_layers[0].data))
    lightmap_uvs = [(0,0) for i in range(len(uvs))]
    if len(obj.data.uv_layers) > 1:
        lightmap_uvs = list(map(lambda x: tuple(x.uv), obj.data.uv_layers[1].data))
    verts = list(map(lambda x: obj.matrix_world @ x.co, obj.data.vertices))
    v_indices = list(functools.reduce(lambda x,y: x+list(y.vertices), obj.data.loop_triangles, []))
    l_indices = list(functools.reduce(lambda x,y: x+list(y.loops), obj.data.loop_triangles, []))

    for v in verts:
        vertex_uv_indices[tuple(v)] = {}
    vertex_norms = {}
    idx2vert = []
    # print("Vertices")
    # print(verts)
    # print("UVs")
    # print(uvs)
    max_idx = 0
    # print([(verts[v_indices[i]], uvs[l_indices[i]])for i in range(len(v_indices))])
    vbo_order = []
    ebo_order = []
    for i, v_idx in enumerate(v_indices):
        smooth_all = 'smooth_all' in obj.data and obj.data['smooth_all'] == 1
        norm = obj.data.loop_triangles[i//3].normal # v_indices is generated from loop_triangles so this lines up
        v = tuple(verts[v_idx])
        uv = uvs[l_indices[i]]
        lightmap_uv = lightmap_uvs[l_indices[i]]
        cur_idx = -1
        if uv in vertex_uv_indices[v]:
            joined_norms = False
            for j in range(len(vertex_norms[v])):
                avg_norm, n, cur_idx = vertex_norms[v][j]
                if smooth_all or avg_norm.normalized().dot(norm.normalized()) > 0.8:
                    # If we want to join all we just do this
                    vertex_norms[v][j] = ((avg_norm*n + norm) / (n+1), n+1, cur_idx)
                    joined_norms = True
                    idx2vert[cur_idx] = (v, vertex_norms[v][j][0], uv, lightmap_uv)
                    break
            if not joined_norms:
                cur_idx = max_idx
                vertex_uv_indices[v][uv].append(cur_idx)
                max_idx += 1
                vertex_norms[v].append((norm, 1, cur_idx)) # Store averaged normal, n, and index for each normal cluster
                idx2vert.append((v, norm, uv, lightmap_uv))
        else:
            cur_idx = max_idx
            vertex_uv_indices[v][uv] = [cur_idx]
            max_idx += 1
            vertex_norms[v] = [(norm, 1, cur_idx)]
            # vbo_order += list(v)
            # vbo_order += list(uv)
            idx2vert.append((v, norm, uv, lightmap_uv))
        ebo_order.append(cur_idx) # Build ebo here after determining the current index
    # 2 pass, now that we have averaged all normals
    # vbo_order = []
    # ebo_order = []
    for i in range(max_idx):
        v, norm, uv, lightmap_uv = idx2vert[i]
        vbo_order += list(v)
        vbo_order += list(norm.normalized()) # Assume static objects have no rotation or scaling. Normals are location invariant
        vbo_order += list(uv)
        vbo_order += list(lightmap_uv)

    # for i, v_idx in enumerate(v_indices):
    #     v = tuple(verts[v_idx])
    #     uv = uvs[l_indices[i]]
    #     cur_idx = vertex_uv_indices[v][uv]
    #     # vbo_order += list(v)
    #     # vbo_order += list(norm.normalized())
    #     # vbo_order += list(uv)
    #     ebo_order.append(cur_idx)
    return (vbo_order, ebo_order)

def get_obj_data(obj):
    vbo, ebo = get_vertex_data(obj)
    # transform_matrix = functools.reduce(lambda x,y: x+list(y), obj.matrix_world, [])
    lightmap_index = obj.data['lightmap_index'] if 'lightmap_index' in obj.data else 0
    return (obj.name.replace('.', '').lower(), vbo, ebo, lightmap_index)

obj_map = {}
dynamic_objs = []
total_v, total_i = 0, 0
max_v_per_object, max_i_per_object = 0, 0
n_objs = 0
lights = []
spotlights = []
colliders = []
camera_volumes = []
ladders = []

MAX_LIGHT_POWER = 20

for obj in bpy.data.objects:
    if obj.type == 'MESH' and obj.data.materials[0].name in ('collider', 'trigger_collider'):
        trigger, collidertype = False, 'RECT'
        event = None
        if obj.data.materials[0].name == 'trigger_collider':
            trigger = True
            event = obj.data['event']
        if 'Plane' in obj.name:
            collidertype = 'PLANE'
        elif 'Sphere' in obj.name:
            collidertype = 'SPHERE'
        elif 'Cylinder' in obj.name:
            collidertype = 'CYLINDER'
        colliders.append({ 'type': collidertype, 'transform': obj.matrix_world.copy(), 'scale': obj.scale, 'trigger': trigger, 'event': event }) 
    elif obj.type == 'MESH' and obj.data.materials[0].name == 'camera_volume':
        camera_volumes.append({ 'transform': obj.matrix_world.copy(), 'scale': obj.scale, 'overrides': obj.data['overrides'] })
    elif obj.type == 'MESH' and obj.data.materials[0].name == 'ladder':
        ladders.append((obj.matrix_world @ mathutils.Vector((1,0,0,1)), obj.matrix_world @ mathutils.Vector((-1,0,0,1)), (obj.matrix_world @ mathutils.Vector((0,0,1,0))).normalized()))
    elif obj.type == 'MESH' and 'dynamic' not in obj.data:
        texture = obj.data.materials[0].name.split('_')[0].upper()
        if texture not in obj_map:
            obj_map[texture] = []
        obj_map[texture].append(get_obj_data(obj))
    elif obj.type == 'LIGHT' and obj.data.type == 'POINT':
        lights.append((obj.location, obj.data.color * obj.data.energy / MAX_LIGHT_POWER))
    elif obj.type == 'LIGHT' and obj.data.type == 'SPOT':
        spotlights.append({
          'location': obj.location,
          'transform': obj.matrix_world,
          'color': obj.data.color,
          'inner': obj.data.spot_size / 2,
          'outer': obj.data.spot_size / 2 * (1 + obj.data.spot_blend / 2)
        })


with open(r'C:\Users\steve\Projects\ff72d\maps\%s.txt' % MAPNAME, 'w') as f:
    f.write('#include "graphics/materials.h"\n#include "map.h"\n#include "map_scripts.h"\n\n')
    for tex in obj_map:
        for odata in obj_map[tex]:
            name, vbo, ebo, _ = odata
            f.write('static const float s_%s_vertices[] = { ' % name)
            f.write(', '.join(map(lambda x: "%.3f" % x, vbo)))
            f.write(" };\nstatic const u32 s_%s_indices[] = { " % name)
            f.write(', '.join(map(lambda x: "%d" % x, ebo)))
            f.write(' };\n')
            total_v += len(vbo)/8
            total_i += len(ebo)
            max_v_per_object = max(max_v_per_object, len(vbo)/8)
            max_i_per_object = max(max_i_per_object, len(ebo))
            n_objs += 1
    f.write('const struct map_static_object_data g_%s_static_objects[] = {\n' % MAPNAME)
    for tex in obj_map:
        for odata in obj_map[tex]:
            name, vbo, ebo, lightmap_index = odata
            f.write('\t{\n\t\t.vertices = s_%s_vertices,\n' % name)
            f.write('\t\t.indices = s_%s_indices,\n' % name)
            f.write('\t\t.n_vertices = %d,\n' % (len(vbo)/8))
            f.write('\t\t.n_indices = %d,\n' % len(ebo))
            f.write('\t\t.material = MAT_%s,\n' % tex)
            f.write('\t\t.lightmap = %d' % lightmap_index)
            f.write('\n\t},\n')
    f.write('};\n')
    f.write('const u32 g_%s_num_static_objects = %d;\n' % (MAPNAME, n_objs))

    f.write('const struct map_point_light_data g_%s_point_lights[] = {\n' % MAPNAME)
    for light in lights:
        f.write('\t{\n\t\t.location = { %s },\n' % ', '.join(map(lambda x: "%.3f" % x, light[0])))
        f.write('\t\t.color = { %s },\n\t},\n' % ', '.join(map(lambda x: "%.3f" % x, light[1])))
    f.write('};\nconst u8 g_%s_num_point_lights = %d;\n' % (MAPNAME, len(lights)))

    f.write('const struct map_spot_light_data g_%s_spot_lights[] = {\n' % MAPNAME)
    for spot in spotlights:
        loc, transform, color = spot['location'], spot['transform'], spot['color']
        forward = transform @ mathutils.Vector((0,0,-1,0))
        f.write('\t{\n\t\t.location = { %s },\n' % ', '.join(map(lambda x: "%.3f" % x, list(loc))))
        f.write('\t\t.direction = { %s },\n' % ', '.join(map(lambda x: "%.3f" % x, list(forward)[:3])))
        f.write('\t\t.color = { %s },\n' % ', '.join(map(lambda x: "%.3f" % x, list(color))))
        f.write('\t\t.inner = %.3f,\n\t\t.outer = %.3f\n\t},\n' % (math.cos(spot['inner']), math.cos(spot['outer'])))
    f.write('};\nconst u8 g_%s_num_spot_lights = %d;\n' % (MAPNAME, len(spotlights)))

    f.write('const struct map_collider_data g_%s_colliders[] = {\n' % MAPNAME)
    for colli in colliders:
        inv = colli['transform']
        scale = colli['scale']
        inv.invert()
        f.write('\t{\n\t\t.inverse_transform = { %s },\n' % ', '.join(map(lambda x: "%.4f" % x, functools.reduce(lambda x,y: x+list(y), inv, []))))
        f.write('\t\t.scale = { %s },\n' % ', '.join(map(lambda x: "%.3f" % x, list(scale)[:3])))
        if colli['trigger'] and colli['event']:
            f.write('\t\t.trigger = true,\n')
            f.write('\t\t.script = g_%s,\n' % colli['event'])
        f.write('\t\t.type = COLLIDER_%s\n\t},\n' % colli['type'])
    f.write('};\nconst u8 g_%s_num_colliders = %d;\n' % (MAPNAME, len(colliders)))

    f.write('const struct map_camera_volume_data g_%s_camera_volumes[] = {\n' % MAPNAME)
    for volume in camera_volumes:
        inv = volume['transform']
        scale = volume['scale']
        inv.invert()
        f.write('\t{\n\t\t.inverse_transform = { %s },\n' % ', '.join(map(lambda x: "%.4f" % x, functools.reduce(lambda x,y: x+list(y), inv, []))))
        f.write('\t\t.scale = { %s },\n' % ', '.join(map(lambda x: "%.3f" % x, list(scale)[:3])))
        f.write('\t\t.overrides = %s\n\t},\n' % volume['overrides'])
    f.write('};\nconst u8 g_%s_num_camera_volumes = %d;\n\n' % (MAPNAME, len(camera_volumes)))

    f.write('const struct map_ladder_data g_%s_ladders[] = {\n' % MAPNAME)
    for i, ladder in enumerate(ladders):
        upper, lower, normal = ladder
        f.write('\t{\n\t\t.upper = { %s },\n' % ', '.join(map(lambda x: "%.3f" % x, list(upper)[:3])))
        f.write('\t\t.lower = { %s },\n' % ', '.join(map(lambda x: "%.3f" % x, list(lower)[:3])))
        f.write('\t\t.normal = { %s },\n\t\t.type = LADDER_VERTICAL,\n' % ', '.join(map(lambda x: "%.3f" % x, list(normal)[:3])))
        f.write('\t\t.exit_top_script = g_%s_script_ladder%d_exit_top,\n\t\t.exit_bottom_script = g_%s_script_ladder%d_exit_bottom,\n\t},\n' % (MAPNAME, i+1, MAPNAME, i+1))
    f.write('};\nconst u8 g_%s_num_ladders = %d;\n' % (MAPNAME, len(ladders)))


print("Total vertices %d" % total_v)
print("Total indices %d" % total_i)
print("Max no. vertices per object %d" % max_v_per_object)
print("Max no. of indices per object %d" % max_i_per_object)
print("Avg. no. of vertices per object %d" % (total_v / n_objs))
