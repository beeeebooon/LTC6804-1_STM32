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
 * @param command 要发送的命令，具体命令见ILTC6804Cmd.h
 */
void LTC6804SendCommand(u16 command)
{
    u8 cmd[2] = {0};
    u16 cmd_pec = 0;

    cmd[0] = (u8)(command >> 8);
    cmd[1] = (u8)command;

    cmd_pec = LTC6804_CalcPEC(cmd, 2);
    
    LTC6804CS(0);  // 拉低片选
    SPIWriteByte(SPI_PORT, cmd[0]);  // 虽然是16位命令，传输的数据必须是8位，这是SPI_DataSize_8b决定的，因为寄存器大小固定
    SPIWriteByte(SPI_PORT, cmd[1]);
    SPIWriteByte(SPI_PORT, (u8)(cmd_pec >> 8));
    SPIWriteByte(SPI_PORT, (u8)cmd_pec);
    LTC6804CS(1);  // 拉高片选
}

/**
 * @brief 运算法计算 LTC6804 命令的 PEC 校验码
 * @param data 要计算 PEC 的数据指针(拆分为字节数组)
 * @param len 数据长度(每个数据指针的长度)
 */
u16 LTC6804_CalcPEC(const u8* data, u8 len)
{
    u16 crc = 0x0010; // 初始值
    for (u8 i = 0; i < len; i++)
    {
        crc ^= (u16)(data[i] << 7);
        for (u8 j = 0; j < 8; j++)  // 逐个处理每一位，从大端开始
        {
            if (crc & 0x4000)  // 检查最高位是否为1
            {
                crc = (crc << 1)^0x4599;
            }
            else
            {
                crc = (crc << 1);
            }
            crc = crc & 0x7FFF; // 保持15位 
        }
    }
    return crc << 1; // PEC的最低位为0
}

/**
 * @brief 启动 LTC6804 ADC 测量
 * @param md 测量模式 (0-3)
 * @param dcp 是否启用开路检测 (0-1)
 * @param ch 通道号 (0-6)
 */
void LTC6804StartADC(u8 md, u8 dcp, u8 ch)
{
    u16 command = 0;

    if (md >3 || dcp > 1 || ch > 6)
    {
        return; // 参数错误，直接返回
    }
    
    command = ADCV(md, dcp, ch);
    LTC6804SendCommand(command);
}

void LTC6804ReadCellVoltages(u8 total_ic);
void LTC6804ReadAuxVoltages(u8 total_ic);



void LTC6804_ReadRaw(u16 cmd, u8* rx, u8 len);
