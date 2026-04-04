#pragma once

#include "mesh.hpp"
#include <string>
#include <vector>
#include <glm/glm.hpp>

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

id_and_indices indices_to_id(uint32_t, uint32_t, uint32_t);
Vertex indices_to_vertex(index_triple, const std::vector<glm::vec3>&,
                         const std::vector<glm::vec3>&,
                         const std::vector<glm::vec2>&);
void insert_index_in_mesh(Mesh& mesh, uint32_t idx);
uint32_t insert_vertex_in_mesh(Mesh&, const Vertex&);
void process_face(std::vector<face_data_indices>&, std::istringstream&);
void load_obj(const std::string&, Mesh&);
