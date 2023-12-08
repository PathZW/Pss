/******************************************************************************
 *
 * 文件名  ： Drv_Ad5683.h
 * 负责人  ： myk
 * 创建日期： 20200827
 * 版本号  ： v1.0
 * 文件描述： PSS校准模块
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/

/*---------------------------------- 预处理区 ---------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _PSS_MCU_APP_MCAL_H_
#define _PSS_MCU_APP_MCAL_H_

/************************************ 头文件 ***********************************/
#include <stdint.h>
#include <stdbool.h>
#include "emb_rtcfg.h"

typedef enum _PSS_BIN_RANGE_CAL_INDEX_TAG_
{
    /* 3组电压量程系数 */
    PSS_MCU_V_RANGE_3V                 = 0,
    PSS_MCU_V_RANGE_30V                = 1,
    PSS_MCU_V_RANGE_100V               = 2,
    /* 光功率系数 */
    PSS_MCU_OPD_RANGE_0_1mW            = 3,
    PSS_MCU_OPD_RANGE_1_0mW            = 4,
    PSS_MCU_OPD_RANGE_10mW             = 5,
    /* 12组分板系数 */
    PSS_MCU_1_RANGE_5A                 = 6,
    PSS_MCU_1_RANGE_10A                = 7,
    PSS_MCU_1_RANGE_15A                = 8,
    PSS_MCU_1_RANGE_20A                = 9,

    PSS_MCU_2_RANGE_5A                 = 10,
    PSS_MCU_2_RANGE_10A                = 11,
    PSS_MCU_2_RANGE_15A                = 12,
    PSS_MCU_2_RANGE_20A                = 13,
    PSS_MCU_2_RANGE_25A                = 14,
    PSS_MCU_2_RANGE_30A                = 15,
    PSS_MCU_2_RANGE_35A                = 16,
    PSS_MCU_2_RANGE_40A                = 17,
    PSS_MCU_2_RANGE_45A                = 18,
    PSS_MCU_2_RANGE_50A                = 19,
    PSS_MCU_2_RANGE_55A                = 20,
    PSS_MCU_2_RANGE_60A                = 21,

    PSS_MCU_3_RANGE_50A                = 22,
    PSS_MCU_3_RANGE_100A               = 23,
    PSS_MCU_3_RANGE_150A               = 24,
    PSS_MCU_3_RANGE_200A               = 25,
    PSS_MCU_3_RANGE_250A               = 26,
    PSS_MCU_3_RANGE_300A               = 27,
    PSS_MCU_3_RANGE_350A               = 28,
    PSS_MCU_3_RANGE_400A               = 29,
    PSS_MCU_3_RANGE_450A               = 30,
    PSS_MCU_3_RANGE_500A               = 31,
    PSS_MCU_3_RANGE_550A               = 32,
    PSS_MCU_3_RANGE_600A               = 33,

    PSS_MCU_READ_1_RANGE_5A            = 34,
    PSS_MCU_READ_1_RANGE_10A           = 35,
    PSS_MCU_READ_1_RANGE_15A           = 36,
    PSS_MCU_READ_1_RANGE_20A           = 37,

    PSS_MCU_READ_2_RANGE_5A            = 38,
    PSS_MCU_READ_2_RANGE_10A           = 39,
    PSS_MCU_READ_2_RANGE_15A           = 40,
    PSS_MCU_READ_2_RANGE_20A           = 41,
    PSS_MCU_READ_2_RANGE_25A           = 42,
    PSS_MCU_READ_2_RANGE_30A           = 43,
    PSS_MCU_READ_2_RANGE_35A           = 44,
    PSS_MCU_READ_2_RANGE_40A           = 45,
    PSS_MCU_READ_2_RANGE_45A           = 46,
    PSS_MCU_READ_2_RANGE_50A           = 47,
    PSS_MCU_READ_2_RANGE_55A           = 48,
    PSS_MCU_READ_2_RANGE_60A           = 49,

    PSS_MCU_READ_3_RANGE_50A           = 50,
    PSS_MCU_READ_3_RANGE_100A          = 51,
    PSS_MCU_READ_3_RANGE_150A          = 52,
    PSS_MCU_READ_3_RANGE_200A          = 53,
    PSS_MCU_READ_3_RANGE_250A          = 54,
    PSS_MCU_READ_3_RANGE_300A          = 55,
    PSS_MCU_READ_3_RANGE_350A          = 56,
    PSS_MCU_READ_3_RANGE_400A          = 57,
    PSS_MCU_READ_3_RANGE_450A          = 58,
    PSS_MCU_READ_3_RANGE_500A          = 59,
    PSS_MCU_READ_3_RANGE_550A          = 60,
    PSS_MCU_READ_3_RANGE_600A          = 61,

    PSS_MCU_PT100_A1                   = 62,
    PSS_MCU_PT100_A2                   = 63,
    PSS_MCU_PT100_A3                   = 64,
    PSS_MCU_PT100_A4                   = 65,
    PSS_MCU_PT100_A5                   = 66,
    PSS_MCU_PT100_A6                   = 67,
    PSS_MCU_PT100_A7                   = 68,
    PSS_MCU_PT100_A8                   = 69,
    PSS_MCU_PT100_A9                   = 70,
    PSS_MCU_PT100_A10                  = 71,
    PSS_MCU_PT100_A11                  = 72,
    PSS_MCU_PT100_A12                  = 73,
    PSS_MCU_PT100_A13                  = 74,
    PSS_MCU_PT100_A14                  = 75,
    PSS_MCU_PT100_A15                  = 76,
    PSS_MCU_PT100_A16                  = 77,
    PSS_MCU_PT100_B1                   = 78,
    PSS_MCU_PT100_B2                   = 79,
    PSS_MCU_PT100_B3                   = 80,
    PSS_MCU_PT100_B4                   = 81,
    PSS_MCU_PT100_B5                   = 82,
    PSS_MCU_PT100_B6                   = 83,
    PSS_MCU_PT100_B7                   = 84,
    PSS_MCU_PT100_B8                   = 85,
    PSS_MCU_PT100_B9                   = 86,
    PSS_MCU_PT100_B10                  = 87,
    PSS_MCU_PT100_B11                  = 88,
    PSS_MCU_PT100_B12                  = 89,
    PSS_MCU_PT100_B13                  = 90,
    PSS_MCU_PT100_B14                  = 91,
    PSS_MCU_PT100_B15                  = 92,
    PSS_MCU_PT100_B16                  = 93,

    PSS_BIN_RANGE_CAL_INDEX_MAX        = 94
} PSS_MCU_RANGE_CAL_INDEX_T;

typedef struct _PSS_MCU_CAL_PARA_TAG_
{
    PSS_MCU_RANGE_CAL_INDEX_T   mIndex;            /* 校准系数索引 */
    double                      mC0;               /* 0次系数 */
    double                      mC1;               /* 1次系数 */
}PSS_MCU_CAL_PARA_T;

typedef struct _PSS_MCU_CAL_TAG_
{
    PSS_MCU_CAL_PARA_T  calPara[PSS_BIN_RANGE_CAL_INDEX_MAX];/* 校准系数 */

} PSS_MCU_CAL_T;                                  /* CAL通道 */

/************************************ 宏定义 ***********************************/
int PssMcuCalInit(PSS_MCU_CAL_T* cal);
double Load2CalVal(PSS_MCU_CAL_T *cal, double load, uint32_t index);
void PssMcuCalFire(PSS_MCU_CAL_T *cal);
void PssMcuCalSet(PSS_MCU_CAL_T *cal, uint32_t index, double c1, double c0);
double PssMcuCalReadC1(PSS_MCU_CAL_T *cal, uint32_t index);
double PssMcuCalReadC0(PSS_MCU_CAL_T *cal, uint32_t index);
void PssAnlgCalReset(PSS_MCU_CAL_T *cal, uint32_t index);

#endif // #ifndef _PSS_MCU_APP_MCAL_H_

#ifdef __cplusplus
}
#endif

