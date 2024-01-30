#include <micromouse/picosdk.hpp>
#include <micromouse/std.hpp>

int main() {
  if (!pico::stdio_init_all()) return 1;

  while (true) {
    (void)puts("Hello, world!");
    pico::sleep_ms(1000);
  }

  return 0;
}

