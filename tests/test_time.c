#include <assert.h>
#include <string.h>
#include "injector_time.h"

static void parse_ok(const char *text, uint16_t year, uint8_t month, uint8_t day,
                     uint8_t hour, uint8_t minute, uint8_t second)
{
  InjectorUtc utc = {0};
  assert(InjectorTime_Parse(text, &utc));
  assert(utc.year == year && utc.month == month && utc.day == day);
  assert(utc.hour == hour && utc.minute == minute && utc.second == second);
}

static void parse_bad(const char *text)
{
  InjectorUtc utc = {0};
  assert(!InjectorTime_Parse(text, &utc));
}

int main(void)
{
  parse_ok("2026-09-15 10:00:00", 2026U, 9U, 15U, 10U, 0U, 0U);
  parse_ok("2026-09-15T10:00:00", 2026U, 9U, 15U, 10U, 0U, 0U);
  parse_ok("2028-02-29 23:59:59", 2028U, 2U, 29U, 23U, 59U, 59U);
  parse_ok("2026-12-31 23:59:59", 2026U, 12U, 31U, 23U, 59U, 59U);
  parse_bad("2026-02-29 10:00:00");
  parse_bad("2026-13-01 10:00:00");
  parse_bad("2026-09-31 10:00:00");
  parse_bad("2026-09-15 24:00:00");
  parse_bad("2026-09-15 10:60:00");
  parse_bad("2026-09-15 10:00:60");
  parse_bad("abc");
  parse_bad("2026-09-15");
  parse_bad("2026-09-15 10:00:00 extra");
  return 0;
}
