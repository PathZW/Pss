/******************************************************************************
 *
 * 文件名  ： Drv_Ad5683.h
 * 负责人  ： zwj
 * 创建日期： 20200827
 * 版本号  ： v1.0
 * 文件描述： PSS AD5683驱动
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/

/*---------------------------------- 预处理区 ---------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _PSS_MCU_DRVS_PWM_TEMP_H_
#define _PSS_MCU_DRVS_PWM_TEMP_H_

/************************************ 头文件 ***********************************/
#include <stdint.h>
#include <stdbool.h>

#define PSS_MCU_DRVS_MEAS_PWM_BASE                           (0x60007000)

typedef struct
{
    volatile uint32_t  pwm1HighLevelLength;                    /* 00 PWM高电平长度寄存器 */
    volatile uint32_t  pwm1LowLevelLength;                     /* 04 PWM低电平长度寄存器 */
    volatile uint32_t  pwm2HighLevelLength;                    /* 08 PWM高电平长度寄存器 */
    volatile uint32_t  pwm2LowLevelLength;                     /* 0C PWM低电平长度寄存器 */
    volatile uint32_t  pwm3HighLevelLength;                    /* 10 PWM高电平长度寄存器 */
    volatile uint32_t  pwm3LowLevelLength;                     /* 14 PWM低电平长度寄存器 */
    volatile uint32_t  pwm4HighLevelLength;                    /* 18 PWM高电平长度寄存器 */
    volatile uint32_t  pwm4LowLevelLength;                     /* 1C PWM低电平长度寄存器 */
    volatile uint32_t  pwm5HighLevelLength;                    /* 20 PWM高电平长度寄存器 */
    volatile uint32_t  pwm5LowLevelLength;                     /* 24 PWM低电平长度寄存器 */
    volatile uint32_t  pwm6HighLevelLength;                    /* 28 PWM高电平长度寄存器 */
    volatile uint32_t  pwm6LowLevelLength;                     /* 2C PWM低电平长度寄存器 */
    volatile uint32_t  pwm7HighLevelLength;                    /* 30 PWM高电平长度寄存器 */
    volatile uint32_t  pwm7LowLevelLength;                     /* 34 PWM低电平长度寄存器 */
    volatile uint32_t  pwm8HighLevelLength;                    /* 38 PWM高电平长度寄存器 */
    volatile uint32_t  pwm8LowLevelLength;                     /* 3C PWM低电平长度寄存器 */
    volatile uint32_t  pwm9HighLevelLength;                    /* 40 PWM高电平长度寄存器 */
    volatile uint32_t  pwm9LowLevelLength;                     /* 44 PWM低电平长度寄存器 */
    volatile uint32_t  clearReg;                               /* 48 测试寄存器 */
}MPwm_TypeDef;

typedef struct _PSS_MCU_DRVS_MPWM_TAG_
{
    MPwm_TypeDef              *pwmHw;                          /*pwm*/
} PSS_MCU_DRVS_MPWM_T;                        /* PWM */

void PssMcuDrvsMPwmInit(PSS_MCU_DRVS_MPWM_T *pwm, uint32_t addr);
void PssMcuDrvsMPwmClear(void);
float PssMcuDrvsGetTemp(int ch);
float PssMcuDrvsGetWaterFlow(void);
/************************************ 宏定义 ***********************************/

#endif // #ifndef _PSS_MCU_DRVS_PWM_TEMP_H_

#ifdef __cplusplus
}
#endif

