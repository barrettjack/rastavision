#include "../include/obj_loader.hpp"
#include "../include/model.hpp"

int main() {
   Mesh mesh = load_obj("models/utah_teapot.obj");
   Model model = {mesh};

   // ...

   return 0;
}
