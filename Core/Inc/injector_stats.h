#ifndef INJECTOR_STATS_H
#define INJECTOR_STATS_H

#include <stdint.h>
typedef struct {
  uint32_t nmea_sched;
  uint32_t nmea_tx_ok;
  uint32_t nmea_tx_err;
  uint32_t rmc_gen;
  uint32_t zda_gen;
  uint32_t lc29h_burst_gen;
  uint32_t lc29h_lines_gen;
  uint32_t lc29h_tx_ok;
  uint32_t lc29h_tx_err;
  uint32_t last_burst_bytes;
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
void InjectorStats_Lc29hBurst(uint32_t lines, uint32_t bytes);
void InjectorStats_Lc29hTxOk(void);
void InjectorStats_Lc29hTxErr(void);
void InjectorStats_CliCommand(void);
void InjectorStats_CliError(void);
#endif
