#ifndef INTERFACE_H
#define INTERFACE_H
#include "stm32f10x.h"

/*---------------- 接口函数声明 ----------------*/
void LTC6804Wakeup(void);
void LTC6804SendCommand(u16 command);
void LTC6804StartADC(void);
void LTC6804ReadCellVoltages(u8 total_ic);
void LTC6804ReadAuxVoltages(u8 total_ic);
void LTC6804CS(u8 state);

u16 LTC6804_CalcPEC(const u8* data, u8 len);
void LTC6804_ReadRaw(u16 cmd, u8* rx, u8 len);

#endif
