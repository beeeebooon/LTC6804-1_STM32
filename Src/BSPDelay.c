#include "BSPDelay.h"

/* ==================== 延时函数 ==================== */

/* 静态变量：用于保存 SysTick 计数 */
static volatile uint32_t g_systick_ms = 0;

/**
 * @brief 延时（毫秒）
 * @param ms 毫秒数
 */
void delayMs(u32 ms)
{
    /* 配置 SysTick 为 1ms */
    if (SysTick_Config(SystemCoreClock / 1000) == 0) {
        g_systick_ms = ms;
        while (g_systick_ms > 0);
    }
}

/**
 * @brief 延时（秒）
 * @param s 秒数
 */
void delayS(void)
{
    for (u32 i = 0; i < 1000; i++) {
        delayMs(1);
    }
}

/**
 * @brief 延时（微秒）
 * @param us 微秒数
 * @note 使用简单循环延时，精度依赖于编译器优化和 CPU 速度
 */
void delayUs(u32 us)
{
    /* 对于 72MHz STM32F103，每个循环约 10 个时钟周期
       调整这个系数以适应你的实际硬件 */
    volatile u32 i = us * 7;
    while (i--);
}

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
