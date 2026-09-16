#ifndef TEST_STM32F7XX_HAL_H
#define TEST_STM32F7XX_HAL_H
#include <stdint.h>
typedef struct { uint32_t counter; uint8_t enabled; } TIM_HandleTypeDef;
typedef struct { uint32_t Pin, Mode, Pull, Speed, Alternate; } GPIO_InitTypeDef;
typedef enum { HAL_OK, HAL_ERROR } HAL_StatusTypeDef;
#define GPIOB ((void *)1)
#define GPIO_PIN_4 16U
#define GPIO_PIN_RESET 0U
#define GPIO_MODE_OUTPUT_PP 1U
#define GPIO_MODE_AF_PP 2U
#define GPIO_NOPULL 0U
#define GPIO_SPEED_FREQ_LOW 0U
#define GPIO_AF2_TIM3 2U
#define TIM_CHANNEL_1 0U
#define __HAL_RCC_GPIOB_CLK_ENABLE() ((void)0)
void test_set_counter(TIM_HandleTypeDef *timer, uint32_t counter);
#define __HAL_TIM_SET_COUNTER(timer, value) test_set_counter(timer, value)
void HAL_GPIO_Init(void *port, GPIO_InitTypeDef *gpio);
void HAL_GPIO_WritePin(void *port, uint32_t pin, uint32_t value);
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *timer, uint32_t channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *timer, uint32_t channel);
#endif
