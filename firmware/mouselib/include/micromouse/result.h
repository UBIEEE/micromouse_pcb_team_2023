#ifndef __MICROMOUSE_RESULT_H__
#define __MICROMOUSE_RESULT_H__

enum result {
  RESULT_OK             = 0,
  RESULT_ERROR          = 1 << 0,
  RESULT_INVALID_ARG    = 1 << 1,
  RESULT_SPI_COMM_ERROR = 1 << 2,
};
typedef enum result result_t;

#ifndef NDEBUG
#define PRINT_ERROR_LOC(result) (void)printf("Error %#04X at %s:%d\n", (int)result, __func__, __LINE__)
#else
#define PRINT_ERROR_LOC(result) ((void)0)
#endif

#define RETURN_IF_ERROR(result)                                                                    \
  do {                                                                                             \
    if (RESULT_OK != result) {                                                                     \
      PRINT_ERROR_LOC(result);                                                                     \
      return result;                                                                               \
    }                                                                                              \
  } while (0)

#define GOTO_IF_ERROR(result, label)                                                               \
  do {                                                                                             \
    if (RESULT_OK != result) {                                                                     \
      PRINT_ERROR_LOC(result);                                                                     \
      goto label;                                                                                  \
    }                                                                                              \
  } while (0)

#endif // __MICROMOUSE_RESULT_H__

