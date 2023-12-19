/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS40X_V3_0_SOURCE_H__
#define __CS40X_V3_0_SOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "CS40xV3.0Rang.h"


/**********************************************************************
  * @}
  * @addtogroup              Source
  * @{   
  *********************************************************************/
typedef struct SOUR_info_{                  //!< SOUR Info
    float               mLevVal;            //!< - 源值(V or A)
    float               mLimVal;            //!< - 限值(V or A)
    float               mDlyDCSt;           //!< - 直流稳定时间(S) 针对某些项目需要等待直流稳定后，方可开PWM
    float               mDlyFallSt;         //!< - 关闭时间(s)（暂不实现）
    float               mPulsWidt;          //!< - 脉冲宽度(S)
    float               mPulsPeri;          //!< - 脉冲周期(S)
    uint32_t            mPulsNum;           //!< - 脉冲输出个数(/* 此处直流是否也有输出个数，根据NPLC来定义输出时间 */)
    bool                bPulsAll;           //!< - 是否全周期测量
    float               mBiasLev;           //!< - 偏置源值(V or A)
    float               mBiasLim;           //!< - 偏置限值(V or A)
    bool                bEStop;             //!< - 是否急停
    bool                bAZero;             //!< - 是否自动归零
} SOUR_info_t;


/******************** 函数声明 ******************/
/**
  * @功能: 源参数设置
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 配置索引
           [in ] txData 配置参数
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_sourSetPara(const SOUR_info_t *anlgDev, void *txIndex, void *txData);

/**
  * @功能: 回读源参数
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 查询索引
           [out] rxdata 读取数据
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_sourReadPara(const SOUR_info_t *anlgDev, void *txIndex, void *rxdata);

#ifdef __cplusplus
}
#endif
#endif /*__CS40X_V3_0_SOURCE_H__ */

