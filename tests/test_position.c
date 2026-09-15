#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "injector_position.h"
#include "injector_time.h"
#include "nmea_gen.h"

static void parse_ok(const char *text, int32_t expected)
{
  int32_t value = 0;
  assert(InjectorPosition_ParseDecimal(text, &value));
  assert(value == expected);
}

static void parse_bad(const char *text)
{
  int32_t value = 0;
  assert(!InjectorPosition_ParseDecimal(text, &value));
}

static void pair_ok(const char *text)
{
  char buffer[48];
  int32_t latitude;
  int32_t longitude;
  (void)strcpy(buffer, text);
  assert(InjectorPosition_ParsePair(buffer, &latitude, &longitude));
}

static void pair_bad(const char *text)
{
  char buffer[48];
  int32_t latitude;
  int32_t longitude;
  (void)strcpy(buffer, text);
  assert(!InjectorPosition_ParsePair(buffer, &latitude, &longitude));
}

static void assert_checksum(char *sentence)
{
  char *star = strchr(sentence, '*');
  assert(star != 0);
  *star = '\0';
  assert((uint8_t)strtoul(star + 1, 0, 16) == NMEA_Checksum(sentence + 1));
}

int main(void)
{
  InjectorUtc utc = {2026U, 9U, 15U, 10U, 0U, 0U};
  char rmc[128];
  char formatted[16];

  parse_ok("+51.5074", 51507400);
  parse_ok("51.5074", 51507400);
  parse_ok("-0.1278", -127800);
  parse_ok("-0.0", 0);
  parse_ok("0", 0);
  parse_ok("0.0", 0);
  parse_ok("-33.8688", -33868800);
  parse_ok("151.2093", 151209300);
  parse_ok("90", 90000000);
  parse_ok("-90", -90000000);
  parse_ok("180", 180000000);
  parse_ok("+19.93658", 19936580);
  parse_bad("abc"); parse_bad("50.0x"); parse_bad("--50"); parse_bad("50 19");
  parse_bad(""); parse_bad("+"); parse_bad(".5"); parse_bad("50.0000000");
  pair_ok("51.5074 -0.1278"); pair_ok("51.5074 0.1278");
  pair_ok("-33.8688 151.2093"); pair_ok("0.0 0.0");
  pair_ok("90 180"); pair_ok("-90 -180"); pair_ok("+50.06143 +19.93658");
  pair_bad("90.000001 0"); pair_bad("0 180.000001");
  pair_bad("abc 19"); pair_bad("50 abc"); pair_bad("50");
  pair_bad("50 19 extra"); pair_bad("50.0x 19"); pair_bad("--50 19");

  InjectorPosition_Format(formatted, sizeof formatted, 51507400);
  assert(strcmp(formatted, "51.507400") == 0);
  InjectorPosition_Format(formatted, sizeof formatted, -127800);
  assert(strcmp(formatted, "-0.127800") == 0);
  InjectorPosition_Format(formatted, sizeof formatted, 0);
  assert(strcmp(formatted, "0.000000") == 0);

  assert(NMEA_FormatRMC(rmc, sizeof rmc, &utc, 51507400, -127800, 'A') > 0);
  assert(strstr(rmc, ",5130.4440,N,00007.6680,W,") != 0);
  assert_checksum(rmc);
  assert(NMEA_FormatRMC(rmc, sizeof rmc, &utc, 0, 0, 'A') > 0);
  assert(strstr(rmc, ",0000.0000,N,00000.0000,E,") != 0);
  assert_checksum(rmc);
  assert(NMEA_FormatRMC(rmc, sizeof rmc, &utc, -33868800, 151209300, 'A') > 0);
  assert(strstr(rmc, ",3352.1280,S,15112.5580,E,") != 0);
  assert_checksum(rmc);
  return 0;
}
