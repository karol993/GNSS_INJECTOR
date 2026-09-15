#include "injector_position.h"
#include <stdio.h>

uint8_t InjectorPosition_ParseDecimal(const char *token, int32_t *microdegrees)
{
  const char *cursor = token;
  uint32_t whole = 0U;
  uint32_t fraction = 0U;
  uint32_t digits = 0U;
  uint8_t negative = 0U;

  if (token == 0 || microdegrees == 0) return 0U;
  if (*cursor == '+' || *cursor == '-') { negative = (uint8_t)(*cursor == '-'); ++cursor; }
  if (*cursor < '0' || *cursor > '9') return 0U;
  while (*cursor >= '0' && *cursor <= '9') {
    if (whole > 2147U) return 0U;
    whole = whole * 10U + (uint32_t)(*cursor++ - '0');
  }
  if (*cursor == '.') {
    ++cursor;
    while (*cursor >= '0' && *cursor <= '9') {
      if (digits >= 6U) return 0U;
      fraction = fraction * 10U + (uint32_t)(*cursor++ - '0');
      ++digits;
    }
  }
  if (*cursor != '\0') return 0U;
  while (digits++ < 6U) fraction *= 10U;
  if (whole > 2147U || (whole == 2147U && fraction > 483647U)) return 0U;
  *microdegrees = (int32_t)(whole * 1000000U + fraction);
  if (negative) *microdegrees = -*microdegrees;
  if (*microdegrees == 0) *microdegrees = 0;
  return 1U;
}

static char *next_token(char **cursor)
{
  char *start;
  while (**cursor == ' ') ++*cursor;
  if (**cursor == '\0') return 0;
  start = *cursor;
  while (**cursor != '\0' && **cursor != ' ') ++*cursor;
  if (**cursor != '\0') *(*cursor)++ = '\0';
  return start;
}

uint8_t InjectorPosition_ParsePair(char *arguments, int32_t *latitude_udeg,
                                   int32_t *longitude_udeg)
{
  char *latitude;
  char *longitude;
  char *extra;
  if (arguments == 0 || latitude_udeg == 0 || longitude_udeg == 0) return 0U;
  latitude = next_token(&arguments);
  longitude = next_token(&arguments);
  extra = next_token(&arguments);
  if (latitude == 0 || longitude == 0 || extra != 0) return 0U;
  if (!InjectorPosition_ParseDecimal(latitude, latitude_udeg) ||
      !InjectorPosition_ParseDecimal(longitude, longitude_udeg)) return 0U;
  return (uint8_t)(*latitude_udeg >= -90000000 && *latitude_udeg <= 90000000 &&
                   *longitude_udeg >= -180000000 && *longitude_udeg <= 180000000);
}

void InjectorPosition_Format(char *out, uint32_t out_size, int32_t microdegrees)
{
  if (microdegrees == 0) microdegrees = 0;
  uint32_t magnitude = (uint32_t)(microdegrees < 0 ? -microdegrees : microdegrees);
  (void)snprintf(out, out_size, "%s%lu.%06lu", microdegrees < 0 ? "-" : "",
                 (unsigned long)(magnitude / 1000000U),
                 (unsigned long)(magnitude % 1000000U));
}
