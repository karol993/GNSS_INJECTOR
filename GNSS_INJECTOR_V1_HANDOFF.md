# GNSS INJECTOR V1 HANDOFF

## 1. MCU

STM32F767ZITx on NUCLEO-F767ZI (LQFP144).

## 2. Clock configuration

HSE bypass is 8 MHz. PLLM=4, PLLN=96, PLLP=2 produces SYSCLK/HCLK=96 MHz.
APB1 is HCLK/2=48 MHz; its timer clock is doubled to 96 MHz. APB2 is 96 MHz.

## 3. CONTROL UART

USART3, PD8 TX and PD9 RX, AF7, 115200 8N1, no flow control. The existing
board configuration labels these pins STLK_RX/STLK_TX; ST's Nucleo-144
documentation confirms this USART3 pair is the ST-LINK VCP connection.

## 4. GNSS UART

USART2 TX only, PD5 AF7, 115200 8N1, no flow control. PD5 is CN9 D53 and
CN11 pin 41 in UM1974. Connect it to the GPSDCF RX with a shared ground.

## 5. PPS

TIM3 channel 1 on PB4 AF2 (PPS_OUT), exposed as CN7 D25 / CN12 pin 27.
TIM3 input is 96 MHz; PSC=9599 gives 10 kHz, ARR=9999 gives 1 Hz, and
CCR1=1000 gives a 100 ms active-high pulse. STOP explicitly drives PB4 low.

## 6. TIM2

TIM2 input is 96 MHz; PSC=95 gives a 1 MHz counter (1 us tick), and
ARR=0xFFFFFFFF gives a free-running 32-bit reference counter.

## 7. CLI commands

HELP, STATUS, START, STOP, TIME YYYY-MM-DD HH:MM:SS,
POS latitude longitude, FIX A|V, PPS ON|OFF, RMC ON|OFF,
ZDA ON|OFF, OFFSET -500..500, and RESET. Responses are INJ,OK,
INJ,ERR,BAD_ARGUMENT, or a single parseable INJ,STATUS record.

## 8. RMC example

$GNRMC,100000.00,A,5000.0000,N,01900.0000,E,0.0,0.0,150926,,,A*45

## 9. ZDA example

$GNZDA,100000.00,15,09,2026,00,00*72

## 10. NMEA checksum implementation

The generator XORs every byte between $ and *, then formats the result as
two uppercase hexadecimal characters. Checksums are never hard-coded.

## 11. NMEA/PPS epoch semantics

Each scheduled NMEA sentence stores the UTC epoch of its corresponding PPS.
For OFFSET=+200, PPS begins at epoch .000 and RMC is transmitted at .200.
For OFFSET=-200, RMC for the upcoming epoch is sent .200 before its PPS.
No HAL_Delay, SysTick PPS, or main-loop GPIO toggle is used. The first PPS
is phase-established on START from TIM2; an external UTC reference is still
needed for absolute real-world UTC alignment.

## 12. Build result

All project application sources compile with CubeIDE ARM GCC 11.3.1 using
-Wall -Wextra -Werror. A manual full local link also completed. The
headless IDE build could not access the workspace because the desktop IDE
holds its lock; no hardware programming was attempted.

## 13. Flash/RAM

Manual local link: text=49,000 bytes, data=2,056 bytes, bss=4,444 bytes.
Flash estimate (text+data) is 51,056 bytes; initialized RAM is 6,500 bytes.

## 14. Host test result

PASS with host GCC and -Wall -Wextra -Werror: checksum, RMC formatting
(including A/V and N/S/E/W), UTC increment, year rollover, leap day, and
month rollover.

## 15. Files created/modified

GNSS_INJECTOR.ioc, Cube-generated integration files, the six injector
interfaces and implementations under Core/Inc and Core/Src, host test
tests/test_nmea.c, .gitignore, and this handoff.

## 16. Git commit hash

The single repository commit is reported in the final delivery message.

## 17. Hardware items still requiring confirmation

Verify the physical GPSDCF RX voltage level is 3.3 V-tolerant and connect a
common ground. Confirm the actual Nucleo board solder-bridge configuration
still routes USART3 PD8/PD9 to ST-LINK VCP; no bridge settings were changed.
Verify the intended harness uses the listed connector positions.

## 18. Known limitations

V1 has no external absolute UTC disciplining, DMA UART transmit, scripted
scenarios, route simulation, jitter/drift/noise controls, or Python runner.
The START PPS phase is deterministic relative to TIM2 but host/CLI time set
does not itself provide an absolute UTC time source.

FLASH PERFORMED: NO
NUMBER OF COMMITS CREATED: 1
