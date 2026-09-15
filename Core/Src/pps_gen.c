#include "pps_gen.h"
#include "main.h"
static TIM_HandleTypeDef *pps_timer;
void PPS_GenInit(TIM_HandleTypeDef *timer) { pps_timer = timer; PPS_GenStop(); }
void PPS_GenStart(void) { GPIO_InitTypeDef gpio = {0}; if (pps_timer != 0) { __HAL_RCC_GPIOB_CLK_ENABLE(); gpio.Pin = PPS_OUT_Pin; gpio.Mode = GPIO_MODE_AF_PP; gpio.Pull = GPIO_NOPULL; gpio.Speed = GPIO_SPEED_FREQ_LOW; gpio.Alternate = GPIO_AF2_TIM3; HAL_GPIO_Init(PPS_OUT_GPIO_Port, &gpio); HAL_TIM_PWM_Stop(pps_timer, TIM_CHANNEL_1); __HAL_TIM_SET_COUNTER(pps_timer, 0U); HAL_TIM_PWM_Start(pps_timer, TIM_CHANNEL_1); } }
void PPS_GenStop(void) { GPIO_InitTypeDef gpio = {0}; if (pps_timer != 0) (void)HAL_TIM_PWM_Stop(pps_timer, TIM_CHANNEL_1); __HAL_RCC_GPIOB_CLK_ENABLE(); HAL_GPIO_WritePin(PPS_OUT_GPIO_Port, PPS_OUT_Pin, GPIO_PIN_RESET); gpio.Pin = PPS_OUT_Pin; gpio.Mode = GPIO_MODE_OUTPUT_PP; gpio.Pull = GPIO_NOPULL; gpio.Speed = GPIO_SPEED_FREQ_LOW; HAL_GPIO_Init(PPS_OUT_GPIO_Port, &gpio); }
