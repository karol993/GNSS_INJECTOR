#ifndef PPS_GEN_H
#define PPS_GEN_H

#include "stm32f7xx_hal.h"

void PPS_GenInit(TIM_HandleTypeDef *timer);
void PPS_GenStart(void);
void PPS_GenStop(void);

#endif
