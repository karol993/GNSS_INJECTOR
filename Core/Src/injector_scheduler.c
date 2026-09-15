#include "injector_scheduler.h"

void InjectorScheduler_Schedule(InjectorScheduler *scheduler, uint32_t pps_tick,
                                int32_t offset_ms)
{
  scheduler->pps_tick = pps_tick;
  scheduler->nmea_tick = (uint32_t)(pps_tick + offset_ms * 1000);
  scheduler->pps_fired = 0U;
  scheduler->nmea_sent = 0U;
}

uint8_t InjectorScheduler_Due(uint32_t now, uint32_t deadline)
{
  return (uint8_t)((int32_t)(now - deadline) >= 0);
}

uint8_t InjectorScheduler_PpsDue(const InjectorScheduler *scheduler, uint32_t now)
{
  return (uint8_t)(scheduler->pps_fired == 0U &&
                   InjectorScheduler_Due(now, scheduler->pps_tick));
}

uint8_t InjectorScheduler_NmeaDue(const InjectorScheduler *scheduler, uint32_t now)
{
  return (uint8_t)(scheduler->nmea_sent == 0U &&
                   InjectorScheduler_Due(now, scheduler->nmea_tick));
}

uint8_t InjectorScheduler_ReadyForNext(const InjectorScheduler *scheduler)
{
  return (uint8_t)(scheduler->pps_fired != 0U && scheduler->nmea_sent != 0U);
}
