#include "injector_cli.h"
#include "injector.h"
#include "injector_stats.h"
#include "injector_position.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UART_HandleTypeDef *cli_uart; static char line[96]; static uint8_t line_len;
static void reply(const char *text) { (void)HAL_UART_Transmit(cli_uart, (uint8_t *)text, (uint16_t)strlen(text), 100U); }
static void process(char *command) {
  InjectorUtc time; int32_t latitude_udeg, longitude_udeg; long offset; char arg[8]; char status[192]; size_t i;
  for (i = 0U; command[i] != '\0'; ++i) command[i] = (char)toupper((unsigned char)command[i]);
  InjectorStats_CliCommand();
  if (strcmp(command,"HELP") == 0) reply("INJ,HELP,START,STOP,STATUS,STATS,TXTEST,TIME,POS,FIX,PPS,RMC,ZDA,OFFSET,RESET\r\n");
  else if (strcmp(command,"START") == 0) { INJECTOR_Start(); reply("INJ,OK\r\n"); }
  else if (strcmp(command,"STOP") == 0) { INJECTOR_Stop(); reply("INJ,OK\r\n"); }
  else if (strcmp(command,"STATUS") == 0) { INJECTOR_Status(status, sizeof status); reply(status); }
  else if (strcmp(command,"STATS") == 0) { INJECTOR_Stats(status, sizeof status); reply(status); }
  else if (strcmp(command,"TXTEST") == 0) { HAL_StatusTypeDef tx_status = INJECTOR_TxTest(); if (tx_status == HAL_OK) reply("INJ,OK,TXTEST\r\n"); else { (void)snprintf(status, sizeof status, "INJ,ERR,TXTEST,%d\r\n", (int)tx_status); reply(status); } }
  else if (sscanf(command,"TIME %hu-%hhu-%hhu %hhu:%hhu:%hhu", &time.year,&time.month,&time.day,&time.hour,&time.minute,&time.second) == 6 && InjectorTime_IsValid(&time)) { INJECTOR_SetTime(&time); reply("INJ,OK\r\n"); }
  else if (strncmp(command,"POS ",4U) == 0 && InjectorPosition_ParsePair(command + 4, &latitude_udeg, &longitude_udeg)) { INJECTOR_SetPosition(latitude_udeg, longitude_udeg); reply("INJ,OK\r\n"); }
  else if (sscanf(command,"FIX %7s",arg) == 1 && (arg[0] == 'A' || arg[0] == 'V') && arg[1] == '\0') { INJECTOR_SetFix(arg[0]); reply("INJ,OK\r\n"); }
  else if (sscanf(command,"PPS %7s",arg) == 1 && (strcmp(arg,"ON") == 0 || strcmp(arg,"OFF") == 0)) { INJECTOR_SetPps((uint8_t)(arg[0] == 'O' && arg[1] == 'N')); reply("INJ,OK\r\n"); }
  else if (sscanf(command,"RMC %7s",arg) == 1 && (strcmp(arg,"ON") == 0 || strcmp(arg,"OFF") == 0)) { INJECTOR_SetRmc((uint8_t)(arg[0] == 'O' && arg[1] == 'N')); reply("INJ,OK\r\n"); }
  else if (sscanf(command,"ZDA %7s",arg) == 1 && (strcmp(arg,"ON") == 0 || strcmp(arg,"OFF") == 0)) { INJECTOR_SetZda((uint8_t)(arg[0] == 'O' && arg[1] == 'N')); reply("INJ,OK\r\n"); }
  else if (sscanf(command,"OFFSET %ld", &offset) == 1 && INJECTOR_SetOffset((int32_t)offset)) reply("INJ,OK\r\n");
  else if (strcmp(command,"RESET") == 0) { INJECTOR_Stop(); INJECTOR_ResetDefaults(); reply("INJ,OK\r\n"); }
  else { InjectorStats_CliError(); reply("INJ,ERR,BAD_ARGUMENT\r\n"); }
}
void INJECTOR_CLI_Init(UART_HandleTypeDef *uart) { cli_uart = uart; line_len = 0U; }
void INJECTOR_CLI_Task(void) { uint8_t c; while (HAL_UART_Receive(cli_uart, &c, 1U, 0U) == HAL_OK) { if (c == '\r' || c == '\n') { if (line_len != 0U) { line[line_len] = '\0'; process(line); line_len = 0U; } } else if (line_len < sizeof(line) - 1U && c >= 0x20U && c < 0x7FU) line[line_len++] = (char)c; else if (line_len >= sizeof(line) - 1U) line_len = 0U; } }
