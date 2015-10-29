#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include "cmsis_os.h"

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_uart.h"

void init_wifi();

void set_command(const uint8_t* command, uint8_t size, uint32_t timeout);
void enter_command_mode();
void exit_command_mode();

#endif //UART_H