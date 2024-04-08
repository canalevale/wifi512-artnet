#include <stdio.h>
#include "WiFi_AP.h"

static const char * TAG_Wifi = "Wifi";

void wifi_init(void){

    ESP_ERROR_CHECK(esp_netif_init());// Inicializo el Stack TCP/IP
    ESP_ERROR_CHECK(esp_event_loop_create_default());// Creacion de eventos por defecto
    esp_netif_create_default_wifi_ap();//Set modo Wifi-AP

    // Evento para la conexion wifi
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    
    // Configuracion por defecto para Wifi-AP
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
   
    // Actualizo la configuracion para Wifi-AP
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        }
    };
    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG_Wifi, "Inicializacion WiFi-SoftAP Finalizada.");
    ESP_LOGI(TAG_Wifi, "SSID:%s password:%s channel:%d",ESP_WIFI_SSID, ESP_WIFI_PASS, ESP_WIFI_CHANNEL);

}

void wifi_event_handler(void* arg, esp_event_base_t event_base,
 int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG_Wifi, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
        
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG_Wifi, "station "MACSTR" leave, AID=%d",MAC2STR(event->mac), event->aid);
       
    }
}