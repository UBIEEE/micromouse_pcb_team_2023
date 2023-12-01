#include <micromouse/micromouse.h>

#include <micromouse/drivetrain.h>
#include <micromouse/hardware/icm20602.h>
#include <micromouse/io.h>

int main(void) {
  if (!stdio_init_all()) {
    (void)puts("stdio_init_all() failed");
    return -1;
  }

  return 0;
}

