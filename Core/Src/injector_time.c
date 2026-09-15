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
static uint8_t parse_digits(const char *text, uint8_t count, uint16_t *value) {
  uint16_t result = 0U;
  uint8_t index;
  for (index = 0U; index < count; ++index) {
    if (text[index] < '0' || text[index] > '9') return 0U;
    result = (uint16_t)(result * 10U + (uint16_t)(text[index] - '0'));
  }
  *value = result;
  return 1U;
}
uint8_t InjectorTime_Parse(const char *text, InjectorUtc *utc) {
  uint16_t year, month, day, hour, minute, second;
  if (text == 0 || utc == 0 || text[19] != '\0' || text[4] != '-' ||
      text[7] != '-' || (text[10] != ' ' && text[10] != 'T') ||
      text[13] != ':' || text[16] != ':') return 0U;
  if (!parse_digits(text, 4U, &year) || !parse_digits(text + 5, 2U, &month) ||
      !parse_digits(text + 8, 2U, &day) || !parse_digits(text + 11, 2U, &hour) ||
      !parse_digits(text + 14, 2U, &minute) || !parse_digits(text + 17, 2U, &second)) return 0U;
  utc->year = year; utc->month = (uint8_t)month; utc->day = (uint8_t)day;
  utc->hour = (uint8_t)hour; utc->minute = (uint8_t)minute; utc->second = (uint8_t)second;
  return InjectorTime_IsValid(utc);
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
