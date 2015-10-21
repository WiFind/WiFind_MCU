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
static uint8_t rx_buffer [20];
// private function protoypes
static void MX_USART1_UART_Init(void);

// public function
void uart_thread(void const *argument) {
  MX_USART1_UART_Init();
	HAL_StatusTypeDef rx_status;
	
	for (;;) {
		osSignalWait(UART_SEND_COMMAND_SIGNAL, osWaitForever);
		HAL_UART_Transmit(&huart1, (uint8_t *) ENTER_COMMAND_MODE, sizeof(ENTER_COMMAND_MODE) - 1, 20);  
		rx_status = HAL_UART_Receive(&huart1, rx_buffer, 3, 5);
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
