import os
import bpy
import math
import mathutils
import functools

def get_dynamic_vertex_data(obj):
    obj.data.calc_loop_triangles()
    vertex_uv_indices = {}
    uvs = list(map(lambda x: tuple(x.uv), obj.data.uv_layers[0].data))
    verts = list(map(lambda x: x.co, obj.data.vertices))
    v_indices = list(functools.reduce(lambda x,y: x+list(y.vertices), obj.data.loop_triangles, []))
    l_indices = list(functools.reduce(lambda x,y: x+list(y.loops), obj.data.loop_triangles, []))

    for v in verts:
        vertex_uv_indices[tuple(v)] = {}
    vertex_norms = {}
    idx2vert = []
    max_idx = 0
    vbo_order = []
    ebo_order = []
    for i, v_idx in enumerate(v_indices):
        smooth_all = 'smooth_all' in obj.data and obj.data['smooth_all'] == 1
        norm = obj.data.loop_triangles[i//3].normal # v_indices is generated from loop_triangles so this lines up
        v = tuple(verts[v_idx])
        uv = uvs[l_indices[i]]
        cur_idx = -1
        if uv in vertex_uv_indices[v]:
            joined_norms = False
            for j in range(len(vertex_norms[v])):
                avg_norm, n, cur_idx = vertex_norms[v][j]
                if smooth_all or avg_norm.normalized().dot(norm.normalized()) > 0.8:
                    # If we want to join all we just do this
                    vertex_norms[v][j] = ((avg_norm*n + norm) / (n+1), n+1, cur_idx)
                    joined_norms = True
                    idx2vert[cur_idx] = (v, vertex_norms[v][j][0], uv)
                    break
            if not joined_norms:
                cur_idx = max_idx
                vertex_uv_indices[v][uv].append(cur_idx)
                max_idx += 1
                vertex_norms[v].append((norm, 1, cur_idx)) # Store averaged normal, n, and index for each normal cluster
                idx2vert.append((v, norm, uv))
        else:
            cur_idx = max_idx
            vertex_uv_indices[v][uv] = [cur_idx]
            max_idx += 1
            vertex_norms[v] = [(norm, 1, cur_idx)]
            # vbo_order += list(v)
            # vbo_order += list(uv)
            idx2vert.append((v, norm, uv))
        ebo_order.append(cur_idx) # Build ebo here after determining the current index

    for i in range(max_idx):
        v, norm, uv = idx2vert[i]
        vbo_order += list(v)
        vbo_order += list(norm.normalized()) # Assume static objects have no rotation or scaling. Normals are location invariant
        vbo_order += list(uv)
        vbo_order += list([0, 0])  # empty lightmap UV

    return (obj.name, vbo_order, ebo_order, obj.data.materials[0].name.split('_')[0].upper())

dynamic_objects = [obj for obj in bpy.data.objects if 'dynamic' in obj.data]

for odata in map(get_dynamic_vertex_data, dynamic_objects):
    name, vbo, ebo, mat = odata
    with open(r'C:\Users\steve\Projects\ff72d\mesh\%s.txt' % name, 'w') as f:
        f.write('#include "graphics/materials.h"\n#include "map.h"\n\n')
        f.write('static const float s_%s_vertices[] = { ' % name)
        f.write(', '.join(map(lambda x: "%.3f" % x, vbo)))
        f.write(" };\nstatic const u32 s_%s_indices[] = { " % name)
        f.write(', '.join(map(lambda x: "%d" % x, ebo)))
        f.write(' };\n\n')
        f.write('const struct dynamic_object_data g_mesh_%s = {\n\t.vertices = s_%s_vertices,\n\t.indices = s_%s_indices,\n\t.n_vertices = %d,\n\t.n_indices = %d,\n\t.material = MAT_%s\n};\n' % (name, name, name, len(vbo), len(ebo), mat))
