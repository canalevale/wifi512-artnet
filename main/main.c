#include <stdio.h>
#include "ArtNet.h"
#include "nvs_flash.h"
#include "WiFi.h"
#include "led_board.h"
#include "UDP.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

///////Defines y variables///////

//Queue para las tareas
QueueHandle_t Packet = 0;
QueueHandle_t PacketArtReply = 0;
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

  //Creo la Queue
  Packet = xQueueCreate(10, sizeof(uint8_t [MAX_BUFFER_ARTNET]));
  PacketArtReply = xQueueCreate(2, sizeof(uint8_t [MAX_BUFFER_ARTNET]));

  //Inicializo Leds
  init_led();
  //Inicializo Wifi
  wifi_init(MODE_STA);
  //Creo una tarea UDP Art-Net
  create_task();

  while (1)
  {
    toggle_led();
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
  
}

esp_err_t create_task(void){

  xTaskCreate(udp_task, "Udp_socket", 4096, (void*)AF_INET, 5, NULL); //Tarea UDP
  xTaskCreate(artnet_task, "Art_net", 4096, NULL , 5, NULL); //Tarea Art-Net

  return ESP_OK;
}
