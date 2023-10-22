#include <micromouse/micromouse.h>

#include <pico/stdlib.h>
#include <stdio.h>

int main() {
  stdio_init_all();

  while (1) {
    puts("Hello World");
    sleep_ms(1000);
  }
}
