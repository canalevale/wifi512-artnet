/* >> SD4 - Proyecto Nodo Wifi512-ArtNet << */

//Includes

//Defines

// Puerto UDP
#define ART_NET_PORT 6454
// Opcodes
#define ART_POLL 0x2000
#define ART_POOLREPLY 0x2100
#define ART_DMX 0x5000
#define ART_SYNC 0x5200
// Buffers
#define MAX_BUFFER_ARTNET 530
// Packet
#define ART_NET_ID "Art-Net"
#define ART_DMX_START 18

uint8_t artnet_packet[MAX_BUFFER_ARTNET];

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
    uint8_t versionInfoH; // Información de versión alta
    uint8_t versionInfoL; // Información de versión baja
    // Otros campos de información, como número de universos, etc.
} ArtNetPollReplyPacket;

// Estructura para el paquete Art-DMX
typedef struct {
    uint8_t id[8];      // "Art-Net" + 0x00
    uint16_t opcode;    // Código de operación (OpPoll = 0x5000)
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