#include "injector_stats.h"
static InjectorStats stats;
const InjectorStats *InjectorStats_Get(void) { return &stats; }
void InjectorStats_Reset(void) { stats = (InjectorStats){0}; }
void InjectorStats_RmcSent(void) { ++stats.rmc_sent; }
void InjectorStats_ZdaSent(void) { ++stats.zda_sent; }
void InjectorStats_CliCommand(void) { ++stats.cli_commands; }
void InjectorStats_CliError(void) { ++stats.cli_errors; }
