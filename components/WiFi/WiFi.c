#include <stdio.h>
#include "WiFi.h"
#include "led_board.h"
static const char * TAG_Wifi = "Wifi";
esp_netif_t *netif;

extern QueueHandle_t Reply;

wifi_mode conexion;
esp_ip4_addr_t ip_STA;
bool disconect=0,reset=1;
TimerHandle_t xTimer = NULL;
char ssid[32];
char pass[64];


void wifi_init(void){
    char mod[4];
    get_nvs_mode(mod);
    if(strcmp(mod,"STA")==0){
        wifi_init_sta();
        conexion=MODE_STA;
    }else if (strcmp(mod,"AP")==0){
        wifi_init_ap();
        conexion=MODE_AP;
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
   
    get_nvs_ssid_ap(ssid);
    get_nvs_pass_ap(pass);

    // Actualizo la configuracion para Wifi-AP
    wifi_config_t wifi_config = {
        .ap = {
            .ssid_len = strlen(ssid),
            .channel = ESP_WIFI_CHANNEL,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        }
    };
    memcpy(wifi_config.ap.ssid, ssid, strlen(ssid));
    memcpy(wifi_config.ap.password, pass, strlen(pass));
    if (strlen(pass) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG_Wifi, "Inicializacion WiFi-SoftAP Finalizada.");
    ESP_LOGI(TAG_Wifi, "SSID:%s password:%s channel:%d",ssid, pass, ESP_WIFI_CHANNEL);

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

    get_nvs_ssid_sta(ssid);
    get_nvs_pass_sta(pass);

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .sae_h2e_identifier = "",
        },
    };
    memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
    memcpy(wifi_config.sta.password, pass, strlen(pass));
    
    if (strlen(pass) == 0) {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG_Wifi, "Inicializacion WiFi-STA Finalizada.");
}

void Wifi_AP_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data){
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG_Wifi, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
        set_led(LED_WIFI,1);
        
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG_Wifi, "station "MACSTR" leave, AID=%d",MAC2STR(event->mac), event->aid);
        set_led(LED_WIFI,0);
    }
}

void Wifi_STA_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        set_led(LED_WIFI,0);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        set_led(LED_WIFI,0);
        ESP_LOGI(TAG_Wifi,"Fallo la conexion con el AP");
        ESP_LOGI(TAG_Wifi, "Reintentando Conectar...");

        if(!disconect&&!reset){ //Si no es el reset y aun no se marco la desconexion, marco y borro el timmer de RRSI
        disconect=1;
        xTimerDelete(xTimer, 0);
        }
        
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        reset=0;
        //Creo el TIMMER PARA RSSI
        xTimer = xTimerCreate("WifiSignal",TIMER_WIFISIGNAL,pdTRUE,(void *) 0,wifi_get_rssi);
        xTimerStart(xTimer, 0);
        //Resumo las tareas
        if(disconect){disconect=0;}
        ESP_LOGI(TAG_Wifi, "Conectado al AP SSID:%s , Password:%s",ssid, pass);
        ESP_LOGI(TAG_Wifi, "Direccion IP:" IPSTR, IP2STR(&event->ip_info.ip));
        ip_STA=event->ip_info.ip;
        set_led(LED_WIFI,1);         
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

//wifi_mode get_conexion_mode(void){return conexion;}

void wifi_get_rssi(TimerHandle_t xTimer){
    int rssi;
    esp_wifi_sta_get_rssi(&rssi);
    WifiSignal wifisignal;
    memcpy(wifisignal.id, "Art-ESP", sizeof(wifisignal.id));
    wifisignal.opcode = ART_WIFISIGNAL;
    wifisignal.protVerH = 0;
    wifisignal.protVerL = 14;
    wifisignal.subuni = get_universo();
    wifisignal.signal=rssi;
    if(!ifconecct()){ if (!xQueueSend(Reply,(uint8_t *) &wifisignal, pdMS_TO_TICKS(15))){
           ESP_LOGE(TAG_Wifi, "Error: No se pudo añadir ArtWifiSignal al Queue");
    }}
    //ESP_LOGI(TAG_Wifi, "RSSI: %d", wifisignal.signal);

}


bool ifconecct(void) {return disconect;} //Devuelve el estado de la conexion

