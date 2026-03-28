#include "../include/obj_loader.hpp"
#include "../include/model.hpp"

int main() {
   Model model{};
   model.mesh.vertices = new Vertex[1000];
   model.mesh.indices = new uint32_t[1000];
   load_obj("models/utah_teapot.obj", model.mesh);

   // ...

   return 0;
}
