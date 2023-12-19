/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS40X_V3_0_RANG_H__
#define __CS40X_V3_0_RANG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "CS40xV3.0AnlgDev.h"

/*******************************************************************************
 * @}
 * @addtogroup              Range
 * @{
 ******************************************************************************/
//!< 量程类型
typedef enum RANG_mode_{            //!< 量程类型
    RANG_FIX       = 0,             //!< - 固定
    RANG_AUTO      = 1,             //!< - 自动(只针对限量程)
    RANG_AUTO_UP   = 2,             //!< - 自动上切(应用于容性负载大电流充电，小电流测量，指定N个量程区间中切换)
    RANG_AUTO_DOWN = 3,             //!< - 自动下切
    RANG_BEST      = 4,             //!< - 最佳
} RANG_mode_e;

//!< 自动量程模式
typedef struct RANG_autoMode_{        //!< 自动量程模式
    bool                bRangAuto;          //!< - 是否自动量程
    float               mRangHi;            //!< - 自动量程高值
    float               mRangLo;            //!< - 自动量程低值
    bool                bRangRe;            //!< - 自动量程是否恢复(自动量程反弹)
} RANG_autoMode_t;

//!< 量程切档表
typedef struct RANG_switch_{            //!< 量程切档表
    void                (*fun)(uint32_t arg); //
    uint32_t            arg;                  //
} RANG_switch_t;

//!< 量程信息
typedef struct RANG_info_{            //!< 量程信息
    float               mMaxVal;      //!< - 最大值
    float               mHiOver;      //!< - 高切换值
    float               mLoOver;      //!< - 低切换值
    double              mCalK;        //!< - 校准系数 K(C1)
    double              mCalB;        //!< - 校准系数 B(C0)
    int32_t             mCalM;        //!< - 校准系数 M(增益)
    int64_t             mCalKmN;      //!< - 校准系数 Km * N
    int64_t             mCalBmN;      //!< - 校准系数 Bm * N
    RANG_switch_t       *mSwListUp;   //!< - 切换操作表, 结束于{ NULL, 0 }
    RANG_switch_t       *mSwListDown; //!< - 切换操作表, 结束于{ NULL, 0 }
} RANG_info_t;

typedef struct RANG_stat_ {           //!< 量程状态(Run-Time)
    unsigned            mIdxCur;      //!< - 当前索引
    unsigned            mIdxCfg;      //!< - 配置索引     :SOUR:{VOLT | CURR}:RANG 
    RANG_mode_e         mMode;        //!< - 量程类型
    unsigned      const mIdxMax;      //!< - 最大索引
    RANG_info_t  *const mInfoList;    //!< - 量程信息表
    void        (*const mExFun)(struct RANG_stat_ *rang); //!< 扩展切换操作处理函数
} RANG_stat_t;

 
/******************** 函数声明 ******************/
/**
  * @功能: 量程参数默认配置
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 配置索引
           [in ] txData 配置参数
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: 例如根据加密信息，生成量程信息列表 
  **/
extern int SMU_rangeCfgPara(const RANG_stat_t *anlgRang, void *txIndex, void *txData);

/**
  * @功能: 设置量程参数
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 设置索引
           [in ] txData 设置参数
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: 设置值放入变量保存
  **/
extern int SMU_rangeSetPara(const RANG_stat_t *anlgRang, void *txIndex, void *txData);

/**
  * @功能: 获取参数保存值
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 查询索引
           [out] rxdata 获取数据
  * @返回值: (>= 0) 成功
            (<= -1) 失败
  * @其它: None
  **/
extern int SMU_rangeGetPara(const RANG_stat_t *anlgRang, void *txIndex, void *rxdata);

/**
  * @功能: 回读量程参数
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 查询索引
           [out] rxdata 获取数据
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_rangeReadPara(const RANG_stat_t *anlgRang, void *txIndex, void *rxdata);


/**
  * @功能: 量程切档
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] desIndex 目标量程索引
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_rangeChange(const RANG_stat_t *anlgRang, void *desIndex);


/**
  * @功能 自动量程切档
  * @参数: [in ] anlgDev 模拟板设备句柄
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_rangeAuto(const RANG_stat_t *anlgRang);


#ifdef __cplusplus
}
#endif
#endif /*__CS40X_V3_0_RANG_H__ */
