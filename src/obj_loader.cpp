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
#include <fstream>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>

typedef struct {
    glm::uint32_t v1;
    glm::uint32_t v2;
    glm::uint32_t v3;
    glm::uint32_t vn1;
    glm::uint32_t vn2;
    glm::uint32_t vn3;
    glm::uint32_t vt1;
    glm::uint32_t vt2;
    glm::uint32_t vt3;
} face_data_indices;

void process_face(std::vector<face_data_indices>& faces, std::istringstream& ss) {
    face_data_indices face;
    std::string blob;
    char slash;

    glm::uint32_t vs[3], vts[3], vns[3];
    int i = 0;

    while (ss >> blob && i < 3) {
        std::istringstream bs(blob);
        bs >> vs[i] >> slash >> vts[i] >> slash >> vns[i];
        ++i;
    }

    face.v1 = vs[0]; face.v2 = vs[1]; face.v3 = vs[2];
    face.vt1 = vts[0]; face.vt2 = vts[1]; face.vt3 = vts[2];
    face.vn1 = vns[0]; face.vn2 = vns[1]; face.vn3 = vns[2];

    faces.push_back(face);
}

Mesh load_obj(const std::string& filename) {
    using glm::vec2, glm::vec3;
    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<vec2> texture_coords;
    std::vector<face_data_indices> faces;

    // Here, we process the lines contained in the specified file.
    // The representation of the file's data in memory mirrors almost identically
    // the formatting of the .obj file. This is a preliminary step; we will further
    // process the data before outputting a Mesh object.
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;

        ss >> prefix;

        if (prefix == "v") {
            vec3 v;
            ss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } else if (prefix == "vt") {
            vec2 vt;
            ss >> vt.x >> vt.y;
            texture_coords.push_back(vt);
        } else if (prefix == "vn") {
            vec3 vn;
            ss >> vn.x >> vn.y >> vn.z;
            normals.push_back(vn);
        } else if (prefix == "f") {
            process_face(faces, ss);
        } else if (prefix == "o") {
            std::cout << "Mesh objects have no data member for object names. "
                         "So, the line |" << line << "| is ignored.\n";
        } else {
            std::cout << "load_obj does not support processing of lines beginning "
                         "with " << prefix << ". This line has been ignored\n";
        }
    }

    // Now that we have an representation of the .obj file's contents in memory,
    // we can do a bit of processing to obtain something more desirable for a
    // renderer to work with. Specifically, we will identify all unique face vertices
    // and store their data (position, normal, texture coordinates) in a Vertex
    // structure. We will have an array of all such vertices, and a second array
    // that indexes into this array. Each 3 consecutive indices in the latter array
    // correspond to a face of the mesh.
}
