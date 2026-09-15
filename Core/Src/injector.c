#include "injector.h"
#include "nmea_gen.h"
#include "pps_gen.h"
#include "injector_stats.h"
#include <stdio.h>

typedef struct {
  UART_HandleTypeDef *control, *gnss; TIM_HandleTypeDef *reference;
  InjectorUtc epoch, nmea_epoch; float latitude, longitude; int32_t offset_ms;
  uint32_t pps_tick, nmea_tick; uint8_t running, pps, rmc, zda, nmea_sent;
} InjectorState;
static InjectorState state;
static uint8_t due(uint32_t now, uint32_t deadline) { return (uint8_t)((int32_t)(now - deadline) >= 0); }
static void schedule_epoch(uint32_t pps_tick) {
  state.pps_tick = pps_tick;
  state.nmea_tick = (uint32_t)(pps_tick + state.offset_ms * 1000);
  state.nmea_epoch = state.epoch;
  state.nmea_sent = 0U;
}
static char fix = 'A';
static void send_epoch_sentences(void) {
  char text[128]; int length;
  if (state.rmc) { length = NMEA_FormatRMC(text, sizeof text, &state.nmea_epoch, state.latitude, state.longitude, fix); if (length > 0) { (void)HAL_UART_Transmit(state.gnss, (uint8_t *)text, (uint16_t)length, 20U); InjectorStats_RmcSent(); } }
  if (state.zda) { length = NMEA_FormatZDA(text, sizeof text, &state.nmea_epoch); if (length > 0) { (void)HAL_UART_Transmit(state.gnss, (uint8_t *)text, (uint16_t)length, 20U); InjectorStats_ZdaSent(); } }
}
void INJECTOR_ResetDefaults(void) {
  state.epoch = (InjectorUtc){2026U, 9U, 15U, 10U, 0U, 0U}; state.latitude = 50.0f; state.longitude = 19.0f;
  state.offset_ms = 200; state.pps = 1U; state.rmc = 1U; state.zda = 0U; fix = 'A';
}
void INJECTOR_Init(UART_HandleTypeDef *control_uart, UART_HandleTypeDef *gnss_uart, TIM_HandleTypeDef *tim2, TIM_HandleTypeDef *pps_timer) {
  static const char startup_log[] = "GNSS INJECTOR V1\r\nMCU=STM32F767ZITx\r\nCONTROL_UART=USART3 PD8/PD9 115200\r\nGNSS_UART=USART2_TX PD5 115200\r\nPPS_OUT=TIM3_CH1 PB4\r\nTIM2=1MHz\r\nSTATE=STOPPED\r\n";
  state.control = control_uart; state.gnss = gnss_uart; state.reference = tim2; state.running = 0U; INJECTOR_ResetDefaults(); InjectorStats_Reset(); PPS_GenInit(pps_timer);
  (void)HAL_UART_Transmit(state.control, (uint8_t *)startup_log, (uint16_t)(sizeof(startup_log) - 1U), 100U);
}
void INJECTOR_Start(void) { uint32_t now = __HAL_TIM_GET_COUNTER(state.reference); if (state.running) return; state.running = 1U; schedule_epoch(now + 1000000U); }
void INJECTOR_Stop(void) { state.running = 0U; PPS_GenStop(); }
uint8_t INJECTOR_IsRunning(void) { return state.running; }
void INJECTOR_SetTime(const InjectorUtc *utc) { if (InjectorTime_IsValid(utc)) state.epoch = *utc; }
InjectorUtc INJECTOR_GetTime(void) { return state.epoch; }
void INJECTOR_SetPosition(float latitude, float longitude) { if (latitude >= -90.0f && latitude <= 90.0f && longitude >= -180.0f && longitude <= 180.0f) { state.latitude = latitude; state.longitude = longitude; } }
void INJECTOR_SetFix(char value) { if (value == 'A' || value == 'V') fix = value; }
void INJECTOR_SetPps(uint8_t enabled) { state.pps = enabled; if (!enabled) PPS_GenStop(); }
void INJECTOR_SetRmc(uint8_t enabled) { state.rmc = enabled; }
void INJECTOR_SetZda(uint8_t enabled) { state.zda = enabled; }
uint8_t INJECTOR_SetOffset(int32_t milliseconds) { if (milliseconds < -500 || milliseconds > 500) return 0U; state.offset_ms = milliseconds; return 1U; }
int32_t INJECTOR_GetOffset(void) { return state.offset_ms; }
void INJECTOR_Status(char *out, uint32_t out_size) { (void)snprintf(out, out_size, "INJ,STATUS,TIME=%04u-%02u-%02uT%02u:%02u:%02u,FIX=%c,PPS=%s,RMC=%s,ZDA=%s,OFFSET=%ld,RUN=%s\r\n", state.epoch.year,state.epoch.month,state.epoch.day,state.epoch.hour,state.epoch.minute,state.epoch.second,fix,state.pps?"ON":"OFF",state.rmc?"ON":"OFF",state.zda?"ON":"OFF",(long)state.offset_ms,state.running?"ON":"OFF"); }
void INJECTOR_Task(void) {
  uint32_t now; if (!state.running) return; now = __HAL_TIM_GET_COUNTER(state.reference);
  if (!state.nmea_sent && due(now, state.nmea_tick)) { send_epoch_sentences(); state.nmea_sent = 1U; }
  if (due(now, state.pps_tick)) { if (state.pps) PPS_GenStart(); InjectorTime_IncrementSecond(&state.epoch); schedule_epoch(state.pps_tick + 1000000U); }
}
