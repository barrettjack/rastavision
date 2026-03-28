#include "../include/obj_loader.hpp"
#include "../include/model.hpp"
#include <cstdint>
#include <iostream>

int main() {
   Model model{};
   // model.mesh.vertices = new Vertex[1000];
   // model.mesh.indices = new uint32_t[1000];
   load_obj("models/utah_teapot.obj", model.mesh);

   const Vertex* vertices = model.mesh.vertices;
   for (uint32_t i = 0; i < model.mesh.vertex_count; ++i) {
       std::cout << vertices[i].nx << vertices[i].ny << vertices[i].nz <<
                  vertices[i].px << vertices[i].py << vertices[i].pz <<
                  vertices[i].u << vertices[i].v << "\n";
   }

   // ...

   return 0;
}
