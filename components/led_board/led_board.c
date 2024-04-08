#include <stdio.h>
#include "led_board.h"

bool led_status=0;
const char *TAG="Led_Board";
//Funciones
esp_err_t init_led(void){
    gpio_reset_pin(LED);
    gpio_set_direction(LED,GPIO_MODE_OUTPUT);
    ESP_LOGI(TAG,"Led Iniciado - GPIO:%d",LED);
    return ESP_OK;
}
esp_err_t toggle_led(void){
    led_status=!led_status;
    gpio_set_level(LED,led_status);
    /*
    if(led_status){
        ESP_LOGI(TAG,"Led On");
    }else{
        ESP_LOGI(TAG,"Led Off");
        }
    */
    

    return ESP_OK;
}
