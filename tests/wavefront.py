import numpy as np
import pywavefront as pwf
from wavefront_cpp import wavefront_convert, get_bvh

def load_obj_file(path: str, precomp_normal = True, verbose = False):
    """
        Meshes (output) are of shape (N_faces, 3, 3)
        Normals should be of shape (N_faces, 3)
        This is the parser for only one onject
    """
    obj         = pwf.Wavefront(path, collect_faces = True)
    vertices    = np.float32(obj.vertices)
    faces       = []
    for mesh in obj.mesh_list:
        for face_idx in mesh.faces:
            faces.append(vertices[face_idx])
    return np.stack(faces, axis = 0).astype(np.float32)

if __name__ == "__main__":
    meshes = load_obj_file("cbox_largebox.obj")
    print(meshes, meshes.shape)
    zero_output = np.zeros_like(meshes)
    wavefront_convert(meshes, zero_output)
    print("Here")
    print(zero_output)

    bvh_list = get_bvh(16)
    for i in range(16):
        bvh = bvh_list[i]
        print(bvh.mini, bvh.maxi)