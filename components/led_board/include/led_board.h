#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#define LED 2

esp_err_t init_led(void);
esp_err_t toggle_led(void);
