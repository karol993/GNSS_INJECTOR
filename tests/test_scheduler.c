#include <assert.h>
#include "injector_scheduler.h"

int main(void)
{
  InjectorScheduler scheduler;

  /* START at tick zero, epoch T, OFFSET=+200 ms. */
  InjectorScheduler_Schedule(&scheduler, 1000000U, 200);
  assert(!InjectorScheduler_NmeaDue(&scheduler, 1199000U));
  assert(InjectorScheduler_PpsDue(&scheduler, 1000000U));
  scheduler.pps_fired = 1U;
  assert(!InjectorScheduler_NmeaDue(&scheduler, 1199000U));
  assert(InjectorScheduler_NmeaDue(&scheduler, 1200000U));
  scheduler.nmea_sent = 1U;
  assert(InjectorScheduler_ReadyForNext(&scheduler));
  InjectorScheduler_Schedule(&scheduler, scheduler.pps_tick + 1000000U, 200);
  assert(scheduler.pps_tick == 2000000U);
  assert(scheduler.nmea_tick == 2200000U);
  assert(!InjectorScheduler_NmeaDue(&scheduler, 1500000U));
  assert(InjectorScheduler_PpsDue(&scheduler, 2000000U));
  scheduler.pps_fired = 1U;
  assert(InjectorScheduler_NmeaDue(&scheduler, 2200000U));

  /* OFFSET=-200 ms: send T before its PPS, then schedule T+1 exactly once. */
  InjectorScheduler_Schedule(&scheduler, 1000000U, -200);
  assert(!InjectorScheduler_NmeaDue(&scheduler, 799000U));
  assert(InjectorScheduler_NmeaDue(&scheduler, 800000U));
  scheduler.nmea_sent = 1U;
  assert(!InjectorScheduler_ReadyForNext(&scheduler));
  assert(InjectorScheduler_PpsDue(&scheduler, 1000000U));
  scheduler.pps_fired = 1U;
  assert(InjectorScheduler_ReadyForNext(&scheduler));
  InjectorScheduler_Schedule(&scheduler, scheduler.pps_tick + 1000000U, -200);
  assert(scheduler.pps_tick == 2000000U);
  assert(scheduler.nmea_tick == 1800000U);
  return 0;
}
