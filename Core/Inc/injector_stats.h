#ifndef INJECTOR_STATS_H
#define INJECTOR_STATS_H

#include <stdint.h>
typedef struct { uint32_t rmc_sent, zda_sent, cli_commands, cli_errors; } InjectorStats;
const InjectorStats *InjectorStats_Get(void);
void InjectorStats_Reset(void);
void InjectorStats_RmcSent(void);
void InjectorStats_ZdaSent(void);
void InjectorStats_CliCommand(void);
void InjectorStats_CliError(void);
#endif
