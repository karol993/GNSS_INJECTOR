#ifndef NMEA_GEN_H
#define NMEA_GEN_H

#include <stddef.h>
#include <stdint.h>
#include "injector_time.h"

uint8_t NMEA_Checksum(const char *body);
int NMEA_FormatRMC(char *out, size_t out_size, const InjectorUtc *utc,
                   float latitude, float longitude, char fix);
int NMEA_FormatZDA(char *out, size_t out_size, const InjectorUtc *utc);

#endif
