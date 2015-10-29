#include <string.h>
#include <stdio.h>

#include "wifi_driver.h"

#include "cmsis_os.h"

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_uart.h"

#define RX_BUF_SIZE 1000

static const uint8_t TEST_COMMAND [] = "Hello World!";
static const uint8_t ENTER_COMMAND_MODE [] = "$$$";
static const uint8_t EXIT_COMMAND_MODE [] = "exit\r\n";

static const uint8_t CONNECT_TO_WIFI [] = "join %s\r\n";

static const uint8_t CONNECT_TO_SERVER [] = "open %s %s\r\n";
//static const uint8_t CONNECT_TO_SERVER [] = "open 192.168.1.4 8000\r\n";
static const uint8_t DISCONNECT_FROM_SERVER [] = "close\r\n";

static const uint8_t SET_SERVER_IP [] = "set ip host %s\r\n";
static const uint8_t SET_SERVER_PORT [] = "set ip remote %s\r\n";
static const uint8_t SET_AUTHENTICATION [] = "set wlan auth 4\r\n";
static const uint8_t SET_PASSPHRASE [] = "set wlan pass %s\r\n";
//static const uint8_t SET_PASSPHRASE [] = "set wlan pass WiFind11\r\n";
static const uint8_t SET_JOIN_MODE [] = "set wlan join 0\r\n";

static const uint8_t SAVE [] = "save\r\n";
static const uint8_t REBOOT [] = "reboot\r\n";

UART_HandleTypeDef huart1;
uint8_t rx_buffer[RX_BUF_SIZE];

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

void init_wifi() {
	uint8_t pass_cmd[50];
	uint8_t ip_cmd[50];
	uint8_t port_cmd[50];
	
	MX_USART1_UART_Init();
	enter_command_mode();
	set_command(SET_AUTHENTICATION, sizeof(SET_AUTHENTICATION) - 1, 100);
	set_command(SET_JOIN_MODE, sizeof(SET_JOIN_MODE) - 1, 100);
	set_command(SAVE, sizeof(SAVE) - 1, 100);
	//send_command(REBOOT, sizeof(REBOOT) - 1, 1000);
}

static void reinit_uart() {
	// Reset the UART device, including timeout flags.
	MX_USART1_UART_Init();
}

static void clear_buffer() {
	memset(rx_buffer, 0, sizeof (rx_buffer));
}

void set_command(const uint8_t* command, uint8_t size, uint32_t timeout) {
	HAL_StatusTypeDef rx_status;
	HAL_UART_Transmit(&huart1, (uint8_t *) command, size, 100);
	rx_status = HAL_UART_Receive(&huart1, rx_buffer, 5, timeout);
	if (rx_status == HAL_TIMEOUT) reinit_uart();
}

void enter_command_mode() {
	osDelay(300);
	HAL_UART_Transmit(&huart1, (uint8_t *) ENTER_COMMAND_MODE, sizeof(ENTER_COMMAND_MODE) - 1, 1000);
	
}

void exit_command_mode() {
	HAL_StatusTypeDef rx_status;

	HAL_UART_Transmit(&huart1, (uint8_t *) ENTER_COMMAND_MODE, sizeof(ENTER_COMMAND_MODE) - 1, 100);
	rx_status = HAL_UART_Receive(&huart1, (uint8_t *) rx_buffer, 5, 1000);
}

void scan_wifi() {
	const char scan_cmd [] = "scan\r\n";
	enter_command_mode();
	HAL_UART_Transmit(&huart1, (uint8_t *)scan_cmd, sizeof(scan_cmd), 100);
	HAL_UART_Receive(&huart1, rx_buffer, 999, 3000);
	reinit_uart();
	exit_command_mode();
}

void connect_to_wifi(uint8_t *ssid, uint8_t *pass) {
	uint8_t pass_cmd[50];
	uint8_t join_cmd[50];
	
	sprintf((char *) pass_cmd, (const char *)SET_PASSPHRASE, pass);
	sprintf((char *) join_cmd, (const char *)CONNECT_TO_WIFI, ssid);
	enter_command_mode();
	send_command(pass_cmd, strlen((char *)pass_cmd), 100);
	send_command(join_cmd, strlen((char *)join_cmd), 10000);
	clear_buffer();
	exit_command_mode();
}

void connect_to_server(uint8_t *ip, uint8_t *port) {
	uint8_t open_cmd[50];
	
}

void reboot() {
	
}
