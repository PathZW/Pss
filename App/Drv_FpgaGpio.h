/********************************************************************************
 *
 * 文件名  ： PssMcuDrvsGpio.h
 * 负责人  :  PengPeng
 * 创建日期:  2019.04.02
 * 版本号  :  v1.1
 * 文件描述： PSS Mcu GPIO驱动 接口
 * 其    他:  无
 * 修改日志:  2019.04.02 PengPeng     初始版本       v1.0
 *            2021.07.26 PengPeng     完善设计       v1.1
 *
 *******************************************************************************/

/*---------------------------------- 预处理区 ---------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _PSS_MCU_DRVS_GPIO_H_
#define _PSS_MCU_DRVS_GPIO_H_

/************************************ 头文件 ***********************************/

/************************************ 宏定义 ***********************************/
#include <stdint.h>
#include <stdbool.h>
#define    GPIO_PORT0_DIR       (volatile uint32_t *)(0x60008000)
#define    GPIO_PORT1_DIR       (volatile uint32_t *)(0x60008004)
#define    GPIO_PORT0_VAL       (volatile uint32_t *)(0x60008008)
#define    GPIO_PORT1_VAL       (volatile uint32_t *)(0x6000800C)

#ifdef MCU_SENS
#define    PSS_MCU_FPGA_GPIO_CHARGE_FULL             ((uint32_t)(1u <<  0))
#define    PSS_MCU_FPGA_GPIO_CHARGE_EMPTY            ((uint32_t)(1u <<  1))
#define    PSS_MCU_FPGA_GPIO_LD_HEAT                 ((uint32_t)(1u <<  2))
#define    PSS_MCU_FPGA_GPIO_R_CTRL_20               ((uint32_t)(1u <<  3))
#define    PSS_MCU_FPGA_GPIO_R_CTRL_60               ((uint32_t)(1u <<  4))
#define    PSS_MCU_FPGA_GPIO_R_CTRL_600              ((uint32_t)(1u <<  5))
#define    PSS_MCU_FPGA_GPIO_I_CTRL_20               ((uint32_t)(1u <<  6))
#define    PSS_MCU_FPGA_GPIO_I_CTRL_60               ((uint32_t)(1u <<  7))
#define    PSS_MCU_FPGA_GPIO_I_CTRL_600              ((uint32_t)(1u <<  8))
#define    PSS_MCU_FPGA_GPIO_DISCHARGE               ((uint32_t)(1u <<  9))
#define    PSS_MCU_FPGA_GPIO_CHARGE                  ((uint32_t)(1u << 10))
#define    GPIO_SIZE                                           (11u)
#endif

#ifdef MCU_SOUR
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_A_CTR          ((uint32_t)(1u <<  0))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_B_CTR          ((uint32_t)(1u <<  1))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_C_CTR          ((uint32_t)(1u <<  2))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_D_CTR          ((uint32_t)(1u <<  3))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_E_CTR          ((uint32_t)(1u <<  4))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_F_CTR          ((uint32_t)(1u <<  5))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_G_CTR          ((uint32_t)(1u <<  6))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_H_CTR          ((uint32_t)(1u <<  7))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_I_CTR          ((uint32_t)(1u <<  8))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_J_CTR          ((uint32_t)(1u <<  9))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_K_CTR          ((uint32_t)(1u << 10))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_L_CTR          ((uint32_t)(1u << 11))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_M_CTR          ((uint32_t)(1u << 12))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_N_CTR          ((uint32_t)(1u << 13))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_O_CTR          ((uint32_t)(1u << 14))
#define    PSS_MCU_FPGA_GPIO_FPGA_DUT_P_CTR          ((uint32_t)(1u << 15))
#if 0
#define    PSS_MCU_FPGA_GPIO_FPGA_IO1                ((uint32_t)(1u << 16))
#define    PSS_MCU_FPGA_GPIO_FPGA_IO2                ((uint32_t)(1u << 17))
#define    PSS_MCU_FPGA_GPIO_FPGA_IO3                ((uint32_t)(1u << 18))
#define    PSS_MCU_FPGA_GPIO_FPGA_IO4                ((uint32_t)(1u << 19))
#define    PSS_MCU_FPGA_GPIO_FPGA_IO5                ((uint32_t)(1u << 20))
#define    PSS_MCU_FPGA_GPIO_FPGA_IO6                ((uint32_t)(1u << 21))
#define    PSS_MCU_FPGA_GPIO_FPGA_IO7                ((uint32_t)(1u << 22))
#define    PSS_MCU_FPGA_GPIO_FPGA_IO8                ((uint32_t)(1u << 23))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH1              ((uint32_t)(1u << 24))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH1              ((uint32_t)(1u << 25))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH2              ((uint32_t)(1u << 26))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH2              ((uint32_t)(1u << 27))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH3              ((uint32_t)(1u << 28))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH3              ((uint32_t)(1u << 29))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH4              ((uint32_t)(1u << 30))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH4              ((uint32_t)(1u << 31))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH5              ((uint32_t)(1u <<  0))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH5              ((uint32_t)(1u <<  1))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH6              ((uint32_t)(1u <<  2))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH6              ((uint32_t)(1u <<  3))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH7              ((uint32_t)(1u <<  4))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH7              ((uint32_t)(1u <<  5))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH8              ((uint32_t)(1u <<  6))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH8              ((uint32_t)(1u <<  7))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH9              ((uint32_t)(1u <<  8))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH9              ((uint32_t)(1u <<  9))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH10             ((uint32_t)(1u << 10))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH10             ((uint32_t)(1u << 11))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH11             ((uint32_t)(1u << 12))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH11             ((uint32_t)(1u << 13))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH12             ((uint32_t)(1u << 14))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH12             ((uint32_t)(1u << 15))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH13             ((uint32_t)(1u << 16))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH13             ((uint32_t)(1u << 17))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH14             ((uint32_t)(1u << 18))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH14             ((uint32_t)(1u << 19))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH15             ((uint32_t)(1u << 20))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH15             ((uint32_t)(1u << 21))
#define    PSS_MCU_FPGA_GPIO_FPGA_W_CH16             ((uint32_t)(1u << 22))
#define    PSS_MCU_FPGA_GPIO_FPGA_R_CH16             ((uint32_t)(1u << 23))
#endif
#define    PSS_MCU_FPGA_GPIO_PD_0_1MA                ((uint32_t)(1u << 24))
#define    PSS_MCU_FPGA_GPIO_PD___1MA                ((uint32_t)(1u << 25))
#define    PSS_MCU_FPGA_GPIO_PD__10MA                ((uint32_t)(1u << 26))
#define    PSS_MCU_FPGA_GPIO_DUT___3V                ((uint32_t)(1u << 27))
#define    PSS_MCU_FPGA_GPIO_DUT__30V                ((uint32_t)(1u << 28))
#define    PSS_MCU_FPGA_GPIO_DUT_100V                ((uint32_t)(1u << 29))
#define    PSS_MCU_FPGA_GPIO_OUTP_DIS_O              ((uint32_t)(1u << 30))
#define    PSS_MCU_FPGA_GPIO_OUTP_STA_I              ((uint32_t)(1u << 31))
#define    GPIO_SIZE                                           (64u)
#endif
/*********************************** 类型定义 **********************************/
typedef enum _PSS_MCU_DRVS_GPIO_STATE_TAG_
{
    PSS_MCU_DRVS_GPIO_STATE_L = 0,      /* 低电平 */
    PSS_MCU_DRVS_GPIO_STATE_H = 1,      /* 高电平 */
} PSS_MCU_DRVS_GPIO_STATE_T;            /* 电平 */

typedef enum _PSS_MCU_DRVS_GPIO_DIR_TAG_
{
    PSS_MCU_DRVS_GPIO_DIR_OUT = 1,      /* 输出*/
    PSS_MCU_DRVS_GPIO_DIR_IN  = 0,      /* 输入*/
} PSS_MCU_DRVS_GPIO_DIR_T;              /* GPIO方向 */

typedef struct _PSS_MCU_DRVS_FPGA_GPIO_TAG_
{
    void                       *phy;    /* 底层驱动 或 物理层 */
    PSS_MCU_DRVS_GPIO_DIR_T     dir;    /* 方向 */
    PSS_MCU_DRVS_GPIO_STATE_T   state;  /* 状态电平 */
} PSS_MCU_DRVS_FPGA_GPIO_T;                  /* GPIO */


typedef struct _PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_TAG_
{
    volatile uint32_t *regDir;               /* 方向寄存器 */
    volatile uint32_t *regVal;               /* 电平寄存器 */
    volatile uint32_t  pin;                   /* 管腿号 */
} PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T;   /* GPIO */

/*--------------------------------- 接口声明区 --------------------------------*/

/*********************************** 全局变量 **********************************/

/*********************************** 接口函数 **********************************/
void    PssMcuDrvsGpioInit(PSS_MCU_DRVS_FPGA_GPIO_T *gpio, void *phy, PSS_MCU_DRVS_GPIO_DIR_T dir, PSS_MCU_DRVS_GPIO_STATE_T state);
bool PssMcuDrvsGpioWrite(PSS_MCU_DRVS_FPGA_GPIO_T *gpio, PSS_MCU_DRVS_GPIO_STATE_T state);
bool PssMcuDrvsGpioToggle(PSS_MCU_DRVS_FPGA_GPIO_T *gpio);
PSS_MCU_DRVS_GPIO_STATE_T PssMcuDrvsGpioRead(const PSS_MCU_DRVS_FPGA_GPIO_T *gpio);

#endif // #ifndef _PSS_MCU_DRVS_GPIO_H_

#ifdef __cplusplus
}
#endif

