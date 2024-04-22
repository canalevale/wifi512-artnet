#include <stdio.h>
#include "ArtNet.h"
#include "nvs_flash.h"
#include "WiFi.h"
#include "led_board.h"
#include "UDP.h"
#include "UART_send.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "DMX.h"
///////Defines y variables///////

//Queue para las tareas
QueueHandle_t Packet = 0;
QueueHandle_t PacketArtReply = 0;
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

  //Creo la Queue
  Packet = xQueueCreate(7, sizeof(uint8_t [MAX_BUFFER_ARTNET]));
  PacketArtReply = xQueueCreate(2, sizeof(uint8_t [MAX_BUFFER_ARTNET]));
  PacketUART=xQueueCreate(7, sizeof(uint8_t [DMX_SIZE+1]));
  //Inicializo Leds
  init_led();
  //Inicializo Wifi
  wifi_init(MODE_STA);
  //Inicializo UART
  UART_init();
  //Creo una tarea UDP Art-Net
  create_task();
  while (1)
  {
    toggle_led();
    vTaskDelay(200/portTICK_PERIOD_MS);
  }
  
}

esp_err_t create_task(void){

  xTaskCreatePinnedToCore(udp_task, "Udp_socket", 4096, (void*)AF_INET, 2, NULL,tskNO_AFFINITY); //Tarea UDP
  xTaskCreatePinnedToCore(artnet_task, "Art_net", 4096, NULL , 2, NULL,tskNO_AFFINITY); //Tarea Art-Net
  xTaskCreatePinnedToCore(uart_task, "UART", 4096, NULL , 1, NULL,tskNO_AFFINITY); //Tarea UART


  return ESP_OK;
}
