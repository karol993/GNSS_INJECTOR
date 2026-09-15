#include "injector_stats.h"
static InjectorStats stats;
const InjectorStats *InjectorStats_Get(void) { return &stats; }
void InjectorStats_Reset(void) { stats = (InjectorStats){0}; }
void InjectorStats_NmeaScheduled(void) { ++stats.nmea_sched; }
void InjectorStats_NmeaTxOk(void) { ++stats.nmea_tx_ok; }
void InjectorStats_NmeaTxErr(void) { ++stats.nmea_tx_err; }
void InjectorStats_RmcGenerated(void) { ++stats.rmc_gen; }
void InjectorStats_ZdaGenerated(void) { ++stats.zda_gen; }
void InjectorStats_CliCommand(void) { ++stats.cli_commands; }
void InjectorStats_CliError(void) { ++stats.cli_errors; }
