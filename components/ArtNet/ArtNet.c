#include <stdio.h>
#include "ArtNet.h"
#include "DMX.h"

static const char *TAG_ART = "Art-Net";

extern QueueHandle_t Packet;
extern QueueHandle_t Reply;
extern QueueHandle_t PacketUART;
uint8_t *data_send;
uint8_t universo=0;


void artnet_task(void *pvParameters){

    uint8_t art_buffer[MAX_BUFFER_ARTNET];
    while (!ifconecct())
    {
        //ESP_LOGW(TAG_ART, "2");
        if(!xQueueReceive(Packet, art_buffer, pdMS_TO_TICKS(20))){
            //ESP_LOGE(TAG_ART, "Error de recepcion Queue");
        }else{

            switch ((art_buffer[8] | art_buffer[9] << 8)){
            case ART_DMX:

                if(universo==art_buffer[14]){ //Compruevo si el paquete corresponde a este universo

                art_buffer[17]=DMX_START_CODE; //Start Code
                data_send=&art_buffer[17];

                if (uxQueueSpacesAvailable(PacketUART) == 0)
                {
                    uint8_t discardedItem2;
                    xQueueReceive(PacketUART, &discardedItem2, 0); // Eliminar el elemento más antiguo
                    //ESP_LOGE(TAG_ART, "borro dato");
                }
                
                if (!xQueueSend(PacketUART, data_send, pdMS_TO_TICKS(15))){
                    ESP_LOGE(TAG_ART, "Error de envio Queue2");
                }
                //ESP_LOGW(TAG_ART,"Envio paquete a UART");
                
                }
                
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
                 sprintf((char *)shortname, "Nodo Wifi");
                 sprintf((char *)longname, "Nodo Wifi-Artnet [Bridge with ESP32]");
                 memcpy(reply.shortname, shortname, sizeof(shortname));
                 memcpy(reply.longname, longname, sizeof(longname));
                 memcpy(reply.nodereport, "#0001 [0000] Nodo Wifi-ArtNet.[Bridge with ESP32] -> Estado OK.", sizeof(reply.nodereport));
                 reply.etsaman[0] = 0;
                 reply.etsaman[1] = 0;
                 reply.verH       = 0;
                 reply.ver        = 14;
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
                

                if (!xQueueSend(Reply,(uint8_t *) &reply, pdMS_TO_TICKS(15))){
                    ESP_LOGE(TAG_ART, "Error: No se pudo añadir ArtPollReply al Queue");
                }

                 break;
            case ART_POOLREPLY:
                 ESP_LOGI(TAG_ART, "Paquete Art_PollReply");
                 break;

            
            }
        }
       
    }

    
}

void set_universo(void){
    char mod[4];
    get_nvs_mode(mod);
    if(strcmp(mod,"STA")==0){
        universo=get_nvs_sta_subuni();
    }else if (strcmp(mod,"AP")==0){
        universo=get_nvs_ap_subuni();
    }
}

uint8_t get_universo(){return universo;}