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
#include <array>
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
#include <unordered_map>

typedef struct {
    uint32_t v1;
    uint32_t v2;
    uint32_t v3;
    uint32_t vn1;
    uint32_t vn2;
    uint32_t vn3;
    uint32_t vt1;
    uint32_t vt2;
    uint32_t vt3;
} face_data_indices;

typedef struct {
    uint32_t vertex_idx;
    uint32_t texture_idx;
    uint32_t normal_idx;
} index_triple;

typedef struct {
    std::string id;
    index_triple indices;
} id_and_indices;

static id_and_indices indices_to_id(uint32_t vertex_idx, uint32_t texture_idx, uint32_t normal_idx) {
    return {
        .id = std::to_string(vertex_idx) + "/" + std::to_string(texture_idx) + "/" + std::to_string(normal_idx),
        .indices = {vertex_idx, texture_idx, normal_idx}
    };
}

static Vertex indices_to_vertex(index_triple indices, const std::vector<glm::vec3>& vertices,
                         const std::vector<glm::vec3>& normals,
                         const std::vector<glm::vec2>& texture_coords) {
    glm::vec3 pos = vertices[indices.vertex_idx];
    glm::vec3 normal = normals[indices.normal_idx];
    glm::vec2 texture_coord = texture_coords[indices.texture_idx];

    return Vertex {
        .px = pos.x,
        .py = pos.y,
        .pz = pos.z,
        .nx = normal.x,
        .ny = normal.y,
        .nz = normal.z,
        .u = texture_coord.x,
        .v = texture_coord.y
    };
}

static void insert_index_in_mesh(Mesh& mesh, uint32_t idx) {
    mesh.indices[mesh.index_count++] = idx;
}

static uint32_t insert_vertex_in_mesh(Mesh& mesh, const Vertex& v) {
    mesh.vertices[mesh.vertex_count] = v;
    insert_index_in_mesh(mesh, mesh.vertex_count);
    return mesh.vertex_count++;
}

static void process_face(std::vector<face_data_indices>& faces, std::istringstream& ss) {
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

    face_data_indices face {
        .v1 = vs[0],
        .v2 = vs[1],
        .v3 = vs[2],
        .vn1 = vns[0],
        .vn2 = vns[1],
        .vn3 = vns[2],
        .vt1 = vts[0],
        .vt2 = vts[1],
        .vt3 = vts[2]
    };

    faces.push_back(face);
}

void load_obj(const std::string& filename, Mesh& mesh) {
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
    uint32_t num_faces = faces.size();
    mesh.vertices = new Vertex[num_faces * 3];
    mesh.indices = new uint32_t[num_faces * 3];

    std::unordered_map<std::string, uint32_t> vertex_string_repr_to_indices;

    // pseudocode:
    // for face in faces
    //     get Vertex data for the three Vertices and represent as strings
    //     i.e. data from line "f 1/1/1 2/2/2 3/3/3" becomes "1/1/1", "2/2/2", "3/3/3" for use as keys in map
    //
    //     for each string:
    //         if the string is not in the map, then
    //              construct vertex from indices (a function for this would be good)
    //              insert vertex into mesh.vertices and and add index to mesh.indices. additionally, insert indices into map
    //         else it is in the map, so get index from map and add index to mesh.indices
    //
    for (uint32_t i = 0; i < num_faces; ++i) {
        face_data_indices fdi = faces[i];
        id_and_indices vertex_1 = indices_to_id(fdi.v1, fdi.vt1, fdi.vn1);
        id_and_indices vertex_2 = indices_to_id(fdi.v2, fdi.vt2, fdi.vn2);
        id_and_indices vertex_3 = indices_to_id(fdi.v3, fdi.vt3, fdi.vn3);
        std::array<id_and_indices, 3> vs = {vertex_1, vertex_2, vertex_3};

        for (const auto& [id, indices]: vs) {
            if (vertex_string_repr_to_indices.count(id) == 0) {
                Vertex v = indices_to_vertex(indices, vertices, normals, texture_coords);
                uint32_t idx = insert_vertex_in_mesh(mesh, v);
                vertex_string_repr_to_indices[id] = idx;
            } else {
                uint32_t idx = vertex_string_repr_to_indices.at(id);
                insert_index_in_mesh(mesh, idx);
            }
        }
    }

    assert(((mesh.index_count / 3) == num_faces) && "Incorrect number of faces. Something is wrong.");
}
