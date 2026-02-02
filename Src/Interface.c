#include "Interface.h"
#include "BSPSPI.h"
#include "BSPDelay.h"
#include "LTC6804Cmd.h"
#include <stdint.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

/** 
 * @brief 唤醒 LTC6804 芯片
 */
void LTC6804Wakeup(void)
{
    LTC6804CS(0);  // 拉低片选
    SPIWriteByte(SPI_PORT, 0xFF); // 发送空数据唤醒
    LTC6804CS(1);  // 拉高片选
    delayUs(300);   // 芯片稳定后再进行其他操作
}

/**
 * @brief 控制LTC6804片选状态
 * @param state 0：拉低片选，1：拉高片选
 */
void LTC6804CS(u8 state)
{
    if (state == 1)
    {
        GPIO_SetBits(SPI_GPIO_PORT, SPI_PIN_CS);
    }
    else
    {
        GPIO_ResetBits(SPI_GPIO_PORT, SPI_PIN_CS);
    }
}

/**
 * @brief 发送命令到 LTC6804
 * @param command 要发送的命令
 */
void LTC6804SendCommand(u16 command)
{

}


void LTC6804StartADC(void);
void LTC6804ReadCellVoltages(u8 total_ic);
void LTC6804ReadAuxVoltages(u8 total_ic);


u16 LTC6804_CalcPEC(const u8* data, u8 len);
void LTC6804_ReadRaw(u16 cmd, u8* rx, u8 len);
