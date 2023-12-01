#include <micromouse/micromouse.h>

#include <pico/stdlib.h>
#include <stdio.h>

int main(void) {
  if (!stdio_init_all()) {
    (void)puts("stdio_init_all() failed");
    return -1;
  }

  while (1) {
    (void)puts("Hello World");
    sleep_ms(1000);
  }
}
