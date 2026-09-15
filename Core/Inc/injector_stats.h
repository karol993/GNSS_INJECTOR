#ifndef INJECTOR_STATS_H
#define INJECTOR_STATS_H

#include <stdint.h>
typedef struct {
  uint32_t nmea_sched;
  uint32_t nmea_tx_ok;
  uint32_t nmea_tx_err;
  uint32_t rmc_gen;
  uint32_t zda_gen;
  uint32_t cli_commands;
  uint32_t cli_errors;
} InjectorStats;
const InjectorStats *InjectorStats_Get(void);
void InjectorStats_Reset(void);
void InjectorStats_NmeaScheduled(void);
void InjectorStats_NmeaTxOk(void);
void InjectorStats_NmeaTxErr(void);
void InjectorStats_RmcGenerated(void);
void InjectorStats_ZdaGenerated(void);
void InjectorStats_CliCommand(void);
void InjectorStats_CliError(void);
#endif
