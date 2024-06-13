#include <stdio.h>
#include <string.h>
#include "NVS_ESP.h"

static const char * TAG_NVS = "NVS";

esp_err_t err;
size_t mode_len;
size_t ssid_len_sta;
size_t ssid_len_ap;
size_t pass_len_sta;
size_t pass_len_ap;

//Devueve el modo de Wifi - STA/AP
void get_nvs_mode(char *mode){   
    
    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error abriendo NVS namespace");
        return;
    }
    
    // Recuperar los datos del espacio de almacenamiento NVS
    //Mode
    err = nvs_get_str(nvs_handle, "mode", mode, &mode_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error obteniendo mode");
        nvs_close(nvs_handle);
        return;
    }
    
    nvs_close(nvs_handle);
}

//Cambia de modo
void change_mode_wifi(void){
    char mode[4];
    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error abriendo NVS namespace");
        return;
    }
    
    // Recuperar los datos del espacio de almacenamiento NVS
    //Mode
    err = nvs_get_str(nvs_handle, "mode", mode, &mode_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error obteniendo mode");
        nvs_close(nvs_handle);
        return;
    }
    
    if(strcmp(mode,"STA")==0){
    //Cambio a AP
    mode_len= strlen("AP")+1;
    err = nvs_set_str(nvs_handle, "mode", "AP");
        if (err != ESP_OK) {
            ESP_LOGE(TAG_NVS,"AP - Error seteando mode\n");
            nvs_close(nvs_handle);
            return;
        }
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
            return;
        }
    
    }else if(strcmp(mode,"AP")==0){
        //Cambio a STA
        mode_len= strlen("STA")+1;
        err = nvs_set_str(nvs_handle, "mode", "STA");
        if (err != ESP_OK) {
            ESP_LOGE(TAG_NVS,"STA - Error seteando mode\n");
            nvs_close(nvs_handle);
            return;
        }
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
            return;
        }
    }

    nvs_close(nvs_handle);
}


//MODOS AP
//Devuelve el SubUniversso Artnet para modo AP
uint8_t get_nvs_ap_subuni(void){   
    uint8_t subuni;
    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error abriendo NVS namespace");
        return 0;
    }
    
    // Recuperar los datos del espacio de almacenamiento NVS
    //SubUni ArtNet
    err = nvs_get_u8(nvs_handle, "uni_ap", &subuni);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error obteniendo subuni");
        nvs_close(nvs_handle);
        return 0;
    }

    nvs_close(nvs_handle);
    return subuni;
}
//Devuelve el SSID para modo AP
void get_nvs_ssid_ap(char *ssid){   
    
    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error abriendo NVS namespace");
        return;
    }
    
    // Recuperar los datos del espacio de almacenamiento NVS
    //SSID
    err = nvs_get_str(nvs_handle, "ssid_ap", ssid, &ssid_len_ap);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error obteniendo ssid");
        nvs_close(nvs_handle);
        return;
    }
    
    
    nvs_close(nvs_handle);
}
//Devuelve el Password para modo AP
void get_nvs_pass_ap(char *pass){   
    
    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error abriendo NVS namespace");
        return;
    }
    // Recuperar los datos del espacio de almacenamiento NVS
    //Password
    err = nvs_get_str(nvs_handle, "pass_ap", pass, &pass_len_ap);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error obteniendo pass");
        nvs_close(nvs_handle);
        return;
    }

    // Cerrar el espacio de almacenamiento NVS
    nvs_close(nvs_handle);
}


//MODOS STA
//Devuelve el SubUniversso Artnet para modo STA
uint8_t get_nvs_sta_subuni(void){   
    uint8_t subuni;
    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error abriendo NVS namespace");
        return 0;
    }
    
    // Recuperar los datos del espacio de almacenamiento NVS
    //SubUni ArtNet
    err = nvs_get_u8(nvs_handle, "uni_sta", &subuni);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error obteniendo subuni");
        nvs_close(nvs_handle);
        return 0;
    }

    nvs_close(nvs_handle);
    return subuni;
}
//Devuelve el SSID para modo STA
void get_nvs_ssid_sta(char *ssid){   
    
    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error abriendo NVS namespace");
        return;
    }

    // Recuperar los datos del espacio de almacenamiento NVS
    //SSID
    err = nvs_get_str(nvs_handle, "ssid_sta", ssid, &ssid_len_sta);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error obteniendo ssid");
        nvs_close(nvs_handle);
        return;
    }
    
    // Cerrar el espacio de almacenamiento NVS  
    nvs_close(nvs_handle);
}

void get_nvs_pass_sta(char *pass){   
    
    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error abriendo NVS namespace");
        return;
    }
    // Recuperar los datos del espacio de almacenamiento NVS
    //Password
    err = nvs_get_str(nvs_handle, "pass_sta", pass, &pass_len_sta);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error obteniendo pass");
        nvs_close(nvs_handle);
        return;
    }

    // Cerrar el espacio de almacenamiento NVS
    nvs_close(nvs_handle);
}

//Escrituras
void escritura_nvs_sta(uint8_t subuni,char ssid[],char pass[]){
    
    // Imprimir los datos recuperados
    ESP_LOGI(TAG_NVS,"STA-Subuni: %d", subuni);
    ESP_LOGI(TAG_NVS,"STA-Ssid: %s", ssid);
    ESP_LOGI(TAG_NVS,"STA-Pass: %s", pass);
    
    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error abriendo NVS namespace\n");
        return;
    }
    
    // Guardo los datos del espacio de almacenamiento NVS
    //SubUni ArtNet
    err = nvs_set_u8(nvs_handle, "uni_sta", subuni);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error seteando subuni\n");
        nvs_close(nvs_handle);
        return;
    }
    //Comento el cambio
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
        return;
    }    
    
    
    //Mode
    mode_len= strlen("STA")+1;
    err = nvs_set_str(nvs_handle, "mode", "STA");
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error seteando mode\n");
        nvs_close(nvs_handle);
        return;
    }
     //Comento el cambio
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
        return;
    }       
    
    
    //SSID
    ssid_len_sta = strlen(ssid)+1;
    err = nvs_set_str(nvs_handle, "ssid_sta", ssid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error seteando ssid");
        nvs_close(nvs_handle);
        return;
    }
    //Comento el cambio
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
        return;
    }    

    
    //Password
    pass_len_sta = strlen(pass)+1;
    err = nvs_set_str(nvs_handle, "pass_sta", pass);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"STA - Error seteando pass\n");
        nvs_close(nvs_handle);
        return;
    }
    //Comento el cambio
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
        return;
    }        
    
    
    ESP_LOGI(TAG_NVS,"Se termino de escribir STA");
    ESP_LOGI(TAG_NVS,"STA - ssdi_len %d", ssid_len_sta);
    ESP_LOGI(TAG_NVS,"STA - pass_len %d", pass_len_sta);
    
    // Cerrar el espacio de almacenamiento NVS
    nvs_close(nvs_handle);
  
}

void escritura_nvs_ap(uint8_t subuni,char ssid[],char pass[]){
    
    // Imprimir los datos recuperados
    ESP_LOGI(TAG_NVS,"AP-Subuni: %d", subuni);
    ESP_LOGI(TAG_NVS,"AP-Ssid: %s", ssid);
    ESP_LOGI(TAG_NVS,"AP-Pass: %s", pass);

    nvs_handle_t nvs_handle;
    // Abrir el espacio de almacenamiento NVS
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error abriendo NVS namespace");
        return;
    }
    
    // Guardo los datos del espacio de almacenamiento NVS
    //SubUni ArtNet
    err = nvs_set_u8(nvs_handle, "uni_ap", subuni);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error seteando subuni\n");
        nvs_close(nvs_handle);
        return;
    }
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
       ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
        return;
    }
    
    //Mode
    mode_len= strlen("AP")+1;
    err = nvs_set_str(nvs_handle, "mode", "AP");
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error seteando mode\n");
        nvs_close(nvs_handle);
        return;
    }
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
       ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
        return;
    }
    
    //SSID
    ssid_len_ap = strlen(ssid)+1;
    err = nvs_set_str(nvs_handle, "ssid_ap", ssid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error seteando ssid\n");
        nvs_close(nvs_handle);
        return;
    }
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
        return;
    }
    
    //Password
    pass_len_ap = strlen(pass)+1;
    err = nvs_set_str(nvs_handle, "pass_ap", pass);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"AP - Error seteando pass\n");
        nvs_close(nvs_handle);
        return;
    }
    
    //Comento el cambio
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_NVS,"Error commiteando cambios en NVS\n");
        return;
    }
    // Cerrar el espacio de almacenamiento NVS
    nvs_close(nvs_handle);
}

void get_nvs_all(void){
    char get[8];
    get_nvs_mode(get);
    //AP
    get_nvs_ssid_ap(get);
    get_nvs_pass_ap(get);
    //STA
    get_nvs_ssid_sta(get);
    get_nvs_pass_sta(get);
    
}