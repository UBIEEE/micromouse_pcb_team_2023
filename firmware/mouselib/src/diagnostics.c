#include <micromouse/diagnostics.h>

#include <micromouse/io.h>

static const char* hardware_status_str_lookup[] = {
    [HARDWARE_STATUS_OK]        = "OK",
    [HARDWARE_STATUS_SPI_ERROR] = "SPI ERROR",
};

enum status {
  STATUS_OK             = 0,
  STATUS_HARDWARE_ERROR = 1 << 0,
};
typedef enum status status_t;

static status_t global_status = STATUS_OK;

static void leds_init(void);
static void leds_deinit(void);
static void leds_update_status(void);

void diag_init(void) {
  global_status = STATUS_OK;
  leds_init();
}

void diag_deinit(void) { leds_deinit(); }

void diag_report_hardware(hardware_status_t s, const char* msg, ...) {
  global_status |= STATUS_HARDWARE_ERROR;
  if (s == HARDWARE_STATUS_OK) return;

  // Update LEDs.
  leds_update_status();

  // Print message.
  (void)printf("[HARDWARE] %s", hardware_status_str_lookup[s]);

  if (msg != NULL) {
    printf(": ");

    va_list args;
    va_start(args, msg);
    (void)vprintf(msg, args);
    va_end(args);
  }

  (void)puts("");
}

#define LED_PINS_NUM 5

static const uint led_pins[LED_PINS_NUM] = {PIN_LED_1, PIN_LED_2, PIN_LED_3,
                                            PIN_LED_4, PIN_LED_5};

static void led_init(uint pin) {
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_OUT);
  gpio_put(pin, 0);
}

static void led_deinit(uint pin) {
  gpio_put(pin, 0);
  gpio_deinit(pin);
}

static void leds_init(void) {
  for (uint i = 0; i < LED_PINS_NUM; ++i) {
    led_init(led_pins[i]);
  }
}

static void leds_deinit(void) {
  for (uint i = 0; i < LED_PINS_NUM; ++i) {
    led_deinit(led_pins[i]);
  }
}

static void leds_update_status(void) {
  for (uint i = 0; i < LED_PINS_NUM; ++i) {
    gpio_put(led_pins[i], (bool)(global_status & (1 << i)));
  }
}
 
