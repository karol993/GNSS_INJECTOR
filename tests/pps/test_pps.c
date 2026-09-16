#include <assert.h>
#include <stdio.h>
#include "pps_gen.h"

/* Model the configured 10 kHz PWM: ARR=9999, CCR1=1000.
 * Re-enter Start just before and after automatic overflow, as a main-loop
 * scheduler with variable latency does. Count physical rising edges.
 */
static TIM_HandleTypeDef timer;
static uint32_t gpio_mode, gpio_latch, pin_level, rising_edges;
static uint32_t start_calls, stop_calls;
static uint8_t fail_start;

static void update_pin(void)
{
  uint32_t next = gpio_mode == GPIO_MODE_AF_PP
    ? (uint32_t)(timer.enabled && timer.counter < 1000U) : gpio_latch;
  if (next && !pin_level) ++rising_edges;
  pin_level = next;
}
void HAL_GPIO_Init(void *port, GPIO_InitTypeDef *gpio)
{
  (void)port; gpio_mode = gpio->Mode; update_pin();
}
void HAL_GPIO_WritePin(void *port, uint32_t pin, uint32_t value)
{
  (void)port; (void)pin; gpio_latch = value; update_pin();
}
void test_set_counter(TIM_HandleTypeDef *handle, uint32_t counter)
{
  handle->counter = counter; update_pin();
}
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *handle, uint32_t channel)
{
  (void)channel; ++start_calls;
  if (fail_start) return HAL_ERROR;
  handle->enabled = 1U; update_pin(); return HAL_OK;
}
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *handle, uint32_t channel)
{
  (void)channel; ++stop_calls;
  handle->enabled = 0U; update_pin(); return HAL_OK;
}
static void advance(uint32_t ticks)
{
  while (ticks--) {
    if (timer.enabled) timer.counter = (timer.counter + 1U) % 10000U;
    update_pin();
  }
}
int main(void)
{
  PPS_GenStart(); /* Safe before initialization. */
  PPS_GenInit(&timer);
  PPS_GenStart();
  assert(rising_edges == 1U && pin_level == 1U);
  uint32_t stops = stop_calls;
  advance(999U); assert(pin_level == 1U);
  advance(1U); assert(pin_level == 0U);
  advance(8999U);
  PPS_GenStart(); /* Scheduler slightly early: no premature pulse. */
  assert(rising_edges == 1U && timer.counter == 9999U);
  advance(2U);
  assert(rising_edges == 2U && pin_level == 1U);
  PPS_GenStart(); /* Scheduler late: hardware already raised PPS. */
  assert(rising_edges == 2U && timer.counter == 1U);
  assert(start_calls == 1U && stop_calls == stops);
  for (unsigned i = 0U; i < 180U; ++i) {
    advance(10000U);
    PPS_GenStart();
    assert(rising_edges == i + 3U);
  }
  PPS_GenStop(); assert(pin_level == 0U);
  uint32_t edges = rising_edges;
  advance(20000U); assert(rising_edges == edges);
  PPS_GenStart(); assert(rising_edges == edges + 1U);
  advance(1000U); assert(pin_level == 0U);
  PPS_GenStop();
  fail_start = 1U;
  PPS_GenStart(); assert(pin_level == 0U);
  fail_start = 0U;
  PPS_GenStart(); assert(pin_level == 1U);
  puts("PASS: one PPS edge per second, 100 ms width, stop/restart and start retry");
  return 0;
}
