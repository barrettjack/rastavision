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
#include <cstdint>
#include <fstream>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <cassert>

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

    // Face indices into vertex, normal, and texture coordinate lists assume
    // 1-based indexing in .obj files. Here, we map indicies i to i-1 for 0-based
    // indexing to simplify later processing.
    for (uint32_t i = 0; i < 3; ++i) {
        --vs[i];
        --vts[i];
        --vns[i];
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
            std::cout << "Unsupported line with contents:\n\t" << line << "\nHas been ignored\n";
        }
    }

    // Now that we have an representation of the .obj file's contents in memory,
    // we can do a bit of processing to obtain something more desirable for a
    // renderer to work with. Specifically, we will identify all unique face vertices
    // and store their data (position, normal, texture coordinates) in a Vertex
    // structure. We will have an array of all such vertices, and a second array
    // that indexes into this array. Each 3 consecutive indices in the latter array
    // correspond to a face of the mesh.

    // TODO(jack): this is silly. We should do something smarter re: determining
    // the number of vertices and indices we'll ultimately need.
    Mesh mesh;
    uint32_t num_faces = faces.size();
    mesh.vertices = new Vertex[num_faces * 3];
    mesh.indices = new glm::uint32_t[num_faces * 3];

    // pseudocode:
    // for each Vertex that would be implied by the indices in our face array
    //     create vertices v1, v2, v3
    //     create indices i, j, k
    //     for each Vertex in mesh.vertices
    //          if we find a Vertex equal to vi anywhere in the array then don't do anything to mesh.vertices
    //          and the corresponding index goes at the back of mesh.indices
    //
    //          else the vertex vi is unique and new so: add it to the back of mesh.vertices and the corresponding index
    //          goes at the back of mesh.indices
    //
    //          for the above two conditions, take care to increment vertex_count and index_count accordingly
    //
    // a postcondition: should have index_count/3 == number_faces
    auto populate_vertex_data = [&vertices, &normals, &texture_coords]
                                (Vertex& v, const uint32_t v_idx, const uint32_t vn_idx, const uint32_t vt_idx) {
                                    glm::vec3 v_pos = vertices[v_idx];
                                    v.px = v_pos.x;
                                    v.py = v_pos.y;
                                    v.pz = v_pos.z;

                                    glm::vec3 v_normal = normals[vn_idx];
                                    v.nx = v_normal.x;
                                    v.ny = v_normal.y;
                                    v.nz = v_normal.z;

                                    glm::vec2 v_texture = texture_coords[vt_idx];
                                    v.u = v_texture.x;
                                    v.v = v_texture.y;
                                };

    auto add_to_mesh = [&mesh](const Vertex& vertex) {
        for (uint32_t i = 0; i < mesh.vertex_count; ++i) {
            if (mesh.vertices[i] == vertex) {
                // since we FOUND a copy of vertex we're trying to insert in the mesh,
                // we need only specify its index in the indices array.
                mesh.indices[mesh.index_count++] = i;
                return;
            }
        }

        // if we get here, then we did not find a vertex in the mesh identical
        // to the vertex we are trying to add. this means that we need to add it
        // to our mesh's vertex array and increment the sizes of the arrays accordingly
        uint32_t& mesh_vertices_ct = mesh.vertex_count;
        mesh.vertices[mesh_vertices_ct] = vertex;
        mesh.indices[mesh.index_count++] = mesh_vertices_ct++;
    };

    for (uint32_t i = 0; i < num_faces; ++i) {
        Vertex v1, v2, v3;
        face_data_indices fdi = faces[i];
        populate_vertex_data(v1, fdi.v1, fdi.vn1, fdi.vt1);
        populate_vertex_data(v2, fdi.v2, fdi.vn2, fdi.vt2);
        populate_vertex_data(v3, fdi.v3, fdi.vn3, fdi.vt3);
        add_to_mesh(v1);
        add_to_mesh(v2);
        add_to_mesh(v3);
    }

    assert(((mesh.index_count / 3) == num_faces) && "Incorrect number of faces. Something is wrong.");

    return mesh;
}
