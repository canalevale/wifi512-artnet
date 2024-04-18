#include <stdio.h>
#include "WiFi.h"

static const char * TAG_Wifi = "Wifi";
esp_netif_t *netif;
static int s_retry_num = 0;

wifi_mode conexion;
esp_ip4_addr_t ip_STA;

void wifi_init(wifi_mode mode){
    if(mode==MODE_STA){
        wifi_init_sta();
    }else if (mode==MODE_AP){
        wifi_init_ap();
    }else{
        ESP_LOGE(TAG_Wifi, "Error al inicializar el Wifi.");
    }
}

void wifi_init_ap(void){

    ESP_ERROR_CHECK(esp_netif_init());// Inicializo el Stack TCP/IP
    ESP_ERROR_CHECK(esp_event_loop_create_default());// Creacion de evento por defecto
    netif=esp_netif_create_default_wifi_ap();//Set modo Wifi-AP

    // Evento para la conexion wifi
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi_AP_event_handler, NULL, NULL));
    
    // Configuracion por defecto para Wifi-AP
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
   
    // Actualizo la configuracion para Wifi-AP
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_WIFI_SSID,
            .ssid_len = strlen(AP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = AP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        }
    };
    if (strlen(AP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG_Wifi, "Inicializacion WiFi-SoftAP Finalizada.");
    ESP_LOGI(TAG_Wifi, "SSID:%s password:%s channel:%d",AP_WIFI_SSID, AP_WIFI_PASS, ESP_WIFI_CHANNEL);

}

void wifi_init_sta(void){

    
    //Inicializao Stack TCP/UDP
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi_STA_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,IP_EVENT_STA_GOT_IP, &Wifi_STA_event_handler,  NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = STA_WIFI_SSID,
            .password = STA_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .sae_h2e_identifier = "",
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG_Wifi, "Inicializacion WiFi-STA Finalizada.");
}

void Wifi_AP_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data){
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG_Wifi, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
        
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG_Wifi, "station "MACSTR" leave, AID=%d",MAC2STR(event->mac), event->aid);
       
    }
}

void Wifi_STA_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG_Wifi, "Reintentando conexion AP");
        } else {
            ESP_LOGI(TAG_Wifi, "Fallo la conexion con SSID:%s, Password:%s", STA_WIFI_SSID, STA_WIFI_PASS);
        }
        ESP_LOGI(TAG_Wifi,"Fallo la conexion con el AP");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_Wifi, "Conectado al AP SSID:%s , Password:%s",STA_WIFI_SSID, STA_WIFI_PASS);
        ESP_LOGI(TAG_Wifi, "Direccion IP:" IPSTR, IP2STR(&event->ip_info.ip));
        ip_STA=event->ip_info.ip;
        s_retry_num = 0;
        
    }
}

esp_ip4_addr_t get_ip(void){
    esp_ip4_addr_t ip;
    if(conexion==MODE_AP){
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(netif,&ip_info);
    ip=ip_info.ip;
    }
    if(conexion==MODE_STA){
        ip=ip_STA;
    }
    return ip;
}