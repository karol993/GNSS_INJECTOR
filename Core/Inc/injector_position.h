#ifndef INJECTOR_POSITION_H
#define INJECTOR_POSITION_H

#include <stdint.h>

uint8_t InjectorPosition_ParseDecimal(const char *token, int32_t *microdegrees);
uint8_t InjectorPosition_ParsePair(char *arguments, int32_t *latitude_udeg,
                                   int32_t *longitude_udeg);
void InjectorPosition_Format(char *out, uint32_t out_size, int32_t microdegrees);

#endif
