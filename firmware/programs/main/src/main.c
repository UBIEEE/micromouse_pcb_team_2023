#include <micromouse/micromouse.h>
#include <micromouse/std.h>

int main(void) {
  if (stdio_init_all()) return 1;

  micromouse_main();
  return 0;
}

