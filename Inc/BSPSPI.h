/**
 * @file spi_hal.h
 * @brief SPI 硬件抽象层头文件（基于 STM32 标准库）
 * @note 针对 STM32F103 + SPI1
 */

#ifndef BSPSPI_H
#define BSPSPI_H

#include <stdint.h>
#include "stm32f10x.h"

/* ==================== SPI 配置宏定义 ==================== */

/* GPIO 和 SPI 端口配置 */
#define SPI_PORT          SPI1
#define SPI_GPIO_PORT     GPIOA
#define SPI_PIN_CLK       GPIO_Pin_5   /* PA5 */
#define SPI_PIN_MISO      GPIO_Pin_6   /* PA6 */
#define SPI_PIN_MOSI      GPIO_Pin_7   /* PA7 */
#define SPI_PIN_CS        GPIO_Pin_4   /* PA4 */

/* 片选脚 GPIO 值 */
#define QUIKEVAL_CS       4  /* PA4 作为片选脚 */

/* ==================== 函数声明 ==================== */

/**
 * @brief 初始化 SPI 接口
 */
void SPIInit(void);

/**
 * @brief 启用 SPI 并配置时钟分频
 * @param clock_div SPI 时钟分频系数（使用 SPI_CLOCK_DIV_* 宏）
 */
void spi_enable(uint16_t clock_div);

/**
 * @brief 通过 SPI 接收一个字节
 * @param data 要发送的字节（为了获得返回数据）
 * @return 接收到的字节
 */
uint8_t spi_read(uint8_t data);

/**
 * @brief 通过 SPI 发送一个字节
 * @param data 要发送的字节
 */
void spi_write(int8_t data);

/**
 * @brief 通过 SPI 同时发送和接收数据
 * @param tx_data 发送数据缓冲区
 * @param tx_len 发送数据长度
 * @param rx_data 接收数据缓冲区
 * @param rx_len 接收数据长度
 */
void spi_write_read(uint8_t *tx_data, uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len);

/**
 * @brief 通过 SPI 发送一组字节
 * @param length 数据长度
 * @param data 数据缓冲区指针
 */
void spi_write_array(uint8_t length, uint8_t *data);

/* ==================== GPIO 控制函数 ==================== */

/**
 * @brief 设置 GPIO 管脚为低电平
 * @param pin GPIO 管脚（如 QUIKEVAL_CS）
 */
void output_low(uint8_t pin);

/**
 * @brief 设置 GPIO 管脚为高电平
 * @param pin GPIO 管脚（如 QUIKEVAL_CS）
 */
void output_high(uint8_t pin);

/* ==================== 延时函数 ==================== */

/**
 * @brief 延时（毫秒）
 * @param ms 毫秒数
 */
void delay(uint32_t ms);

/**
 * @brief 延时（微秒）
 * @param us 微秒数
 */
void delayMicroseconds(uint32_t us);

#endif /* SPI_HAL_H */

