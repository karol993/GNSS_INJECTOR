#ifndef NMEA_GEN_H
#define NMEA_GEN_H

#include <stddef.h>
#include <stdint.h>
#include "injector_time.h"

uint8_t NMEA_Checksum(const char *body);
int NMEA_FormatRMC(char *out, size_t out_size, const InjectorUtc *utc,
                   int32_t latitude_udeg, int32_t longitude_udeg, char fix);
int NMEA_FormatZDA(char *out, size_t out_size, const InjectorUtc *utc);
int NMEA_FormatLC29HBurst(char *out, size_t out_size, const InjectorUtc *utc,
                           int32_t latitude_udeg, int32_t longitude_udeg,
                           char fix, uint32_t *line_count);

#endif
