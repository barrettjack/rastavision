// some notes re: .obj file parsing
// this .obj file loader will support, initially, a minimal subset of features
// supported by the file format. Specifically, it will parse lines of the form:
//
// o [object name]
// v [geometric vertex]
// vn [vertex normal]
// vt [texture coordinates]
// f [polygonal face element]
//
// Subject to the following limitations:
//
// v: vertices have no w coordinate and provide no colour
// see: https://en.wikipedia.org/wiki/Wavefront_.obj_file#Geometric_vertex
//
// vt: only u and v coordinates are provided (i.e. no w coordinate)
//
// f: face elements are triangles only
//
// QUESTIONS:
//
// 1. why are there more vt lines than v, vn?
// 2. and why are there more f lines than v, vt, vn?
// 3. how do faces work?
// 4. how should I represent and store this data in my program?
// 5. what data is needed for the rendering pipeline and what are the expectations
//    or norms regarding how it will be stored/represented?
//
// ANSWERS:
//
// 1. TBD
// 2. each vertex in a mesh can belong to more than one face of a triangular mesh
// 3. for our purposes, faces will be triangles only. a face line stores 3 indices
//    indicating the vertex index, texture index, and normal index for the three
//    vertices of a triangular face. preceding vertex, vertex normal, and texture
//    coordinate lines specify 1-indexed lists of such items. faces index into these
//    lists.
// 4.
//
// array of structs of the form:
// Vec3 for v
// Vec3 for n
//
// problem: there are more vt lines than v, vn. Why? Store this data separately?
//

#include "../include/obj_loader.hpp"

Mesh load_obj(const std::string& filename) {
    // do stuff.
}
