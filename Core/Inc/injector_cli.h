#ifndef INJECTOR_CLI_H
#define INJECTOR_CLI_H

#include "stm32f7xx_hal.h"
void INJECTOR_CLI_Init(UART_HandleTypeDef *uart);
void INJECTOR_CLI_Task(void);

#endif
