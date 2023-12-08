/******************************************************************************
 *
 * 文件名  ： PssMcuDrvsFpgaAds1259.h
 * 负责人  ： myk
 * 创建日期： 20210918
 * 版本号  ： v1.0
 * 文件描述： PSS ADS1259驱动接口
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/

/*---------------------------------- 预处理区 ---------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _PSS_MCU_DRVS_ADS1259_H_
#define _PSS_MCU_DRVS_ADS1259_H_

/************************************ 头文件 ***********************************/
#include <stdint.h>
/************************************ 宏定义 ***********************************/

#define   MCU_DRVS_ADS1259_BASE1        (0x60004000)
#define   MCU_DRVS_ADS1259_BASE2        (0x60005000)
#define   MCU_DRVS_ADS1259_BASE3        (0x60006000)
/*********************************** 类型定义 **********************************/
typedef enum _PSS_MCU_NPLC_TYPE_TAG_
{
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_14400SPS          = 0x07,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_3600SPS           = 0x06,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_1200SPS           = 0x05,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_400SPS            = 0x04,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_60SPS             = 0x03,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_50SPS             = 0x02,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_16E6SPS           = 0x01,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_10SPS             = 0x00,
} PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_ENUM;

typedef struct
{
    volatile  uint32_t cmdReg;                /* 命令寄存器 */
    volatile  uint32_t dataReg;               /* 采样数据FIFO寄存器 [23:0]采样值  [31:24] 数据放大倍数*/
    volatile  uint32_t realDataReg;           /* 实时采样数据寄存器*/
    volatile  uint32_t fifoStausReg;          /* FIFO状态寄存器 */
    volatile  uint32_t readDealy;             /* 采样读取延时寄存器 预留 */
    volatile  uint32_t readNum;               /* 采样读取个数寄存器 预留*/
    volatile  uint32_t fifoReset;             /* FIFO清空寄存器 */
    volatile  uint32_t watchReg;              /* 帧测寄存器 */
    volatile  uint32_t ReadBackReg;           /* 回读数据寄存器 */
    volatile  uint32_t SpiSendReg;            /* SPI发送完成寄存器 */
    volatile  uint32_t ClearSpiReg;           /* 清除SPI发送寄存器 */
    volatile  uint32_t InitSuccessReg;        /* 初始化完成寄存器 */
}Ad1259_TypeDef;

typedef struct _PSS_MCU_DRVS_ADS1259_TAG_
{
    Ad1259_TypeDef       *ad1259Hw;         /* 寄存器 */
    uint32_t             originData;        /* 原始采样值 */
} PSS_MCU_DRVS_ADS1259_T;             /* 普塞斯 AD1259驱动 */

/*--------------------------------- 接口声明区 --------------------------------*/

/*********************************** 全局变量 **********************************/

/*********************************** 接口函数 **********************************/
int PssMcuDrvsAds1259Init(PSS_MCU_DRVS_ADS1259_T *ads1259, uint32_t addr);
int PssMcuDrvsAds1259Update(PSS_MCU_DRVS_ADS1259_T *ads1259);
int PssMcuDrvsAds1259Read(PSS_MCU_DRVS_ADS1259_T *ads1259,  float *vVal);

#endif // #ifndef _PSS_MCU_DRVS_ADS1259_H_

#ifdef __cplusplus
}
#endif

