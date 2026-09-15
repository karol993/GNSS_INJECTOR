#include "injector_time.h"

uint8_t InjectorTime_IsLeapYear(uint16_t year) { return (uint8_t)(((year % 4U == 0U) && (year % 100U != 0U)) || (year % 400U == 0U)); }
uint8_t InjectorTime_DaysInMonth(uint16_t year, uint8_t month) {
  static const uint8_t days[] = { 31U,28U,31U,30U,31U,30U,31U,31U,30U,31U,30U,31U };
  if (month < 1U || month > 12U) return 0U;
  return (uint8_t)((month == 2U && InjectorTime_IsLeapYear(year)) ? 29U : days[month - 1U]);
}
uint8_t InjectorTime_IsValid(const InjectorUtc *utc) {
  return (uint8_t)(utc != 0 && utc->year >= 2000U && utc->month >= 1U && utc->month <= 12U && utc->day >= 1U && utc->day <= InjectorTime_DaysInMonth(utc->year, utc->month) && utc->hour < 24U && utc->minute < 60U && utc->second < 60U);
}
void InjectorTime_IncrementSecond(InjectorUtc *utc) {
  if (utc == 0) return;
  if (++utc->second < 60U) return;
  utc->second = 0U; if (++utc->minute < 60U) return;
  utc->minute = 0U; if (++utc->hour < 24U) return;
  utc->hour = 0U; if (++utc->day <= InjectorTime_DaysInMonth(utc->year, utc->month)) return;
  utc->day = 1U; if (++utc->month <= 12U) return;
  utc->month = 1U; ++utc->year;
}
