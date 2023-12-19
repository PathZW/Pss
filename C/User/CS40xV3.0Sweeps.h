/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS40X_V3_0_SWEEPS_H__
#define __CS40X_V3_0_SWEEPS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



/***********************************************************************
 * @}
 * @addtogroup              Sweeps
 * @{
 **********************************************************************/
#ifndef SMU_LIST_SIZE
    #define SMU_LIST_SIZE       (1ul * 2)       // 最大扫描列表大小
#else
    #define SMU_LIST_SIZE       (1024ul * 2)    // 最大扫描列表大小
#endif
  
//!< 区间扫描方向
typedef enum SWE_zone_{                     //!< 区间扫描方向
    ZONE_UP   = 0,                          //!< - 向上, 起点 --> 终点
    ZONE_DOW  = 1,                          //!< - 向下, 终点 --> 起点
    ZONE_DUAL = 2,                          //!< - 双向, 起点 --> 终点 --> 起点
} SWE_zone_e;
  
//!< 自定义扫描
typedef struct SWE_list_{                   //!< 输出列表
    uint32_t    mCnt;                       //!< - 输出次数
    double      mVal[SMU_LIST_SIZE];        //!< - 输出源值
} SWE_list_t;
  
typedef struct SWE_info_{                   //!< 区间扫描参数
    uint32_t        mLoopCount;             //!< - 扫描循环次数(连续N个点循环)
    uint32_t        mRepCount;              //!< - 扫描重复次数
    SWE_list_t      *const mList;           //!< - 输出列表（线性扫描取前2个点值，分别为起点、终点值）, 是否考虑脉冲参数可以每个点周期，脉宽，DC稳定时间不同?)
    uint32_t        mPoin;                  //!< - 扫描点数 (是否增加步进V/I?)
    SWE_zone_e      mDire;                  //!< - 扫描方向
    uint32_t        mSCnt;                  //!< - 扫描采样数据读取计数
} SWE_info_t;





/******************** 函数声明 ******************/
/**
  * @功能: 扫描参数设置
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 配置索引
           [in ] txData 配置参数
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_sweSetPara(const SWE_info_t *anlgDev, void *txIndex, void *txData);

/**
  * @功能: 回读扫描参数
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 查询索引
           [out] rxdata 读取数据
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_sweReadPara(const SWE_info_t *anlgDev, void *txIndex, void *rxdata);


#ifdef __cplusplus
}
#endif
#endif /*__CS40X_V3_0_SWEEPS_H__ */
