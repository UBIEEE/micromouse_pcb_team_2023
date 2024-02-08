#include <mouse_control/micromouse.hpp>

int main() {
  if (!pico::stdio_init_all()) return 1;
  MicroMouse::get().run();
  return 0;
}
