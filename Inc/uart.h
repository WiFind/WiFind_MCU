#ifndef UART_H
#define UART_H

#include "stm32l0xx_hal.h"
#include "cmsis_os.h"
#include "semphr.h"

#define UART_SEND_COMMAND_SIGNAL (0x01)

extern xSemaphoreHandle button_sem;

void uart_thread(void const *);


#endif //UART_H
