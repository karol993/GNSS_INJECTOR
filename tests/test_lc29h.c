#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "injector_time.h"
#include "nmea_gen.h"

static void assert_burst_checksums(char *burst, uint32_t expected_lines)
{
  char *line = burst;
  uint32_t lines = 0U;
  while (*line != '\0') {
    char *star = strchr(line, '*');
    char *end = strstr(line, "\r\n");
    assert(star != 0 && end != 0 && star < end);
    *star = '\0';
    assert((uint8_t)strtoul(star + 1, 0, 16) == NMEA_Checksum(line + 1));
    line = end + 2;
    ++lines;
  }
  assert(lines == expected_lines);
}

int main(void)
{
  InjectorUtc utc = {2026U, 9U, 15U, 10U, 0U, 59U};
  char burst[1536];
  uint32_t lines = 0U;
  int length;

  length = NMEA_FormatLC29HBurst(burst, sizeof burst, &utc, 51507400, -127800, 'A', &lines);
  assert(length > 700 && length < 1200);
  assert(lines == 17U);
  assert(strstr(burst, "$GNRMC,100059.000,A,5130.444000,N,00007.668000,W,0.01,000.00,150926,,,A,V*") == burst);
  assert_burst_checksums(burst, lines);

  utc = (InjectorUtc){2026U, 9U, 15U, 23U, 59U, 59U};
  InjectorTime_IncrementSecond(&utc);
  assert(utc.day == 16U && utc.hour == 0U && utc.minute == 0U && utc.second == 0U);
  length = NMEA_FormatLC29HBurst(burst, sizeof burst, &utc, -33868800, 151209300, 'V', &lines);
  assert(length > 700);
  assert(strstr(burst, "$GNRMC,000000.000,V,3352.128000,S,15112.558000,E,0.01,000.00,160926,,,A,V*") != 0);
  assert_burst_checksums(burst, lines);

  length = NMEA_FormatLC29HBurst(burst, sizeof burst, &utc, 0, 0, 'A', &lines);
  assert(length > 700);
  assert(strstr(burst, ",0000.000000,N,00000.000000,E,") != 0);
  return 0;
}
