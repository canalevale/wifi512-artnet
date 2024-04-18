#ifndef ___DMX_H__
#define ___DMX_H__

#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"

#define BREAK 176                //Space for break
#define MAB 30                  //Mark After Break
#define DMX_SIZE 512            //Numero de canales
#define DMX_START_CODE 0x00     // Código de inicio de DMX
#define DMX_BAUD_RATE 250000    //Baud-Rate DMX 250k

//Funciones


#endif //___DMX_H__