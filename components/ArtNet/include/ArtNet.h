/* >> SD4 - Proyecto Nodo Wifi512-ArtNet << */
#ifndef ___ARTNET_H__
#define ___ARTNET_H__
//Includes
#include "esp_log.h"
#include "UDP.h"
#include "esp_wifi.h"
#include "WiFi.h"
//Defines

// Puerto UDP
#define ART_NET_PORT 6454
// Opcodes
#define ART_POLL 0x2000
#define ART_WIFISIGNAL 0x6001
#define ART_POOLREPLY 0x2100
#define ART_DMX 0x5000
#define ART_SYNC 0x5200
// Buffers
#define MAX_BUFFER_ARTNET 530
// Packet
#define ART_NET_ID "Art-Net"
#define ART_DMX_START 18
//Tiempo en el que se envia los datos de Wifi
#define TIMER_WIFISIGNAL pdMS_TO_TICKS(2000)

// Estructura para el paquete ArtPoll
typedef struct {
    uint8_t id[8];      // "Art-Net" + 0x00
    uint16_t opcode;    // Código de operación (OpPoll = 0x2000)
    uint8_t protVerH;   // Versión de protocolo alta (siempre 0)
    uint8_t protVerL;   // Versión de protocolo baja (siempre 14)
    uint8_t talkToMe;   // Opciones de comunicación
    uint8_t priority;   // Prioridad de sincronización de la red
} ArtNetPollPacket;

// Estructura para el paquete ArtPollReply
typedef struct {
    uint8_t id[8];      // "Art-Net" + 0x00
    uint16_t opcode;    // Código de operación (OpPollReply = 0x2100)
    uint8_t ipAddress[4]; // Dirección IP del dispositivo
    uint16_t port;      // Puerto de comunicación (generalmente 6454)
    uint8_t verH; // Información de versión alta
    uint8_t ver; // Información de versión baja
    uint8_t  subH;
    uint8_t  sub;
    uint8_t  oemH;
    uint8_t  oem;
    uint8_t  ubea;
    uint8_t  status;
    uint8_t  etsaman[2];
    uint8_t  shortname[18];
    uint8_t  longname[64];
    uint8_t  nodereport[64];
    uint8_t  numbportsH;
    uint8_t  numbports;
    uint8_t  porttypes[4];//max of 4 ports per node
    uint8_t  goodinput[4];
    uint8_t  goodoutput[4];
    uint8_t  swin[4];
    uint8_t  swout[4];
    uint8_t  swvideo;
    uint8_t  swmacro;
    uint8_t  swremote;
    uint8_t  sp1;
    uint8_t  sp2;
    uint8_t  sp3;
    uint8_t  style;
    uint8_t  mac[6];
    uint8_t  bindip[4];
    uint8_t  bindindex;
    uint8_t  status2;
    uint8_t  filler[26];
} ArtNetPollReplyPacket;

// Estructura para el paquete Art-DMX
typedef struct {
    uint8_t id[8];      // "Art-Net" + 0x00
    uint16_t opcode;    // Código de operación (OpDMX = 0x5000)
    uint8_t protVerH;   // Versión de protocolo alta (siempre 0)
    uint8_t protVerL;   // Versión de protocolo baja (siempre 14)
    uint8_t sequence;   // Numero de secuencia
    uint8_t physical;   // Direccion fisica del puerto DMX
    uint8_t subuni;     // SubNet
    uint8_t net;        // Net
    uint8_t lengthhi;   // Tamaño de los datos DMX
    uint8_t length;     // Tamaño de los datos DMX
    uint8_t data[512];  // Datos DMX

} ArtNetDMX;




//Funciones
void artnet_task(void *pvParameters);
void set_universo(void);
uint8_t get_universo();


#endif //___ARTNET_H__