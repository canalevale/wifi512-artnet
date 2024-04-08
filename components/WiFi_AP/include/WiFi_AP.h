#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lwip/err.h"
#include "lwip/sys.h"

//Defines
#define ESP_WIFI_SSID   "SD4_ESP32_test"
#define ESP_WIFI_PASS   "sd4_1234"
#define ESP_WIFI_CHANNEL  1 
#define MAX_STA_CONN    2

//Funciones
void wifi_init(void);
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);