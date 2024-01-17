#include <micromouse/std.h>

int main(void) {
  if (!stdio_init_all()) return 1;

  while (1) {
    (void)puts("Hello, world!");
    sleep_ms(1000);
  }

  return 0;
}

