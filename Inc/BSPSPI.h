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

/* ==================== SPI 函数声明 ==================== */
void SPIInit(void);
void SPIWriteByte(SPI_TypeDef* SPICh, u16 comm);
void SPIRead(SPI_TypeDef* SPICh, u16 comm, u8* data, u8 len);

void BSP_SPI_CS_Low(void);
void BSP_SPI_CS_High(void);
void SPI_WaitIdle(SPI_TypeDef* SPICh);

#endif /* SPI_HAL_H */
