/**
 * @file BSPSPI.c
 * @brief SPI 硬件抽象层实现（基于 STM32 标准库）
 * @note STM32F103 + SPI1
 */

#include "BSPSPI.h"
#include "stm32f10x.h"

/* ==================== SPI 初始化和控制 ==================== */

/**
 * @brief 初始化 SPI 接口和相关 GPIO
 */
void SPIInit(void)
{
    SPI_TypeDef* SPIChn = SPI_PORT;
    SPI_InitTypeDef SPI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;

    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置CLK、MOSI为复用推挽输出，由SPI外设控制
    GPIO_InitStruct.GPIO_Pin = SPI_PIN_CLK | SPI_PIN_MOSI;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStruct);

    // 配置CS为普通推挽输出，由软件控制
    GPIO_InitStruct.GPIO_Pin = SPI_PIN_CS;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStruct);

    GPIO_SetBits(SPI_GPIO_PORT, SPI_PIN_CS);  // CS 默认为高电平（空闲状态）

    // 配置MISO为浮空输入，从设备上拉
    GPIO_InitStruct.GPIO_Pin = SPI_PIN_MISO;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStruct);

    /* 初始化SPI结构体 */
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 全双工
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                        // 主机模式
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                    // 8位数据帧
    /* LTC6804要求CPOL = 1,CPHA = 1,时钟空闲和采样定义 */
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;                              // 时钟空闲低电平
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;                             // 第二个时钟边沿采样
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;    // 波特率预分频64，不能超过LTC6804最大频率(1.5 MHz)
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;                            // CS由GPIO控制
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                    // LTC6804要求MSB先行

    SPI_I2S_ITConfig(SPIChn, SPI_I2S_IT_RXNE, ENABLE); // 使能接收中断
    SPI_Init(SPIChn, &SPI_InitStruct);
    SPI_Cmd(SPIChn, ENABLE);  // 使能SPI
}


/**
 * @brief SPI 发送数据或数据
 * @param SPICh SPI 通道
 * @param comm LTC6804指令
 */
void SPIWriteByte(SPI_TypeDef* SPICh, u16 comm)
{
    u8 j = 0;

    SPI_I2S_SendData(SPICh, comm);
    // 等待发送缓冲区空，发送完成
    while (SPI_I2S_GetFlagStatus(SPICh, SPI_I2S_FLAG_TXE) == RESET)
    {
        if (j > 0xff)
        {
            break;      
        }
        j++;
    }
}

/**
 * @brief SPI 读取数据
 * @param SPICh SPI 通道
 * @param comm LTC6804指令
 * @param data 存储读取到的数据
 * @param len 读取数据长度
 * @note 读取指定长度数据（需持续发送dummy）
 */
void SPIRead(SPI_TypeDef* SPICh, u16 comm, u8* data, u8 len)
{
    u8 j = 0;

    SPIWriteByte(SPICh, comm);
    *data = SPI_I2S_ReceiveData(SPICh); // 读取无效数据
    for (u8 i = 0; i < len; i++)
    {
        SPIWriteByte(SPICh, 0xff); // 发送空数据以产生时钟
        *data = SPI_I2S_ReceiveData(SPICh);
        while (SPI_I2S_GetFlagStatus(SPICh, SPI_I2S_FLAG_RXNE) == RESET)  // 等待接收缓冲区空，接收完成
        {
            if (j > 0xff)
            {
                break;
            }
            j++;
        }
        data++;
    }
}


