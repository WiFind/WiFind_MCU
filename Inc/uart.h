#ifndef UART_H
#define UART_H

#include "stm32l0xx_hal.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "stdbool.h"

#define UART_SEND_COMMAND_SIGNAL (0x01)

extern xSemaphoreHandle button_sem;
extern bool uart_gb_message_processing;
extern bool uart_gb_cancel;

void uart_thread(void const *);


#endif //UART_H
