#ifndef BSPDELAY_H
#define BSPDELAY_H
#include "stm32f10x.h"

void delayMs(u32 ms);
void delayS(void);
void delayUs(u32 us);
void SysTick_Handler(void);

#endif
