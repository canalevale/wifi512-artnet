
#ifndef ___WIFI_H__
#define ___WIFI_H__

#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lwip/err.h"
#include "lwip/sys.h"

//////Defines//////
//Defines AP-MODE
#define AP_WIFI_SSID   "SD4_ESP32_test"
#define AP_WIFI_PASS   "sd4_1234"
#define ESP_WIFI_CHANNEL  1 
#define MAX_STA_CONN    2
//Defines STA-MODE
#define STA_WIFI_SSID      "Fibertel WiFi CE2 2.4GHz"
#define STA_WIFI_PASS      "mUTe7gRw2Z"
#define ESP_MAXIMUM_RETRY  4

typedef enum {
    MODE_STA = 0,       /**< WiFi STA-MODE */
    MODE_AP            /**< WiFi AP-MODE */
} wifi_mode;

//Funciones
void wifi_init_ap(void);
void wifi_init_sta(void);
void wifi_init(wifi_mode mode);
void Wifi_STA_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
void Wifi_AP_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
esp_ip4_addr_t get_ip(void);

#endif //___WIFI_H__