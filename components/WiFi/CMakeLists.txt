idf_component_register(SRCS "WiFi.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "lwip""esp_wifi""esp_event""ArtNet""NVS_ESP""led_board")
