#include "nmea_gen.h"
#include <stdio.h>

uint8_t NMEA_Checksum(const char *body) { uint8_t sum = 0U; while (*body != '\0') sum ^= (uint8_t)*body++; return sum; }
static void format_coordinate(char *out, size_t size, int32_t value_udeg, uint8_t degrees, char *hemisphere, char positive, char negative) {
  if (value_udeg == 0) value_udeg = 0;
  uint32_t magnitude = (uint32_t)(value_udeg < 0 ? -value_udeg : value_udeg);
  uint32_t deg = magnitude / 1000000U;
  uint32_t minute_10000 = (uint32_t)(((uint64_t)(magnitude % 1000000U) * 600000U + 500000U) / 1000000U);
  *hemisphere = value_udeg < 0 ? negative : positive;
  if (minute_10000 >= 600000U) { ++deg; minute_10000 = 0U; }
  (void)snprintf(out, size, degrees == 2U ? "%02lu%02lu.%04lu" : "%03lu%02lu.%04lu",
                 (unsigned long)deg, (unsigned long)(minute_10000 / 10000U), (unsigned long)(minute_10000 % 10000U));
}
int NMEA_FormatRMC(char *out, size_t out_size, const InjectorUtc *utc, int32_t latitude_udeg, int32_t longitude_udeg, char fix) {
  char lat[16], lon[16], ns, ew, body[100]; uint8_t sum;
  if (out == 0 || !InjectorTime_IsValid(utc) || (fix != 'A' && fix != 'V')) return -1;
  format_coordinate(lat, sizeof lat, latitude_udeg, 2U, &ns, 'N', 'S'); format_coordinate(lon, sizeof lon, longitude_udeg, 3U, &ew, 'E', 'W');
  (void)snprintf(body, sizeof body, "GNRMC,%02u%02u%02u.00,%c,%s,%c,%s,%c,0.0,0.0,%02u%02u%02u,,,A", utc->hour, utc->minute, utc->second, fix, lat, ns, lon, ew, utc->day, utc->month, (unsigned)(utc->year % 100U));
  sum = NMEA_Checksum(body); return snprintf(out, out_size, "$%s*%02X\r\n", body, sum);
}
int NMEA_FormatZDA(char *out, size_t out_size, const InjectorUtc *utc) {
  char body[64]; uint8_t sum; if (out == 0 || !InjectorTime_IsValid(utc)) return -1;
  (void)snprintf(body, sizeof body, "GNZDA,%02u%02u%02u.00,%02u,%02u,%04u,00,00", utc->hour, utc->minute, utc->second, utc->day, utc->month, utc->year);
  sum = NMEA_Checksum(body); return snprintf(out, out_size, "$%s*%02X\r\n", body, sum);
}
