#include "../include/obj_loader.hpp"
#include "../include/model.hpp"
#include "../include/vertex_shader.hpp"
#include "../include/camera.hpp"
#include "../include/rasterizer.hpp"
#include <cstdint>
#include <glm/fwd.hpp>

int main() {
    // Loading model into memory
    // For now, placing model at the point (0, 1, 0) unscaled.
    // Assuming glm takes matrices in column-major order, per Claude...
    // In the future, this is a thing that might be programmatically updated
    // For instance, rotating a model about the z-axis would involve constructing
    // a new model transform matrix each frame or physics update.
    glm::vec4 col1 (1, 0, 0, 0);
    glm::vec4 col2 (0, 1, 0, 0);
    glm::vec4 col3 (0, 0, 1, 0);
    glm::vec4 col4 (0, 1, 0, 1);
    glm::mat4 transform = glm::mat4(
        col1, col2, col3, col4
    );

    Model model {
        .mesh = {},
        .transform = transform
    };

    load_obj("models/utah_teapot.obj", model.mesh);


    // Next, we setup a camera and define the planes bounding its view volume
    // Like the model's transform, this too can be programatically updated on a
    // per-frame basis in the future, once some form of input-handling has been
    // implemented.
    //
    // This camera, coupled with display info and the model's data are used by
    // the vertex shader.
    glm::vec3 position (0, 0, 0);
    glm::vec3 gaze_direction (0, 1, 0);
    Camera camera {
        .position = position,
        .gaze_direction = gaze_direction,
        .l = -1.0f,
        .r = 1.0f,
        .n = -0.1f,
        .f = -2.0f,
        .t = 1.0f,
        .b = -1.0f,
    };

    DisplayInfo display_info = {.nx = 1280, .ny = 720};
    ScreenSpaceData vertex_shader_outputs = apply_vertex_shader(model, camera, display_info);


    // Next, data is sent off to the rasterizer for fragment coverage determination
    // and barycentric interpolation of vertex attributes.
    uint32_t fragment_count = display_info.nx * display_info.ny;
    FragmentData fragments {
        .fragments = new Fragment[fragment_count],
        .fragment_count = fragment_count,
    };

    z_buffer z_buffer = {
        .z_buffer = new float[fragment_count],
        .buffer_length = fragment_count
    };

    for (uint32_t i = 0; i < z_buffer.buffer_length; ++i) {
        z_buffer.z_buffer[i] = camera.f;
    }

    rasterizer(vertex_shader_outputs, fragments, z_buffer, display_info);


    return 0;
}
