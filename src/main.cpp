#include "types_and_utils.hpp"
#include <glm/ext/vector_float3.hpp>
#define TIMING

#ifdef TIMING
#include <chrono>
#endif

#include "obj_loader.hpp"
#include "vertex_shader.hpp"
#include "rasterizer.hpp"
#include "fragment_shader.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <cstdint>
#include <glm/fwd.hpp>
#include <SDL3/SDL_surface.h>
#include <glm/geometric.hpp>
#include <iostream>

using namespace std::string_view_literals;

int main() {
    // Loading model into memory.
    VertexBuffer vertex_buffer {.buf = nullptr, .len = 0};
    IndexBuffer index_buffer {.buf = nullptr, .len = 0};
    load_obj("models/utah_teapot_mid_complex.obj", vertex_buffer, index_buffer);


    // Next, we setup a camera and define the planes bounding its view volume
    // This can be programatically updated on a per-frame basis in the future,
    // once some form of input-handling has been implemented. Specifically, one
    // might have the camera's position and gaze direction change programatically
    // in response to inputs.
    ViewVolume view_volume {
        .l = -5.0f,
        .r = 5.0f,
        .n = -0.1f,
        .f = -15.0f,
        .t = 5.0f,
        .b = -5.0f,
    };

    Camera camera {
        .position = glm::vec3(0, 0, 0),
        .gaze_direction = glm::normalize(glm::vec3(0, 1, -0.25)),
        .view_volume = view_volume
    };

    assert(IS_UNIT_LENGTH(camera.gaze_direction));
    assert(IS_VALID_VIEW_VOLUME(camera.view_volume));

    DisplayInfo display_info = {.nx = 1280, .ny = 720};

    ScreenSpaceData screen_space_data = {
        .vertices = new ScreenSpaceVertex[vertex_buffer.len],
        .vertex_count = vertex_buffer.len,
    };

    // In the future, this is a thing that might be programmatically updated,
    // like the camera's gaze direction and position.
    glm::vec4 col1 (1, 0, 0, 0);
    glm::vec4 col2 (0, 1, 0, 0);
    glm::vec4 col3 (0, 0, 1, 0);
    glm::vec4 col4 (0, 5, 0, 1);
    glm::mat4 transform = glm::mat4(
        col1, col2, col3, col4
    );

    #ifdef TIMING
    auto start = std::chrono::high_resolution_clock::now();
    #endif
    apply_vertex_shader(screen_space_data, vertex_buffer, index_buffer, camera, transform, display_info);


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
        z_buffer.z_buffer[i] = camera.view_volume.f;
    }

    rasterizer(fragments, z_buffer, screen_space_data, display_info, index_buffer);


    // Next, we send the fragments off to the fragment shader.
    // It will determine the colours to display at each pixel.
    // These colours are written to an RGBA (in this order) buffer of uint32_t.
    // i.e. the most significant 8 bits store the red value, and the least sign-
    // ificant 8 bits store the alpha value
    uint32_t* RGBA_values = new uint32_t[fragment_count];
    for (uint32_t i = 0; i < fragment_count; ++i) {
        RGBA_values[i] = 0;
    }

    // TODO(jack): Reorganize/redo approach to lighting and materials..?
    // How should a material be "bound" to a model?
    // How should a texture for a given model be bound to that model?
    // i.e. where or how is this data "grouped?"
    Light::init("./lighting/single-light.l"sv);
    Material::init();
    apply_fragment_shader(fragments, RGBA_values, display_info, camera);


    // Finally, the buffer is written to a .PNG file, leaning on the functionality
    // of glorious SDL3.
    SDL_Surface* display_buffer = SDL_CreateSurfaceFrom(display_info.nx, display_info.ny, SDL_PIXELFORMAT_RGBA8888, RGBA_values, display_info.nx * 4);
    if (!display_buffer) {
        std::cout << SDL_GetError() << "\n";
        SDL_ClearError();
    }
    if (!SDL_SavePNG(display_buffer, "tests/out.png")) {
        std::cout << SDL_GetError() << "\n";
        SDL_ClearError();
    }

    #ifdef TIMING
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << duration.count() << "ms\n";
    #endif

    return 0;
}
