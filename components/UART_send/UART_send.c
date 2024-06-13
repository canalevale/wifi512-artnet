#include <stdio.h>
#include "UART_send.h"
#include "UDP.h"
#include "ArtNet.h"
#include "led_board.h"
#include "driver/gptimer.h"

gptimer_handle_t gptimer = NULL;
extern QueueHandle_t PacketUART;
static const char *TAG_UART = "UART";
TimerHandle_t xTimer_uart = NULL;
//static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
bool time_uart=1;
uint8_t dmx_packet[513];

bool uart_time(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *user_ctx){
    time_uart=true;
    return false;
}

void uart_task(void *pvParameters){

    while (!ifconecct())
    {
        //ESP_LOGW(TAG_UART,"3");
        if(time_uart){
        if(!xQueueReceive(PacketUART,dmx_packet,pdMS_TO_TICKS(10))){
            //ESP_LOGI(TAG_UART,"No se recibio paquete");
        }
        else{
            //ESP_LOGE(TAG_UART,"Envio de dato DMX");
            ESP_ERROR_CHECK(gptimer_stop(gptimer));
            set_led(LED_UART,1);
            //Seteo pin RTS para enviar datos
            uart_set_rts(UART_NUM_2,0);
            //Mantengo un tiempo la linea en alto
            vTaskDelay(4/portTICK_PERIOD_MS);
            //Space for Break 
            uart_set_line_inverse(UART_NUM_2,UART_SIGNAL_TXD_INV);
            ets_delay_us(BREAK);
            //Mark 
            uart_set_line_inverse(UART_NUM_2,UART_SIGNAL_INV_DISABLE);
            ets_delay_us(MAB);
            //Envio datos
            uart_write_bytes(UART_NUM_2,(const uint8_t *)dmx_packet,sizeof(uint8_t [DMX_SIZE+1]));
            //Espero que se terminen de enviar los datos antes de enviar otro, RTS baja automaticamente
            uart_wait_tx_done(UART_NUM_2,pdMS_TO_TICKS(20));
            //Seteo pin RTS para enviar datos
            uart_set_rts(UART_NUM_2,1);
            time_uart=false;
            set_led(LED_UART,0);
            //ESP_LOGI(TAG_UART,"Mando un paquete");
            ESP_ERROR_CHECK(gptimer_start(gptimer));
        }}
    }

}

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
    uart_set_mode(UART_NUM_2,UART_MODE_UART ); //UART_MODE_RS485_HALF_DUPLEX
    ESP_LOGI(TAG_UART,"rs485");
    //Seteo Timeout para la lectura
    uart_set_rx_timeout(UART_NUM_2, READ_TOUT);
    // Configurar el temporizador

    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1 MHz, 1 tick = 1 microsecond
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&config, &gptimer));

    // Configurar la alarma del temporizador
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 10000, // Alarma cada 1 mS (30000 ticks)
        .reload_count = 0,      // No hay recarga del contador
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    // Registrar la función de devolución de llamada
    gptimer_event_callbacks_t cbs = {
        .on_alarm = uart_time,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    // Iniciar el temporizador
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    ESP_LOGI(TAG_UART,"UART Iniciada");
}
