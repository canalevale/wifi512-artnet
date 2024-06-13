
#ifndef ___WIFI_H__
#define ___WIFI_H__

#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "ArtNet.h"
#include "NVS_ESP.h"

//////Defines//////
//Defines AP-MODE
#define AP_WIFI_SSID   "SD4_ESP32_test"
#define AP_WIFI_PASS   "sd4_1234"
#define ESP_WIFI_CHANNEL  1 
#define MAX_STA_CONN    2
//Defines STA-MODE
#define STA_WIFI_SSID      "Fibertel WiFi CE2 2.4GHz" //"Laboratorio DSI"
#define STA_WIFI_PASS      "mUTe7gRw2Z"               //"alumnolab"

typedef enum {
    MODE_STA = 0,       /**< WiFi STA-MODE */
    MODE_AP            /**< WiFi AP-MODE */
} wifi_mode;

// Estructura para el paquete WifiSignal
typedef struct {
    uint8_t id[8];      // "Art-Net" + 0x00
    uint16_t opcode;    // Código de operación (OpWifiSignal = 0x6001)
    uint8_t protVerH;   // Versión de protocolo alta (siempre 0)
    uint8_t protVerL;   // Versión de protocolo baja (siempre 14)
    uint8_t subuni;     // Universo Art-Net
    int8_t signal;      // Nivel de señal
} WifiSignal;

//Funciones
//Inicializacion
void wifi_init_ap(void);
void wifi_init_sta(void);
void wifi_init(void);
//Devuelve el modo de conexion
//wifi_mode get_conexion_mode(void);
bool ifconecct(void);
void wifi_get_rssi(TimerHandle_t xTimer);
void Wifi_STA_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
void Wifi_AP_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
esp_ip4_addr_t get_ip(void);

#endif //___WIFI_H__