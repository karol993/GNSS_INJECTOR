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
static int append_sentence(char *out, size_t out_size, size_t *used, const char *body) {
  int count = snprintf(out + *used, out_size - *used, "$%s*%02X\r\n", body, NMEA_Checksum(body));
  if (count < 0 || (size_t)count >= out_size - *used) return -1;
  *used += (size_t)count;
  return 0;
}
static void format_coordinate_precise(char *out, size_t size, int32_t value_udeg, uint8_t degrees, char *hemisphere, char positive, char negative) {
  uint32_t magnitude = (uint32_t)(value_udeg < 0 ? -value_udeg : value_udeg);
  uint32_t deg = magnitude / 1000000U;
  uint32_t minutes_1000000 = (uint32_t)(((uint64_t)(magnitude % 1000000U) * 60000000U + 500000U) / 1000000U);
  *hemisphere = value_udeg < 0 ? negative : positive;
  if (minutes_1000000 >= 60000000U) { ++deg; minutes_1000000 = 0U; }
  (void)snprintf(out, size, degrees == 2U ? "%02lu%02lu.%06lu" : "%03lu%02lu.%06lu",
                 (unsigned long)deg, (unsigned long)(minutes_1000000 / 1000000U),
                 (unsigned long)(minutes_1000000 % 1000000U));
}
int NMEA_FormatLC29HBurst(char *out, size_t out_size, const InjectorUtc *utc,
                           int32_t latitude_udeg, int32_t longitude_udeg,
                           char fix, uint32_t *line_count) {
  char lat[20], lon[20], ns, ew, body[160];
  size_t used = 0U;
  uint32_t lines = 0U;
  static const char *const fixed[] = {
    "GNVTG,000.00,T,,M,0.01,N,0.02,K,A",
    "GNGSA,A,3,03,07,11,16,19,22,28,31,,,,,1.2,0.9,0.8,1",
    "GNGSA,A,3,65,66,67,68,69,70,71,72,,,,,1.5,1.0,1.1,2",
    "GNGSA,A,3,301,302,303,304,305,306,,,,,,,1.8,1.1,1.4,3",
    "GNGSA,A,3,201,202,203,204,205,206,,,,,,,1.7,1.0,1.3,4",
    "GPGSV,3,1,10,03,45,120,38,07,70,220,42,11,25,310,35,16,50,045,40,1",
    "GPGSV,3,2,10,19,15,180,33,22,55,270,41,28,35,090,37,31,65,330,43,1",
    "GPGSV,3,3,10,33,20,150,30,36,40,060,36,,,,,,,,,1",
    "GPGSV,1,1,00,,,,,,,,,,,,,,,,,1",
    "GLGSV,2,1,06,65,55,100,36,66,30,210,31,67,65,320,40,1",
    "GLGSV,2,2,06,68,20,040,28,69,45,250,34,70,15,170,25,1",
    "GAGSV,1,1,04,301,50,120,36,302,30,220,31,303,65,310,40,304,20,040,26,1",
    "GBGSV,2,1,06,201,55,100,35,202,30,210,30,203,65,320,39,1",
    "GBGSV,2,2,06,204,20,040,27,205,45,250,33,206,15,170,24,1"
  };
  uint32_t index;
  if (out == 0 || line_count == 0 || !InjectorTime_IsValid(utc) || (fix != 'A' && fix != 'V')) return -1;
  format_coordinate_precise(lat, sizeof lat, latitude_udeg, 2U, &ns, 'N', 'S');
  format_coordinate_precise(lon, sizeof lon, longitude_udeg, 3U, &ew, 'E', 'W');
  (void)snprintf(body, sizeof body, "GNRMC,%02u%02u%02u.000,%c,%s,%c,%s,%c,0.01,000.00,%02u%02u%02u,,,A,V",
                 utc->hour, utc->minute, utc->second, fix, lat, ns, lon, ew,
                 utc->day, utc->month, (unsigned)(utc->year % 100U));
  if (append_sentence(out, out_size, &used, body) != 0) return -1;
  ++lines;
  if (append_sentence(out, out_size, &used, fixed[0]) != 0) return -1;
  ++lines;
  (void)snprintf(body, sizeof body, "GNGGA,%02u%02u%02u.000,%s,%c,%s,%c,%u,08,0.9,250.0,M,0.0,M,,",
                 utc->hour, utc->minute, utc->second, lat, ns, lon, ew, fix == 'A' ? 1U : 0U);
  if (append_sentence(out, out_size, &used, body) != 0) return -1;
  ++lines;
  for (index = 1U; index < sizeof(fixed) / sizeof(fixed[0]); ++index) {
    if (append_sentence(out, out_size, &used, fixed[index]) != 0) return -1;
    ++lines;
  }
  (void)snprintf(body, sizeof body, "GNGLL,%s,%c,%s,%c,%02u%02u%02u.000,%c,A",
                 lat, ns, lon, ew, utc->hour, utc->minute, utc->second, fix);
  if (append_sentence(out, out_size, &used, body) != 0) return -1;
  ++lines;
  *line_count = lines;
  return (int)used;
}
