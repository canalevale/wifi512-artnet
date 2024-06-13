#include <stdio.h>
#include "UDP.h"
#include "DMX.h"
#include "ArtNet.h"

static const char *TAG_UDP = "UDP";
extern QueueHandle_t Packet;
extern QueueHandle_t Reply;


void udp_task(void *pvParameters)
{
    
    //
    uint8_t rx_buffer[530];
    uint8_t art_reply[530];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;
    
    while (1) 
    {
        //ESP_LOGW(TAG_UDP, "4");
        //Creacion de socket
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;   
        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
        ESP_LOGE(TAG_UDP, "No es posible crear el Socket Art-Net: errno %d", errno);
        break;
        }

        ESP_LOGI(TAG_UDP, "Socket Art-Net Creado");
        
        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG_UDP, "Socket Art-Net no se puede vincular: errno %d", errno);
        }
        ESP_LOGI(TAG_UDP, "Socket Art-Net vinculado, port %d", PORT);

        struct sockaddr_in source_addr; // sockaddr_storage
        socklen_t socklen = sizeof(source_addr);
        
        struct sockaddr_in dest_addr_signal;
        dest_addr_signal.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_signal.sin_family = AF_INET;
        dest_addr_signal.sin_port = htons(6455);
        
        while (!ifconecct()){
            //ESP_LOGI(TAG_UDP, "Esperando datos");
            //ESP_LOGW(TAG_UDP, "1");
            if(!xQueueReceive(Reply, art_reply, pdMS_TO_TICKS(20))){    
            }else{
                if((art_reply[8] | art_reply[9]<<8) == ART_POOLREPLY){
                    int err = sendto(sock, art_reply, sizeof(art_reply), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                    ESP_LOGW(TAG_UDP,"Se envio una mensaje - ArtPoolReaply");
                    if (err < 0) {
                        ESP_LOGE(TAG_UDP, "Error occurred during sending: errno %d", errno);
                        break;
                }  
                
                }else{
                    dest_addr_signal.sin_addr =source_addr.sin_addr;
                    int err = sendto(sock, art_reply, (size_t) 14, 0, (struct sockaddr *)&dest_addr_signal, sizeof(dest_addr_signal));
                    //ESP_LOGW(TAG_UDP,"Se envio una mensaje - Rssi");
                    if (err < 0) {
                        ESP_LOGE(TAG_UDP, "Error occurred during sending: errno %d", errno);
                        break;
                }
                
                }  
            }
            

            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr *)&source_addr, &socklen);
            //ESP_LOGW(TAG_UDP,"Llego paquete UDP");
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGI(TAG_UDP, "recvfrom Fallo: errno %d", errno);
                break;
            }
            // Data received
            else{
                //Add to Queue
                //ESP_LOGI(TAG_UDP, "Parquete a Queue");
                //ESP_LOGW(TAG_UDP,"Secuencia_UDP:%d",rx_buffer[12]);
                // Si la cola está llena, elimina el elemento más antiguo
                if (uxQueueSpacesAvailable(Packet) == 0)
                {
                    uint8_t discardedItem;
                    xQueueReceive(Packet, &discardedItem, 0); // Eliminar el elemento más antiguo
                    //ESP_LOGE(TAG_UDP, "borro dato");
                }

                if (!xQueueSend(Packet, rx_buffer, pdMS_TO_TICKS(20))){ESP_LOGE(TAG_UDP, "Error de envio Queue");}
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
