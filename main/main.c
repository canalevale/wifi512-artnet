#include <stdio.h>
#include "ArtNet.h"
#include "nvs_flash.h"
#include "WiFi_AP.h"
#include "led_board.h"
#include "UDP.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
//inicializo NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

  init_led();
  //inicializo Wifi
  wifi_init();
  //Creo una tarea UDP
  xTaskCreate(udp_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);
  while (1)
  {
    toggle_led();
    vTaskDelay(500/portTICK_PERIOD_MS);

  }
  
}
