#ifndef __MICROMOUSE_DIAGNOSTICS_H__
#define __MICROMOUSE_DIAGNOSTICS_H__

#include <micromouse/std.h>

void diag_init(void);
void diag_deinit(void);

enum hardware_status {
  HARDWARE_STATUS_OK        = 0,
  HARDWARE_STATUS_SPI_ERROR = 1 << 0,
};
typedef enum hardware_status hardware_status_t;

//
// Reports a hardware error.
//
void diag_report_hardware(hardware_status_t status, const char* message, ...);

// Reports an invalid argument passed to a function.
#define DIAG_REPORT_INVALID_ARG()                                              \
  do {                                                                         \
    printf("[INVALID ARG] %s:%s:%d\n", __FILE__, __func__, __LINE__);          \
    exit(1);                                                                   \
  } while (0)

#endif // __MICROMOUSE_DIAGNOSTICS_H__

