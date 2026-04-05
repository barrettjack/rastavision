#include "../include/vertex_shader.hpp"
#include <cassert>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>

static void transform_normals(const Model& model, ScreenSpaceData& target) {
    glm::mat3 normal_transforming_matrix = glm::mat3(glm::transpose(glm::inverse(model.transform)));

    for (uint32_t i = 0; i < target.vertex_count; ++i) {

        const Vertex& vertex_in_model_space = model.mesh.vertices[i];
        glm::vec3 normal_pre_transformation (vertex_in_model_space.nx,
            vertex_in_model_space.ny,
            vertex_in_model_space.nz);

        glm::vec3 normal_post_transformation = normal_transforming_matrix * normal_pre_transformation;

        ScreenSpaceVertex& currentVertex = target.vertices[i];
        currentVertex.nx = normal_post_transformation.x;
        currentVertex.ny = normal_post_transformation.y;
        currentVertex.nz = normal_post_transformation.z;
    }
}

static void transform_positions(const Model& model, const Camera& camera, ScreenSpaceData& target, const DisplayInfo& display_info) {
    // Note: modelling transformation (model-to-world) is just model.transform

    // Camera transformation (world-to-camera; a specific canonical-to-frame transformation)
    // first, establishing an orthonormal, right-handed uvw basis about the camera's position and
    // gaze direction. then, applying a canonical-to-frame transformation to de-
    // scribe a point's position relative to the constructed frame

    // should consider assert(camera.gaze_direction is not zero vector) to avoid craziness
    glm::vec3 w = (-camera.gaze_direction) / glm::length(camera.gaze_direction);
    glm::vec3 view_up_cross_w = glm::cross(glm::vec3(0, 0, 1), w);
    glm::vec3 u = view_up_cross_w / glm::length(view_up_cross_w);
    glm::vec3 v = glm::cross(w, u);

    glm::mat4 world_to_camera = glm::inverse(glm::mat4(
        glm::vec4(u, 0.0f),
        glm::vec4(v, 0.0f),
        glm::vec4(w, 0.0f),
        glm::vec4(camera.position, 1.0f)
    ));


    // Projection transformation (camera-to-canonical-view-volume)
    // Orthographic only for now, i.e. no linear perspective
    glm::mat4 translate_volume_corner_to_origin(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(-camera.l, -camera.b, -camera.n, 1.0f)
    );
    glm::mat4 scale_axes(
        glm::vec4((2.0f / (camera.r - camera.l)), 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, (2.0f / (camera.t - camera.b)), 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, (2.0f / (camera.n - camera.f)), 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    glm::mat4 translate_volume_to_cvv_corner(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f)
    );
    glm::mat4 camera_to_cvv = translate_volume_to_cvv_corner * scale_axes * translate_volume_corner_to_origin;


    // Viewport transformation (canonical view volume to screen space)
    glm::mat4 cvv_to_screen_space(
        glm::vec4((display_info.nx / 2.0f), 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, (display_info.ny / 2.0f), 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(((display_info.nx - 1.0f) / 2.0f), ((display_info.ny - 1.0f) / 2.0f), 0.0f, 1.0f)
    );


    // The full sequence of transformations
    glm::mat4 model_to_screen_space = cvv_to_screen_space * camera_to_cvv * world_to_camera * model.transform;


    for (uint32_t i = 0; i < target.vertex_count; ++i) {
        const Vertex& vertex_in_model_space = model.mesh.vertices[i];
        glm::vec4 pos_in_model_space (vertex_in_model_space.px, vertex_in_model_space.py, vertex_in_model_space.pz, 1.0f);

        glm::vec4 pos_in_screen_space = model_to_screen_space * pos_in_model_space;
        ScreenSpaceVertex& current_vertex = target.vertices[i];
        current_vertex.px = pos_in_screen_space.x;
        current_vertex.py = pos_in_screen_space.y;
        current_vertex.pz = pos_in_screen_space.z;
    }
}

ScreenSpaceData apply_vertex_shader(const Model& model, const Camera& camera, const DisplayInfo& display_info) {
    ScreenSpaceData data = {
        .vertices = new ScreenSpaceVertex[model.mesh.vertex_count],
        .indices = model.mesh.indices,
        .vertex_count = model.mesh.vertex_count,
        .index_count = model.mesh.index_count
    };

    transform_normals(model, data);
    transform_positions(model, camera, data, display_info);

    return data;
}
