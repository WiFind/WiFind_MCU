/**
  ******************************************************************************
  * File Name          : uart.c
  * Description        : Thread that handles communication with the Wi-Fi module
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 EECS 473 2015 Fall WiFind Group
  */
#include "stm32l0xx_hal.h"
#include "cmsis_os.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>


/*
$$$
set dhcp
set wlan auth
set pass
set ssid
set join 0
save
reboot
$$$
join ...
// wait
open ...
// out of CMD mode
HELLO WORLD
*/


UART_HandleTypeDef huart1;
static const uint8_t TEST_COMMAND [] = "Hello World!";
static const uint8_t ENTER_COMMAND_MODE [] = "$$$";
static const uint8_t EXIT_COMMAND_MODE [] = "exit\r\n";
static const uint8_t CONNECT_TO_SERVER [] = "open 192.168.1.128 8000\r\n";
static const uint8_t DISCONNECT_FROM_SERVER [] = "close\r\n";
static const uint8_t SET_DHCP [] = "set ip dhcp 1\r\n";
static const uint8_t SET_AUTHENTICATION [] = "set wlan auth 4\r\n";
static const uint8_t SET_SSID [] = "set wlan ssid WiFind\r\n";
static const uint8_t SET_PASSPHRASE [] = "set wlan pass WiFind11\r\n";
static const uint8_t SET_JOIN_MODE [] = "set wlan join 0\r\n";
static const uint8_t SAVE [] = "save\r\n";
static const uint8_t REBOOT [] = "reboot\r\n";
static const uint8_t JOIN_WIFIND [] = "join WiFind\r\n";
static const uint8_t SLEEP [] = "sleep\r\n";
static const uint8_t SCAN [] = "scan 10\r\n";
static uint8_t rx_buffer [2000];
// private function protoypes
static void MX_USART1_UART_Init(void);

xSemaphoreHandle button_sem;

void send_command(const uint8_t * command, size_t size, size_t remain) {
	HAL_StatusTypeDef rx_status;
	for (size_t i = 0; i < size; i++) {
		HAL_UART_Transmit(&huart1, (uint8_t *) command + i, 1, 20);
		rx_status = HAL_UART_Receive(&huart1, rx_buffer + i, 1, 20);
	}
	rx_status = HAL_UART_Receive(&huart1, rx_buffer + size, 1, 20);

	rx_status = HAL_UART_Receive(&huart1, rx_buffer + size + 1, remain, 1000);
}

int send_scan_command(void) {
    int num_of_scan_result = 0;
    size_t size = sizeof(SCAN) - 1;
	HAL_StatusTypeDef rx_status;
	for (size_t i = 0; i < size; i++) {
		HAL_UART_Transmit(&huart1, (uint8_t *) SCAN + i, 1, 20);
		rx_status = HAL_UART_Receive(&huart1, rx_buffer + i, 1, 20);
	}
	rx_status = HAL_UART_Receive(&huart1, rx_buffer + size, 1, 10);

	rx_status = HAL_UART_Receive(&huart1, rx_buffer + size + 1, 8, 10);

    memset(rx_buffer, 0x00, 2000); // clear the buffer

    int buffer_offset = 0;
	rx_status = HAL_UART_Receive(&huart1, rx_buffer, 13, 400);
	buffer_offset = 13;

    for (;;) {
        rx_status = HAL_UART_Receive(&huart1, rx_buffer + buffer_offset, 1, 5);
        buffer_offset += 1;
        if (rx_buffer[buffer_offset - 1] == 0x0A) {
            break;
        }
    }

    if (rx_buffer[14] == 0x0D) {
        /* it discovered less than 10 channel */
        num_of_scan_result = rx_buffer[13] - '0';
    } else {
        /* it discovered more than 9 channel */
        num_of_scan_result = 10 * (rx_buffer[13] - '0') + rx_buffer[14] - '0';
    }
/*    buffer_offset += 13;
	for (;;) {
		rx_status = HAL_UART_Receive(&huart1, rx_buffer + buffer_offset, 1, 80);
		buffer_offset += 1;
		if (rx_buffer[buffer_offset - 1] == 0x0A) {
        break;
    }
	}
	for (int i = 13; i < buffer_offset-2; i++) {
		num_of_scan_result *= 10;
		num_of_scan_result += (rx_buffer[i] - '0');
	}*/
    for (int i = 0; i < num_of_scan_result; i++) {
        rx_status = HAL_UART_Receive(&huart1, rx_buffer + buffer_offset, 42, 80);
        buffer_offset += 42;
        for (;;) {
            /* keep receiving until hitting linefeed */
            rx_status = HAL_UART_Receive(&huart1, rx_buffer + buffer_offset, 1, 5);
            buffer_offset++;
            if (rx_buffer[buffer_offset - 1] == 0x0A) {
                break;
            }
        }
    }

    /* receive the 'END:\r\n' */
    rx_status = HAL_UART_Receive(&huart1, rx_buffer + buffer_offset, 6, 20);
    buffer_offset += 6;

    return buffer_offset; // return the number of bytes received
}

// public function
void uart_thread(void const *argument) {
	// This function assumes that wireless addresses and such have been set already.
	osDelay(1000);
  MX_USART1_UART_Init();
	button_sem = xSemaphoreCreateBinary();
	HAL_StatusTypeDef rx_status;

	for (;;) {
		xSemaphoreTake(button_sem, portMAX_DELAY);
		osDelay(1000);
		HAL_UART_Transmit(&huart1, (uint8_t *) ENTER_COMMAND_MODE, sizeof(ENTER_COMMAND_MODE) - 1, 1000);
		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
		//osDelay(500);
		rx_status = HAL_UART_Receive(&huart1, (uint8_t *) rx_buffer, 5, 500);

		//HAL_UART_Transmit(&huart1, (uint8_t *) SCAN, sizeof(SCAN) - 1, 1000);
		//osDelay(4000);
		//HAL_UART_Transmit(&huart1, (uint8_t *) "h", 1, 10);
		//HAL_UART_Receive(&huart1, rx_buffer, 1, 20);

		//HAL_UART_Transmit(&huart1, (uint8_t *) "hi12", 4, 20);
		//HAL_UART_Receive(&huart1, rx_buffer + 1, 4, 40);
		//rx_status = HAL_UART_Receive(&huart1, (uint8_t *) rx_buffer, 5, 10000);


		//huart1.State = HAL_UART_STATE_READY;

		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);


		//send_command(SLEEP, sizeof(SLEEP) - 1);

		send_command(SET_AUTHENTICATION, sizeof(SET_AUTHENTICATION) - 1, 13);
		send_command(SET_SSID, sizeof(SET_SSID) - 1, 13);
		send_command(SET_PASSPHRASE, sizeof(SET_PASSPHRASE) - 1, 13);
		send_command(SET_JOIN_MODE, sizeof(SET_JOIN_MODE) - 1, 13);
		send_command(SET_DHCP, sizeof(SET_DHCP) -1, 13);
		send_command(SAVE, sizeof(SAVE) - 1, 27);
		osDelay(100);
		send_command(REBOOT, sizeof(REBOOT) - 1, 111);
		osDelay(300);
		//enter command mode
		HAL_UART_Transmit(&huart1, (uint8_t *) ENTER_COMMAND_MODE, sizeof(ENTER_COMMAND_MODE) - 1, 1000);
		rx_status = HAL_UART_Receive(&huart1, (uint8_t *) rx_buffer, 5, 500);
		int sizeofscan = send_scan_command();
		send_command(JOIN_WIFIND, sizeof(JOIN_WIFIND) - 1, 13);
		__HAL_UART_CLEAR_IT(&huart1, UART_CLEAR_OREF);
		__HAL_UART_SEND_REQ(&huart1, UART_RXDATA_FLUSH_REQUEST);
		osDelay(1000);
		send_command(CONNECT_TO_SERVER, sizeof(CONNECT_TO_SERVER) - 1, 13);
		__HAL_UART_CLEAR_IT(&huart1, UART_CLEAR_OREF);
		__HAL_UART_SEND_REQ(&huart1, UART_RXDATA_FLUSH_REQUEST);
		osDelay(300);
		HAL_UART_Transmit(&huart1, (uint8_t *) rx_buffer, sizeofscan, 1000);

				
		/*
		osDelay(300);
		send_command(ENTER_COMMAND_MODE, sizeof(ENTER_COMMAND_MODE) - 1);
		send_command(JOIN_WIFIND, sizeof(JOIN_WIFIND) - 1);
		osDelay(6000);
		send_command(CONNECT_TO_SERVER, sizeof(CONNECT_TO_SERVER) - 1);
		osDelay(300);
		send_command(TEST_COMMAND, sizeof(TEST_COMMAND) - 1);
		*/

//		HAL_UART_Transmit(&huart1, (uint8_t *) CONNECT_TO_SERVER, sizeof(CONNECT_TO_SERVER) - 1, 20);
//		rx_status = HAL_UART_Receive(&huart1, rx_buffer, 3, 100);
//		HAL_UART_Transmit(&huart1, (uint8_t *) EXIT_COMMAND_MODE, sizeof(EXIT_COMMAND_MODE) - 1, 20);
//		rx_status = HAL_UART_Receive(&huart1, rx_buffer, 4, 100);
//		HAL_Delay(10);
//
//		// Talk to server
//		HAL_UART_Transmit(&huart1, (uint8_t *) TEST_COMMAND, sizeof(TEST_COMMAND) - 1, 100);
//		// Close connection
//
//		HAL_Delay(300);
//		HAL_UART_Transmit(&huart1, (uint8_t *) ENTER_COMMAND_MODE, sizeof(ENTER_COMMAND_MODE) - 1, 20);
//		HAL_Delay(300);
//		rx_status = HAL_UART_Receive(&huart1, rx_buffer, 3, 100);
//		HAL_UART_Transmit(&huart1, (uint8_t *) DISCONNECT_FROM_SERVER, sizeof(DISCONNECT_FROM_SERVER) - 1, 20);
//		rx_status = HAL_UART_Receive(&huart1, rx_buffer, 3, 100);
//		HAL_UART_Transmit(&huart1, (uint8_t *) EXIT_COMMAND_MODE, sizeof(EXIT_COMMAND_MODE) - 1, 20);
//		rx_status = HAL_UART_Receive(&huart1, rx_buffer, 4, 100);
	}
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart1);

}
