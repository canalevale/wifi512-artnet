#include <stdio.h>

#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/queue.h"
///////Componentes///////
#include "DMX.h"
#include "WiFi.h"
#include "led_board.h"
#include "UDP.h"
#include "UART_send.h"
#include "ArtNet.h"
#include "Web_Server.h"
#include "NVS_ESP.h"

///////Defines y variables///////

//Queue para las tareas
QueueHandle_t Packet = 0;
QueueHandle_t Reply = 0;
QueueHandle_t PacketUART=0;
///////Funicones///////
esp_err_t create_task(void);
///////Main///////
void app_main(void)
{
  //Inicializo Flash
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
     ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  //escritura_nvs_ap(0,"NodoTest","SD4NodoTest");
  //escritura_nvs_sta(0,"Fibertel WiFi CE2 2.4GHz","mUTe7gRw2Z");
  //Obtengo todos los parametros antes de inicializar
  get_nvs_all();

  //Inicializo Leds
  init_led();

  //Creo la Queue
  Packet = xQueueCreate(10, sizeof(uint8_t [MAX_BUFFER_ARTNET]));
  Reply = xQueueCreate(5, sizeof(uint8_t [MAX_BUFFER_ARTNET]));
  PacketUART=xQueueCreate(10, sizeof(uint8_t [DMX_SIZE+1]));

  //Inicializo Wifi
  wifi_init();
  //Inicializo UART
  UART_init();
  //Seteo universo Art-Net
  set_universo();
  //Creo una tarea UDP Art-Net
  create_task();
  //Inicializo el WebServer
  start_webserver();

  while (1)
  {
    
    set_led(LED_BOARD,1);
    vTaskDelay(500/portTICK_PERIOD_MS);
    set_led(LED_BOARD,0);
    vTaskDelay(500/portTICK_PERIOD_MS);
  
    if(gpio_get_level(BOOT_PUSH) == 0){
      set_led(LED_WIFI,1);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      if(gpio_get_level(BOOT_PUSH) == 0){
        ESP_LOGI("MAIN","Cambio de Modo");
        change_mode_wifi();
        ESP_LOGI("MAIN","Reset....");
        vTaskDelay(500 / portTICK_PERIOD_MS);
        esp_restart();
      }

    }
  }
  
}

esp_err_t create_task(void){

  xTaskCreatePinnedToCore(udp_task, "Udp_socket", 4096, (void*)AF_INET, 8, NULL,1); //Tarea UDP
  xTaskCreatePinnedToCore(artnet_task, "Art_net", 4096, NULL , 9, NULL,1); //Tarea Art-Net
  xTaskCreatePinnedToCore(uart_task, "UART", 4096, NULL , 1, NULL, 0); //Tarea UART tskNO_AFFINITY

  return ESP_OK;
}
