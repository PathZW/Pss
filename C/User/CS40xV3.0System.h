/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS40X_V3_0_SYSTEM_H__
#define __CS40X_V3_0_SYSTEM_H__

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
 * @addtogroup              System
 * @{
 **********************************************************************/
//!< 系统开关
typedef struct SYST_switch_{
    bool                bIsFPanel;          //!< - 前/后面板输出
    bool                bIs4Line;           //!< - 是否使用4线制输出
    bool                bBeepEn;            //!< - 蜂鸣器使能
} SYST_switch_t;

//!< 系统信息
typedef struct SYST_info_t{
    SYST_switch_t       mSysSw;            //!< - 系统使能
    uint32_t            mBeepPeri;         //!< - 蜂鸣器周期
    uint32_t            mBeepWidt;         //!< - 蜂鸣器脉宽
    uint32_t            mCap;              //!< - 电容补偿值(PF)
    uint32_t            mRsen;             //!< - 设置接触检测阈值电阻(暂不实现)
} SYST_info_t;


/******************** 函数声明 ******************/
/**
  * @功能: 系统参数设置
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 配置索引
           [in ] txData 配置参数
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_sysSetPara(const SYST_info_t *anlgDev, void *txIndex, void *txData);

/**
  * @功能: 回读系统参数
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 查询索引
           [out] rxdata 读取数据
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_sysReadPara(const SYST_info_t *anlgDev, void *txIndex, void *rxdata);


#ifdef __cplusplus
}
#endif
#endif /*__CS40X_V3_0_SYSTEM_H__ */
