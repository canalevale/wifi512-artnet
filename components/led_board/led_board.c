#include <stdio.h>
#include "led_board.h"

bool change_mode=0;
const char *TAG="GPIO";
//Funciones
esp_err_t init_led(void){

    gpio_reset_pin(LED_BOARD);
    gpio_set_direction(LED_BOARD,GPIO_MODE_OUTPUT);
    ESP_LOGI(TAG,"Led Iniciado - GPIO:%d",LED_BOARD);

    gpio_reset_pin(LED_UART);
    gpio_set_direction(LED_UART,GPIO_MODE_OUTPUT);
    ESP_LOGI(TAG,"Led Iniciado - GPIO:%d",LED_UART);

    gpio_reset_pin(LED_WIFI);
    gpio_set_direction(LED_WIFI,GPIO_MODE_OUTPUT);
    ESP_LOGI(TAG,"Led Iniciado - GPIO:%d",LED_WIFI);

    gpio_reset_pin(BOOT_PUSH);
    gpio_set_direction(BOOT_PUSH,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BOOT_PUSH, GPIO_PULLUP_ONLY);


    set_led(LED_UART,0);
    set_led(LED_WIFI,0);
    /*
    gpio_set_intr_type(BOOT_PUSH,GPIO_INTR_NEGEDGE);
    // Configurar una interrupción en el pin BOOT
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
    gpio_isr_handler_add(BOOT_PUSH, change_mode_handler, (void*) BOOT_PUSH);
    ESP_LOGI(TAG,"Pulsador Iniciado - GPIO:%d",BOOT_PUSH);
    */
    return ESP_OK;
}

void change_mode_handler(void* arg) {
    // Verificar el estado del botón BOOT después del debounce
    if (gpio_get_level(BOOT_PUSH) == 0) { // Si el botón está presionado (nivel bajo)
        change_mode=1;
    }
}
bool get_mode_change(void){return change_mode;}
/*
esp_err_t toggle_led(gpio_num_t led){
    ESP_LOGI(TAG,"Led:%d",led);
    ESP_LOGI(TAG,"estado gget:%d",gpio_get_level(led));
    if (gpio_get_level(led)==1){
        gpio_set_level(led,0);
        ESP_LOGI(TAG,"Led APAGO");
    }else{
        gpio_set_level(led,1);
        ESP_LOGI(TAG,"Led prendido");
    }
    return ESP_OK;
}*/

esp_err_t set_led(gpio_num_t led,bool status){
    gpio_set_level(led,status);  
    return ESP_OK;
}
