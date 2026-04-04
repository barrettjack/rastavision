#include "../include/obj_loader.hpp"
#include "../include/model.hpp"
#include "../include/vertex_shader.hpp"
#include "../include/camera.hpp"
#include <glm/fwd.hpp>

int main() {
   Model model{};
   load_obj("models/utah_teapot.obj", model.mesh);

   // For now, placing model at the point (0, 1, 0) unscaled.
   // Assuming glm takes matrices in column-major order, per Claude...
   // In the future, this is a thing that might be programmatically updated
   // For instance, rotating a model about the z-axis would involve constructing
   // a new model transform matrix each frame or physics update.
   glm::vec4 col1 (1, 0, 0, 0);
   glm::vec4 col2 (0, 1, 0, 0);
   glm::vec4 col3 (0, 0, 1, 0);
   glm::vec4 col4 (0, 1, 0, 1);
   model.transform = glm::mat4(
       col1, col2, col3, col4
   );

   glm::vec3 position (0, 0, 0);
   glm::vec3 gaze_direction (0, 1, 0);
   Camera camera {position, gaze_direction};
   camera.l = -1.0f;
   camera.r = 1.0f;
   camera.t = 1.0f;
   camera.b = -1.0f;
   camera.n = -0.1f;
   camera.f = -2.0f;

   DisplayInfo display_info = {1280, 720};

   // Remark: for the world coordinate system, I am assuming a right-handed
   // system with +z up.
   ScreenSpaceData vertex_shader_outputs = apply_vertex_shader(model, camera, display_info);

   // const Vertex* vertices = model.mesh.vertices;
   // for (uint32_t i = 0; i < model.mesh.vertex_count; ++i) {
   //     std::cout << vertices[i].nx << vertices[i].ny << vertices[i].nz <<
   //                vertices[i].px << vertices[i].py << vertices[i].pz <<
   //                vertices[i].u << vertices[i].v << "\n";
   // }

   return 0;
}
