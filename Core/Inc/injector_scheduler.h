#ifndef INJECTOR_SCHEDULER_H
#define INJECTOR_SCHEDULER_H

#include <stdint.h>

typedef struct {
  uint32_t pps_tick;
  uint32_t nmea_tick;
  uint8_t pps_fired;
  uint8_t nmea_sent;
} InjectorScheduler;

void InjectorScheduler_Schedule(InjectorScheduler *scheduler, uint32_t pps_tick,
                                int32_t offset_ms);
uint8_t InjectorScheduler_Due(uint32_t now, uint32_t deadline);
uint8_t InjectorScheduler_PpsDue(const InjectorScheduler *scheduler, uint32_t now);
uint8_t InjectorScheduler_NmeaDue(const InjectorScheduler *scheduler, uint32_t now);
uint8_t InjectorScheduler_ReadyForNext(const InjectorScheduler *scheduler);

#endif
