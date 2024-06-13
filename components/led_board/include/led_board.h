
#ifndef ___LED_BOARD_H__
#define ___LED_BOARD_H__

#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "NVS_ESP.h"
#define LED_BOARD 2
#define LED_UART 25
#define LED_WIFI 26
#define BOOT_PUSH 0

esp_err_t init_led(void);
esp_err_t toggle_led(gpio_num_t led);
esp_err_t set_led(gpio_num_t led,bool status);
void change_mode_handler(void* arg);
bool get_mode_change(void);

#endif //___LED_BOARD_H__