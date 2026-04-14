#include "../include/obj_loader.hpp"
#include "../include/model.hpp"
#include "../include/vertex_shader.hpp"
#include "../include/camera.hpp"
#include "../include/rasterizer.hpp"
#include <cstdint>
#include <glm/fwd.hpp>

int main() {

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

    // For now, placing model at the point (0, 1, 0) unscaled.
    // Assuming glm takes matrices in column-major order, per Claude...
    // In the future, this is a thing that might be programmatically updated
    // For instance, rotating a model about the z-axis would involve constructing
    // a new model transform matrix each frame or physics update.
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

    uint32_t fragment_count = display_info.nx * display_info.ny;
    FragmentData fragments {
        .fragments = new Fragment[fragment_count],
        .fragment_count = fragment_count,
    };
    rasterizer(vertex_shader_outputs, fragments);


    return 0;
}
