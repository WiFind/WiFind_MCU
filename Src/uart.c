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

UART_HandleTypeDef huart1;
static const uint8_t TEST_COMMAND [] = "Hello World!";
static const uint8_t ENTER_COMMAND_MODE [] = "$$$";
static const uint8_t EXIT_COMMAND_MODE [] = "exit";
static const uint8_t CONNECT_TO_SERVER [] = "open 192.168.1.4 8000";
static const uint8_t DISCONNECT_FROM_SERVER [] = "close";
static const uint8_t SET_AUTHENTICATION [] = "set wlan auth 4";
static const uint8_t SET_SSID [] = "set wlan ssid WiFind";
static const uint8_t SET_PASSPHRASE [] = "set wlan pass WiFind11";
static const uint8_t SET_JOIN_MODE [] = "set wlan join 1";
static const uint8_t SAVE [] = "save";
static const uint8_t REBOOT [] = "reboot";
static uint8_t rx_buffer [100];
// private function protoypes
static void MX_USART1_UART_Init(void);

void send_command(const uint8_t * command, size_t size) {
	HAL_StatusTypeDef rx_status;
	HAL_UART_Transmit(&huart1, (uint8_t *) command, size, 1000000);
	//rx_status = HAL_UART_Receive(&huart1, rx_buffer, 3, 5);
}

// public function
void uart_thread(void const *argument) {
	// This function assumes that wireless addresses and such have been set already.
  MX_USART1_UART_Init();
	HAL_StatusTypeDef rx_status;
	
	for (;;) {
		osSignalWait(UART_SEND_COMMAND_SIGNAL, osWaitForever);
		//HAL_Delay(300);
		osDelay(300);
		HAL_UART_Transmit(&huart1, (uint8_t *) ENTER_COMMAND_MODE, sizeof(ENTER_COMMAND_MODE) - 1, 1000);
		//osDelay(1000);
		rx_status = HAL_UART_Receive(&huart1, (uint8_t *) rx_buffer, 5, 2000);
		huart1.State = HAL_UART_STATE_READY;
		HAL_UART_Transmit(&huart1, (uint8_t *) TEST_COMMAND, sizeof(TEST_COMMAND) - 1, 1000); 
		osDelay(200);
		//send_command(SET_AUTHENTICATION, sizeof(SET_AUTHENTICATION) - 1);
		// Enter command mode
//		HAL_Delay(300);
//		HAL_UART_Transmit(&huart1, (uint8_t *) ENTER_COMMAND_MODE, sizeof(ENTER_COMMAND_MODE) - 1, 20);
//		HAL_Delay(300);
//		rx_status = HAL_UART_Receive(&huart1, rx_buffer, 3, 5);
//		
//		send_command(SET_AUTHENTICATION, sizeof(SET_AUTHENTICATION) - 1);
//		send_command(SET_SSID, sizeof(SET_SSID) - 1);
//		send_command(SET_PASSPHRASE, sizeof(SET_PASSPHRASE) - 1);
//		send_command(SET_JOIN_MODE, sizeof(SET_JOIN_MODE) - 1);
//		send_command(SAVE, sizeof(SAVE) - 1);
//		send_command(REBOOT, sizeof(REBOOT) - 1);
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
		HAL_UART_Transmit(&huart1, (uint8_t *) EXIT_COMMAND_MODE, sizeof(EXIT_COMMAND_MODE) - 1, 20);
		rx_status = HAL_UART_Receive(&huart1, rx_buffer, 4, 100);
		osSignalClear(osThreadGetId(), UART_SEND_COMMAND_SIGNAL);
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
