#include <stdio.h>
#include "ArtNet.h"

static const char *TAG_ART = "Art-Net";

uint16_t opcode=0;
extern QueueHandle_t Packet;
extern QueueHandle_t PacketArtReply;


void artnet_task(void *pvParameters){

    uint8_t art_buffer[MAX_BUFFER_ARTNET];

    while (1)
    {
        if(!xQueueReceive(Packet, art_buffer, pdMS_TO_TICKS(100))){
            //ESP_LOGE(TAG_ART, "Error de recepcion Queue");
        }else{
            ESP_LOGE(TAG_ART, "Recepcion Queue");
        
            opcode = art_buffer[8] | art_buffer[9] << 8;

            switch (opcode){
            case ART_DMX:
                ESP_LOGI(TAG_ART, "DMX Canal 1:%d", art_buffer[18]);
                break;
            case ART_POLL:
                 ESP_LOGW(TAG_ART, "Paquete Art_Poll");
                 ArtNetPollReplyPacket reply;
                
                // ID Art-net
                memcpy(reply.id, ART_NET_ID, sizeof(reply.id));
                // Datos IP
                 esp_ip4_addr_t ip_esp;
                 ip_esp=get_ip();
                 printf( "IP: "IPSTR"\n", IP2STR(&ip_esp));
                 int8_t datoip[4];
                 datoip[0]=ip_esp.addr & 0xff;
                 datoip[1]=(ip_esp.addr>>8) & 0xff;
                 datoip[2]=(ip_esp.addr>>16) & 0xff;
                 datoip[3]=(ip_esp.addr>>24) & 0xff;
                 memcpy(reply.ipAddress,datoip,sizeof(reply.ipAddress));
                // OPCODE
                 reply.opcode = ART_POOLREPLY;
                // Puerto
                 reply.port =  ART_NET_PORT;
                 memset(reply.goodinput,  0x08, 4);
                 memset(reply.goodoutput,  0x80, 4);
                 memset(reply.porttypes,  0xc0, 4);

                 uint8_t shortname [18];
                 uint8_t longname [64];
                 sprintf((char *)shortname, "Art-net ESP32");
                 sprintf((char *)longname, "Art-Net -> ESP32 bridge");
                 memcpy(reply.shortname, shortname, sizeof(shortname));
                 memcpy(reply.longname, longname, sizeof(longname));

                 reply.etsaman[0] = 0;
                 reply.etsaman[1] = 0;
                 reply.verH       = 1;
                 reply.ver        = 0;
                 reply.subH       = 0;
                 reply.sub        = 0;
                 reply.oemH       = 0;
                 reply.oem        = 0xFF;
                 reply.ubea       = 0;
                 reply.status     = 0xd2;
                 reply.swmacro    = 0;
                 reply.swremote   = 0;
                 reply.style      = 0;

                 reply.numbportsH = 0;
                 reply.numbports  = 4;
                 reply.status2    = 0x08;

                 reply.bindip[0] = datoip[0];
                 reply.bindip[1] = datoip[1];
                 reply.bindip[2] = datoip[2];
                 reply.bindip[3] = datoip[3];

                 uint8_t swin[4]  = {0x01,0x02,0x03,0x04};
                 uint8_t swout[4] = {0x01,0x02,0x03,0x04};

                for(uint8_t i = 0; i < 4; i++){
                    reply.swout[i] = swout[i];
                    reply.swin[i] = swin[i];
                }

                if (!xQueueSend(PacketArtReply,(uint8_t *) &reply, pdMS_TO_TICKS(100))){
                    ESP_LOGE(TAG_ART, "Error: No se pudo añadir ArtPollReply al Queue");
                }

                 break;
            case ART_POOLREPLY:
                 ESP_LOGI(TAG_ART, "Paquete Art_Poll");
                 break;
            }
        }
    }

    
}
