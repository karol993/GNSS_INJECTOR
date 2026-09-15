#ifndef INJECTOR_H
#define INJECTOR_H

#include "stm32f7xx_hal.h"
#include "injector_time.h"

void INJECTOR_Init(UART_HandleTypeDef *control_uart, UART_HandleTypeDef *gnss_uart,
                   TIM_HandleTypeDef *tim2, TIM_HandleTypeDef *pps_timer);
void INJECTOR_Task(void);
void INJECTOR_Start(void);
void INJECTOR_Stop(void);
uint8_t INJECTOR_IsRunning(void);
void INJECTOR_SetTime(const InjectorUtc *utc);
InjectorUtc INJECTOR_GetTime(void);
void INJECTOR_SetPosition(float latitude, float longitude);
void INJECTOR_SetFix(char fix);
void INJECTOR_SetPps(uint8_t enabled);
void INJECTOR_SetRmc(uint8_t enabled);
void INJECTOR_SetZda(uint8_t enabled);
uint8_t INJECTOR_SetOffset(int32_t milliseconds);
int32_t INJECTOR_GetOffset(void);
void INJECTOR_Status(char *out, uint32_t out_size);
void INJECTOR_ResetDefaults(void);

#endif
