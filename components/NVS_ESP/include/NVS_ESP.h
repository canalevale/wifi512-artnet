#ifndef ___NVS_ESP_H__
#define ___NVS_ESP_H__


#include <stdio.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "nvs.h"

//Funciones
//Carga todos los datos -> dado que al leer por primera vez los datos se desconoce el tamaño
void get_nvs_all(void);
//Devuelve modo de Wifi
void get_nvs_mode(char *mode);
//Cambia de modo
void change_mode_wifi(void);
//Opciones de configuracion para modo AP
uint8_t get_nvs_ap_subuni(void);
void get_nvs_ssid_ap(char *ssid);
void get_nvs_pass_ap(char *pass);
//Opciones de configuracion para modo STA
uint8_t get_nvs_sta_subuni(void);
void get_nvs_ssid_sta(char *ssid);
void get_nvs_pass_sta(char *pass);
//Escritura
void escritura_nvs_sta(uint8_t subuni,char ssid[],char pass[]);
void escritura_nvs_ap(uint8_t subuni,char ssid[],char pass[]);

#endif //___NVS_ESP_H__