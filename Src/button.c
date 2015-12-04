/**
  ******************************************************************************
  * File Name          : button.c
  * Description        : Thread that determines if button has been long pushed or not
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 EECS 473 2015 Fall WiFind Group
  */
#include "stm32l0xx_hal.h"
#include "cmsis_os.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include <string.h>
#include <stdbool.h>
#include <uart.h>

xSemaphoreHandle raw_button_sem;
bool button_gb_short_push;

// public function
void button_thread(void const *argument) {
    raw_button_sem = xSemaphoreCreateBinary();
		portTickType current = 0;
    for (;;) {
        // wait forever for a falling edge
			
        xSemaphoreTake(raw_button_sem, portMAX_DELAY);
			  portTickType temp = xTaskGetTickCount();
				if ((temp - current) * portTICK_RATE_MS < 1000) {
					current = temp;
					continue;
				}
				current = temp;
        if (!uart_gb_message_processing) {
            // do the following only if the uart thread is not processing a push
            // button message
            button_gb_short_push = false;

            for (int i = 0; i < 3; i++) {
                // poll the level of PA0 to see if it stays high or not
                // TODO: change the following to GPIO_PIN_SET because the board
                // use logic low instead of logic high for button push
                osDelay(500);
                if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
                    // if level changs to high, declare it as a short push
                    button_gb_short_push = true;
                    break;
                }
            }
            if (button_gb_short_push) {
                uart_gb_cancel = false;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
            } else {
                uart_gb_cancel = true;
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
            }
            xSemaphoreGive(button_sem);
        }
    }
}

