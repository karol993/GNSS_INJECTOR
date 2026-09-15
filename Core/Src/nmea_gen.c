#include "nmea_gen.h"
#include <stdio.h>

uint8_t NMEA_Checksum(const char *body) { uint8_t sum = 0U; while (*body != '\0') sum ^= (uint8_t)*body++; return sum; }
static void format_coordinate(char *out, size_t size, float value, uint8_t degrees, char *hemisphere, char positive, char negative) {
  uint32_t scaled; uint32_t deg; uint32_t minute_10000;
  *hemisphere = value < 0.0f ? negative : positive; if (value < 0.0f) value = -value;
  scaled = (uint32_t)(value * 600000.0f + 0.5f); deg = scaled / 600000U; minute_10000 = scaled - deg * 600000U;
  (void)snprintf(out, size, degrees == 2U ? "%02lu%02lu.%04lu" : "%03lu%02lu.%04lu",
                 (unsigned long)deg, (unsigned long)(minute_10000 / 10000U), (unsigned long)(minute_10000 % 10000U));
}
int NMEA_FormatRMC(char *out, size_t out_size, const InjectorUtc *utc, float latitude, float longitude, char fix) {
  char lat[16], lon[16], ns, ew, body[100]; uint8_t sum;
  if (out == 0 || !InjectorTime_IsValid(utc) || (fix != 'A' && fix != 'V')) return -1;
  format_coordinate(lat, sizeof lat, latitude, 2U, &ns, 'N', 'S'); format_coordinate(lon, sizeof lon, longitude, 3U, &ew, 'E', 'W');
  (void)snprintf(body, sizeof body, "GNRMC,%02u%02u%02u.00,%c,%s,%c,%s,%c,0.0,0.0,%02u%02u%02u,,,A", utc->hour, utc->minute, utc->second, fix, lat, ns, lon, ew, utc->day, utc->month, (unsigned)(utc->year % 100U));
  sum = NMEA_Checksum(body); return snprintf(out, out_size, "$%s*%02X\r\n", body, sum);
}
int NMEA_FormatZDA(char *out, size_t out_size, const InjectorUtc *utc) {
  char body[64]; uint8_t sum; if (out == 0 || !InjectorTime_IsValid(utc)) return -1;
  (void)snprintf(body, sizeof body, "GNZDA,%02u%02u%02u.00,%02u,%02u,%04u,00,00", utc->hour, utc->minute, utc->second, utc->day, utc->month, utc->year);
  sum = NMEA_Checksum(body); return snprintf(out, out_size, "$%s*%02X\r\n", body, sum);
}
