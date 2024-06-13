#ifndef ___UART_Send_H__
#define ___UART_Send_H__

#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "DMX.h"
#include "rom/ets_sys.h"
#include "driver/gptimer.h"


#define BUFER 513
#define TX_PIN 17
#define RX_PIN 16
#define RTS_PIN 5
// Timeout threshold for UART = number of symbols (~10 tics) with unchanged state on receive pin
#define READ_TOUT          (3) // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks
#define UART_TIMER_INTERVAL_MS 19

void uart_task(void *pvParameters);
bool uart_time(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *user_ctx);
void UART_init(void);

#endif //___UART_Send_H__