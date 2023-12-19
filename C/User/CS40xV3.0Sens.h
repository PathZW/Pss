/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS40X_V3_0_SENS_H__
#define __CS40X_V3_0_SENS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "CS40xV3.0Rang.h"

/***********************************************************************
 * @}
 * @addtogroup              Sens
 * @{
 **********************************************************************/
typedef struct SENS_info_{                  //!< SENS Info
    RANG_autoMode_t     mRangAuto;          //!< - 自动量程参数
    bool                bCAbort;            //!< - 超限停止
    float               mDlyRis;            //!< - 上升沿稳定时间(S)
    float               mDlyFall;           //!< - 下降沿稳定时间(S)
    uint32_t            mSCnt;              //!< - 读数个数(单个周期)
    float               mNplc;              //!< - NPLC 值(CPx00无NPLC，只有采样点)
    uint32_t            mAvgCnt;            //!< - 滤波平均数(单个读数，N个ADC采样点平均)
    uint32_t            mSPoint;            //!< - ADC采样点数(单个周期)
    uint32_t            mSRate;             //!< - ADC采样速率
} SENS_info_t;


/******************** 函数声明 ******************/
/**
  * @功能: Sens参数设置
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 配置索引
           [in ] txData 配置参数
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_sensSetPara(const SENS_info_t *anlgDev, void *txIndex, void *txData);

/**
  * @功能: 回读Sens参数
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 查询索引
           [out] rxdata 读取数据
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_sensReadPara(const SENS_info_t *anlgDev, void *txIndex, void *rxdata);

#ifdef __cplusplus
}
#endif
#endif /*__CS40X_V3_0_SENS_H__ */
