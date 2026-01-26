/**
 * @file BSPSPI.c
 * @brief SPI 硬件抽象层实现（基于 STM32 标准库）
 * @note STM32F103 + SPI1
 */

#include "BSPSPI.h"
#include "stm32f10x.h"

/* 静态变量：用于保存 SysTick 计数 */
static volatile uint32_t g_systick_ms = 0;

/* ==================== SysTick 中断处理 ==================== */

/**
 * @brief SysTick 中断处理函数
 */
void SysTick_Handler(void)
{
    if (g_systick_ms > 0) {
        g_systick_ms--;
    }
}

/* ==================== SPI 初始化和控制 ==================== */

/**
 * @brief 初始化 SPI 接口和相关 GPIO
 */
void SPIInit(void)
{
    SPI_TypeDef* SPIChn = SPI1;
    SPI_InitTypeDef SPI_InitStruct;

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

    SPI_Init(SPIChn, &SPI_InitStruct);
}

// void quikeval_SPI_connect(void)
// {
//     GPIO_InitTypeDef GPIO_InitStruct;
//     SPI_InitTypeDef SPI_InitStruct;
    
//     /* 启用时钟 */
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
    
//     /* 配置 SPI1 GPIO 管脚：CLK(PA5)、MISO(PA6)、MOSI(PA7) */
//     GPIO_InitStruct.GPIO_Pin = SPI_PIN_CLK | SPI_PIN_MISO | SPI_PIN_MOSI;
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStruct);
    
//     /* 配置 CS 管脚（PA4）为普通输出 */
//     GPIO_InitStruct.GPIO_Pin = SPI_PIN_CS;
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(SPI_GPIO_PORT, &GPIO_InitStruct);
    
//     /* CS 默认为高电平（未选中） */
//     GPIO_SetBits(SPI_GPIO_PORT, SPI_PIN_CS);
    
//     /* 配置 SPI1 */
//     SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//     SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
//     SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
//     SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
//     SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
//     SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
//     SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
//     SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
//     SPI_InitStruct.SPI_CRCPolynomial = 7;
    
//     SPI_Init(SPI_PORT, &SPI_InitStruct);
//     SPI_Cmd(SPI_PORT, ENABLE);
// }

/**
 * @brief 启用 SPI 并设置时钟分频
 * @param clock_div 时钟分频系数
 */
void spi_enable(uint16_t clock_div)
{
    SPI_InitTypeDef SPI_InitStruct;
    
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = clock_div;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    
    SPI_Init(SPI_PORT, &SPI_InitStruct);
    SPI_Cmd(SPI_PORT, ENABLE);
}

/**
 * @brief 通过 SPI 收发一个字节
 * @param data 要发送的字节
 * @return 接收到的字节
 */
uint8_t spi_read(uint8_t data)
{
    /* 等待发送缓冲为空 */
    while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE) == RESET);
    
    /* 发送数据 */
    SPI_I2S_SendData(SPI_PORT, data);
    
    /* 等待接收缓冲非空 */
    while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_RXNE) == RESET);
    
    /* 返回接收到的数据 */
    return SPI_I2S_ReceiveData(SPI_PORT);
}

/**
 * @brief 通过 SPI 发送一个字节
 * @param data 要发送的字节
 */
void spi_write(int8_t data)
{
    spi_read((uint8_t)data);
}

/**
 * @brief 通过 SPI 同时发送和接收数据
 * @param tx_data 发送数据缓冲区
 * @param tx_len 发送数据长度
 * @param rx_data 接收数据缓冲区
 * @param rx_len 接收数据长度
 */
void spi_write_read(uint8_t *tx_data, uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len)
{
    uint8_t i;
    
    /* 发送数据 */
    for (i = 0; i < tx_len; i++) {
        spi_read(tx_data[i]);
    }
    
    /* 接收数据 */
    for (i = 0; i < rx_len; i++) {
        rx_data[i] = spi_read(0xFF);
    }
}

/**
 * @brief 通过 SPI 发送一组字节
 * @param length 数据长度
 * @param data 数据缓冲区指针
 */
void spi_write_array(uint8_t length, uint8_t *data)
{
    uint8_t i;
    for (i = 0; i < length; i++) {
        spi_write((int8_t)data[i]);
    }
}

/* ==================== GPIO 管脚控制 ==================== */

/**
 * @brief 设置 GPIO 管脚为低电平
 * @param pin GPIO 管脚号
 */
void output_low(uint8_t pin)
{
    if (pin == QUIKEVAL_CS) {
        GPIO_ResetBits(SPI_GPIO_PORT, SPI_PIN_CS);
    }
}

/**
 * @brief 设置 GPIO 管脚为高电平
 * @param pin GPIO 管脚号
 */
void output_high(uint8_t pin)
{
    if (pin == QUIKEVAL_CS) {
        GPIO_SetBits(SPI_GPIO_PORT, SPI_PIN_CS);
    }
}

/* ==================== 延时函数 ==================== */

/**
 * @brief 延时（毫秒）
 * @param ms 毫秒数
 */
void delay(uint32_t ms)
{
    /* 配置 SysTick 为 1ms */
    if (SysTick_Config(SystemCoreClock / 1000) == 0) {
        g_systick_ms = ms;
        while (g_systick_ms > 0);
    }
}

/**
 * @brief 延时（微秒）
 * @param us 微秒数
 * @note 使用简单循环延时，精度依赖于编译器优化和 CPU 速度
 */
void delayMicroseconds(uint32_t us)
{
    /* 对于 72MHz STM32F103，每个循环约 10 个时钟周期
       调整这个系数以适应你的实际硬件 */
    volatile uint32_t i = us * 7;
    while (i--);
}

