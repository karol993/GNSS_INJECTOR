# GNSS INJECTOR LC29H PROFILE HANDOFF

## 1. Architecture MODE MINIMAL / LC29H

MODE MINIMAL is the reset default and retains the existing RMC/ZDA scheduler
behavior. MODE LC29H selects a fixed-buffer NMEA burst generated at the same
scheduler deadline. The LC29H burst uses one USART2 interrupt-driven transmit,
so individual lines do not block the scheduler.

## 2. Generated LC29H sentences

GNRMC, GNVTG, GNGGA, four GNGSA, four GPGSV, two GLGSV, one GAGSV,
two GBGSV, and GNGLL.

## 3. Exact order

GNRMC, GNVTG, GNGGA, GNGSA x4, GPGSV x4, GLGSV x2, GAGSV,
GBGSV x2, GNGLL.

## 4. Lines per burst

17 lines.

## 5. Burst bytes

1032 bytes for the tested default data (the fixed sentence fields make this
stable for normal coordinate values).

## 6. Estimated TX time at 115200 8N1

1032 x 10 / 115200 = 89.6 ms.

## 7. RMC format

GNRMC uses HHMMSS.000, microdegree-derived ddmm.mmmmmm / dddmm.mmmmmm
coordinates, dynamic A/V fix status, speed 0.01, course 000.00, and
the trailing fields ,,,A,V. XOR checksums are calculated for every line.

## 8. OFFSET behavior

OFFSET remains the scheduler offset to the beginning of GNRMC. The remaining
LC29H lines follow contiguously in the same UART transfer. PPS/TIM2 scheduler
logic is unchanged.

## 9. PPS

Unchanged: TIM3 CH1 PB4, 1 Hz, 100 ms HIGH.

## 10. Changed files

Core/Inc/injector.h, injector_stats.h, nmea_gen.h, stm32f7xx_it.h;
Core/Src/injector.c, injector_cli.c, injector_stats.c, nmea_gen.c,
stm32f7xx_hal_msp.c, stm32f7xx_it.c; tests/test_lc29h.c; and this handoff.

## 11. Host tests

PASS with -Wall -Wextra -Werror. Tests cover MINIMAL generator compatibility,
LC29H RMC prefix and .000 format, A/V fields, all-line XOR validation,
UTC/date rollover, London/Sydney/zero coordinates, and burst size.

## 12. ARM build

ARM source compilation with -Wall -Wextra -Werror: PASS.
Manual full ARM link: PASS.

## 13. Flash/RAM

text=53424 bytes, data=2064 bytes, bss=6020 bytes.
Flash estimate (text + data): 55488 bytes. Initialized RAM: 8084 bytes.

## 14. Remaining risks

The LC29H profile is deterministic rather than satellite-accurate. USART2 TX
uses an interrupt-driven transfer; hardware confirmation should verify the
selected NVIC priority coexists with the existing application interrupt load.

FLASH PERFORMED: NO
COMMIT PERFORMED: NO
