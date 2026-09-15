#ifndef INJECTOR_TIME_H
#define INJECTOR_TIME_H

#include <stdint.h>

typedef struct {
  uint16_t year;
  uint8_t month, day, hour, minute, second;
} InjectorUtc;

uint8_t InjectorTime_IsLeapYear(uint16_t year);
uint8_t InjectorTime_DaysInMonth(uint16_t year, uint8_t month);
uint8_t InjectorTime_IsValid(const InjectorUtc *utc);
void InjectorTime_IncrementSecond(InjectorUtc *utc);

#endif
