#include <assert.h>
#include <string.h>
#include "injector_time.h"
#include "nmea_gen.h"

int main(void) {
  InjectorUtc utc = {2026U, 12U, 31U, 23U, 59U, 59U}; char text[128];
  assert(NMEA_Checksum("GNRMC,100000.00,A,5000.0000,N,01900.0000,E,0.0,0.0,150926,,,A") == 0x45U);
  InjectorTime_IncrementSecond(&utc); assert(utc.year == 2027U && utc.month == 1U && utc.day == 1U && utc.hour == 0U);
  utc = (InjectorUtc){2028U,2U,28U,23U,59U,59U}; InjectorTime_IncrementSecond(&utc); assert(utc.day == 29U); utc.hour=23U; utc.minute=59U; utc.second=59U; InjectorTime_IncrementSecond(&utc); assert(utc.month == 3U && utc.day == 1U);
  utc = (InjectorUtc){2026U,9U,15U,10U,0U,0U}; assert(NMEA_FormatRMC(text,sizeof text,&utc,50000000,19000000,'A') > 0); assert(strcmp(text,"$GNRMC,100000.00,A,5000.0000,N,01900.0000,E,0.0,0.0,150926,,,A*45\r\n") == 0);
  assert(NMEA_FormatRMC(text,sizeof text,&utc,-50000000,-19000000,'V') > 0); assert(strstr(text,",5000.0000,S,01900.0000,W,") != 0);
  assert(NMEA_FormatZDA(text,sizeof text,&utc) > 0); assert(strcmp(text,"$GNZDA,100000.00,15,09,2026,00,00*72\r\n") == 0); return 0;
}
