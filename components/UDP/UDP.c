#include <stdio.h>
#include "UDP.h"

static const char *TAG_UDP = "UDP";

void udp_task(void *pvParameters)
{
    uint8_t rx_buffer[530];
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;
    uint16_t opcode;
    
    while (1) {
        //Creacion de socket
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;   
        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
        ESP_LOGE(TAG_UDP, "No es posible crear el Socket: errno %d", errno);
        break;
        }
   
        ESP_LOGI(TAG_UDP, "Socket Creado");

        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG_UDP, "Socket no se puede vincular: errno %d", errno);
        }
        ESP_LOGI(TAG_UDP, "Socket vinculado, port %d", PORT);

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);

        while (1) {
            ESP_LOGI(TAG_UDP, "Esperando datos");
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr *)&source_addr, &socklen);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGI(TAG_UDP, "recvfrom Fallo: errno %d", errno);
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                ESP_LOGI(TAG_UDP, "Se recivieron %d bytes de %s:", len, addr_str);
                opcode = rx_buffer[8] | rx_buffer[9] << 8;
                if(opcode==0x5000){
                    ESP_LOGI(TAG_UDP, "DMX Canal 1:%d", rx_buffer[18]);
                }
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG_UDP, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}