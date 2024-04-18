#include <stdio.h>
#include "UART_send.h"
#include "UDP.h"
#include "ArtNet.h"

extern QueueHandle_t PacketUART;
static const char *TAG_UART = "UART";
//static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

uint8_t dmx_packet[513];

void UART_init(){

    uart_config_t uart_config= {
        .baud_rate=DMX_BAUD_RATE,
        .data_bits=UART_DATA_8_BITS,
        .stop_bits=UART_STOP_BITS_2,
        .parity=UART_PARITY_DISABLE,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };
    //Instalo los Drivers de la UART
    uart_driver_install(UART_NUM_2,BUFER,0,0,NULL,0);
    ESP_LOGI(TAG_UART,"Driver");
    //Configuro la UART
    uart_param_config(UART_NUM_2, &uart_config);
    ESP_LOGI(TAG_UART,"config");
    // Set UART log level
    esp_log_level_set(TAG_UART, ESP_LOG_INFO);
    ESP_LOGI(TAG_UART,"logi");
    //Configuro los PINs
    uart_set_pin(UART_NUM_2,TX_PIN,RX_PIN,RTS_PIN,UART_PIN_NO_CHANGE);
    ESP_LOGI(TAG_UART,"set pin");
    //Seteo la UART para una comunicacion Half-Duplex RS485 
    uart_set_mode(UART_NUM_2, UART_MODE_RS485_HALF_DUPLEX);
    ESP_LOGI(TAG_UART,"rs485");
    //Seteo Timeout para la lectura
    uart_set_rx_timeout(UART_NUM_2, READ_TOUT);

    ESP_LOGI(TAG_UART,"UART Iniciada");

}

void uart_task(void *pvParameters){

    //uart_event_t event_uart;
    //uint8_t *data= (uint8_t *)malloc(BUFER);

    while (1)
    {
        if(!xQueueReceive(PacketUART,dmx_packet,pdMS_TO_TICKS(15))){
            //ESP_LOGI(TAG_UART,"No se recibio paquete");
        }
        else{
           // vTaskSuspend(artnet_task);
          //  vTaskSuspend(udp_task);
           // taskENTER_CRITICAL(&spinlock);

            ESP_LOGE(TAG_UART,"Envio de dato DMX");
            uart_set_rts(UART_NUM_2,0);
            //Space for Break 
            uart_set_line_inverse(UART_NUM_2,UART_SIGNAL_TXD_INV); 
            ets_delay_us(BREAK);
            //Mark 
            uart_set_line_inverse(UART_NUM_2,UART_SIGNAL_INV_DISABLE);
            ets_delay_us(MAB);
            
            uart_write_bytes(UART_NUM_2, dmx_packet,sizeof(dmx_packet));

           // taskEXIT_CRITICAL(&spinlock);
            //vTaskResume(artnet_task);
           // vTaskResume(udp_task);

        }
    }
    

}