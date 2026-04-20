// some notes re: .obj file parsing
// this .obj file loader supports a minimal subset of features
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

#include "obj_loader.hpp"
#include "types_and_utils.hpp"
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
#include <filesystem>

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

static void insert_index_in_mesh(VertexBuffer& vertex_buffer, IndexBuffer& index_buffer, uint32_t idx) {
    index_buffer.buf[index_buffer.len++] = idx;
}

static uint32_t insert_vertex_in_mesh(VertexBuffer& vertex_buffer, IndexBuffer& index_buffer, const Vertex& v) {
    vertex_buffer.buf[vertex_buffer.len] = v;
    insert_index_in_mesh(vertex_buffer, index_buffer, vertex_buffer.len);
    return vertex_buffer.len++;
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

void load_obj(const std::string& filepath, VertexBuffer& vertex_buffer, IndexBuffer& index_buffer) {
    using glm::vec2, glm::vec3;
    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<vec2> texture_coords;
    std::vector<face_data_indices> faces;

    // We will process the lines contained in the specified file.
    // The in-memory representation of the file's data will mirror almost identically
    // the formatting of the .obj file to start. Further transformations will be applied
    // to obtain a more compact, more computation-friendly in-memory representation.
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Failed to open file at path: " << filepath << "\n";
        std::cerr << "Current working directory is: " << std::filesystem::current_path() << "\n";
        std::cerr << "Exiting\n";
        exit(1);
    }

    std::string line;
    // TODO(jack):
    // - offer a more complete .obj loader feature set..?
    // - proper error handling within prefix cases
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
            // for now, doing nothing
        } else {
            // line is either a comment or some unsupported line type
            // either way, for now, we do nothing
        }
    }

    // Now that we have an representation of the .obj file's contents in memory,
    // we can do a bit of processing to obtain something more desirable for a
    // renderer to work with. Specifically, we will identify all unique face vertices
    // and store their data (position, normal, texture coordinates) in a Vertex
    // structure. We will have an array of all such vertices, and a second array
    // that indexes into this array. Each 3 consecutive indices in the latter array
    // correspond to a face of the mesh.

    // TODO(jack): we could conceivably do something smarter than assume we need
    // 3*num_faces space in memory to store vertices and indices. But, I'm not sure
    // this matters all that much, and it is a safe upper bound to use.
    uint32_t num_faces = faces.size();
    vertex_buffer.buf = new Vertex[num_faces * 3];
    index_buffer.buf = new uint32_t[num_faces * 3];

    std::unordered_map<std::string, uint32_t> vertex_string_repr_to_indices;

    for (uint32_t i = 0; i < num_faces; ++i) {
        face_data_indices fdi = faces[i];
        id_and_indices vertex_1 = indices_to_id(fdi.v1, fdi.vt1, fdi.vn1);
        id_and_indices vertex_2 = indices_to_id(fdi.v2, fdi.vt2, fdi.vn2);
        id_and_indices vertex_3 = indices_to_id(fdi.v3, fdi.vt3, fdi.vn3);
        std::array<id_and_indices, 3> vs = {vertex_1, vertex_2, vertex_3};

        for (const auto& [id, indices]: vs) {
            if (vertex_string_repr_to_indices.count(id) == 0) {
                Vertex v = indices_to_vertex(indices, vertices, normals, texture_coords);
                uint32_t idx = insert_vertex_in_mesh(vertex_buffer, index_buffer, v);
                vertex_string_repr_to_indices[id] = idx;
            } else {
                uint32_t idx = vertex_string_repr_to_indices.at(id);
                insert_index_in_mesh(vertex_buffer, index_buffer, idx);
            }
        }
    }

    assert(((mesh.index_count / 3) == num_faces) && "Incorrect number of faces. Something is wrong.");
}
