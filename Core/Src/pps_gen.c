#include "pps_gen.h"
#include "main.h"
static TIM_HandleTypeDef *pps_timer;
static uint8_t pps_running;

void PPS_GenInit(TIM_HandleTypeDef *timer)
{
  pps_timer = timer;
  PPS_GenStop();
}

void PPS_GenStart(void)
{
  GPIO_InitTypeDef gpio = {0};
  /* TIM3 repeats the 1 Hz waveform in hardware. The scheduler calls here
   * every second, possibly just after TIM3 has already raised the pin.
   * Stopping/restarting PWM then creates an extra rising edge and advances
   * the downstream GPS-to-DCF clock twice. Leave an active timer untouched.
   */
  if (pps_timer == 0 || pps_running) return;

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_TIM_SET_COUNTER(pps_timer, 0U);
  if (HAL_TIM_PWM_Start(pps_timer, TIM_CHANNEL_1) != HAL_OK) return;

  gpio.Pin = PPS_OUT_Pin;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  gpio.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(PPS_OUT_GPIO_Port, &gpio);
  pps_running = 1U;
}

void PPS_GenStop(void)
{
  GPIO_InitTypeDef gpio = {0};
  if (pps_timer != 0) (void)HAL_TIM_PWM_Stop(pps_timer, TIM_CHANNEL_1);
  pps_running = 0U;
  __HAL_RCC_GPIOB_CLK_ENABLE();
  HAL_GPIO_WritePin(PPS_OUT_GPIO_Port, PPS_OUT_Pin, GPIO_PIN_RESET);
  gpio.Pin = PPS_OUT_Pin;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PPS_OUT_GPIO_Port, &gpio);
}
