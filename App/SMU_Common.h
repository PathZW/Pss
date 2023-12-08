/*******************************************************************************
 * @file    SMU_Common.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/29
 * @brief   Source Meter Unit common
 *******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) Wuhan Precise Co.,Ltd</center></h2>
 *
 * The information and source code contained herein is the exclusive
 * property of Wuhan Precise Electronic Technology Co.,Ltd. And may
 * not be disclosed, examined or reproduced in whole or in part without
 * explicit written authorization from the company.
 *
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2023/3/29 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __SMU_COMMON_H__
#define __SMU_COMMON_H__
/******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "SMU_Range.h"

#ifndef SMU_LIST_SIZE
#define SMU_LIST_SIZE       (1024ul * 2)    // 最大扫描列表大小
#endif

/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup SMU_Common
 * @{
 * @addtogroup              Common
 * @{
 ******************************************************************************/

//!< SMU Shap 类型
typedef enum {                  //!< SMU 类型
    SMU_DC    = 0,              //!< - 直流
    SMU_PULS  = 1,              //!< - 脉冲
} SMU_SHAP_E;

//!< SMU Func 类型
typedef enum {                  //!< 源类型
    SMU_VOLT = 0,               //!< - 电压源
    SMU_CURR = 1,               //!< - 电流源
} SMU_FUNC_E;

typedef enum {                  //!< 输出模式
    OUT_FIX   = 0,              //!< - 恒定输出
    SWE_LIST  = 1,              //!< - 列表输出
    SWE_LIN   = 2,              //!< - 扫描:线性
    SWE_LOG   = 4,              //!< - 扫描:对数
    SWE_SIN   = 6,              //!< - 扫描:正弦波
    SWE_SQU   = 7,              //!< - 扫描:方波
    SWE_TRI   = 8,              //!< - 扫描:三角波
    SWE_RIMP  = 9,              //!< - 扫描:锯齿波
} SMU_MODE_E;

//lint -emacro(835, SMU_CHNL)
typedef uint32_t      SMU_CHNL_T;           //!< 通道数据类型
#define SMU_CHNL(n) ((SMU_CHNL_T)1 << (n))  //!< 通道 n 参数
#define SMU_CHNL_ALL   (0xFFFFFFFFul)       //!< 所有通道


/*******************************************************************************
 * @}
 * @addtogroup              Range
 * @{
 ******************************************************************************/

//!< 量程类型
typedef enum {                      //!< 量程类型
    RANG_FIX  = 0,                  //!< - 固定
    RANG_AUTO = 1,                  //!< - 自动
    RANG_BEST = 2,                  //!< - 最佳
    RANG_UP   = 3,                  //!< - 上切
    RANG_DOWN = 4,                  //!< - 下切
} RANG_MODE_E;

typedef const struct {                      //
    void              (*fun)(uint32_t arg); //
    uint32_t            arg;                //
} RANG_SWITCH_T;

typedef struct {                    //!< 量程信息
    float               mVal;       //!< - 最大值
    float               mHiOver;    //!< - 高切换值
    float               mLoOver;    //!< - 低切换值
    double              mCalK;      //!< - 校准系数 K(C1)
    double              mCalB;      //!< - 校准系数 B(C0)
    int32_t             mCalM;      //!< - 校准系数 M
    int64_t             mCalKmN;    //!< - 校准系数 Km * N
    int64_t             mCalBmN;    //!< - 校准系数 Bm * N
    RANG_SWITCH_T      *mSwList;    //!< - 切换操作表, 结束于{ NULL, 0 }
} RANG_INFO_T;

typedef struct RANG_STAT_T_ {       //!< 量程状态(Run-Time)
    unsigned            mIdxCur;    //!< - 当前索引
    unsigned            mIdxCfg;    //!< - 配置索引
    RANG_MODE_E         mMode;      //!< - 量程类型
    unsigned      const mIdxMax;    //!< - 最大索引
    RANG_INFO_T  *const mInfoList;  //!< - 量程信息表
    void        (*const mExFun)(struct RANG_STAT_T_ *rang); //!< 扩展切换操作处理函数
} RANG_STAT_T;
 
void  SMU_RangCfg   (RANG_INFO_T *info, float val, float hiOver, float loOver);
void  SMU_RangCalSet(RANG_INFO_T *info, double K, double B);

void        SMU_RangRst    (RANG_STAT_T *rang);
RANG_MODE_E SMU_RangModeGet(RANG_STAT_T *rang);
unsigned    SMU_RangIdxGet (RANG_STAT_T *rang);
float       SMU_RangValGet (RANG_STAT_T *rang);
double      SMU_RangCalVal (RANG_STAT_T *rang, double val);
bool        SMU_RangChange (RANG_STAT_T *rang, RANG_MODE_E mode, float val);
bool        SMU_RangSwitch (RANG_STAT_T *rang);


/*******************************************************************************
 * @}
 * @addtogroup              Sweeps
 * @{
 ******************************************************************************/

//!< 区间扫描方向
typedef enum {                  //!< 区间扫描方向
    ZONE_UP   = 0,              //!< - 向上, 起点 --> 终点
    ZONE_DOW  = 1,              //!< - 向下, 终点 --> 起点
    ZONE_DUAL = 2,              //!< - 双向, 起点 --> 终点 --> 起点
} SWE_ZONE_E;

typedef struct {                //!< 区间扫描参数
    RANG_MODE_E     mRangM;     //!< - 扫描量程类型
    uint32_t        mCount;     //!< - 扫描重复次数
    bool            bCAbort;    //!< - 超限停止
    float           mStart;     //!< - 扫描起点
    float           mStop;      //!< - 扫描终点
    uint32_t        mPoin;      //!< - 扫描点数
    SWE_ZONE_E      mDire;      //!< - 扫描方向
} SWE_ZONE_T;

#define SWE_ZONE_DFT {          /*!< 默认(区间扫描参数) */ \
      RANG_FIX                  /*!< - 扫描量程类型     */ \
    , 1u                        /*!< - 扫描重复次数     */ \
    , false                     /*!< - 超限停止         */ \
    , 0.0f                      /*!< - 扫描起点         */ \
    , 0.0f                      /*!< - 扫描终点         */ \
    , 1u                        /*!< - 扫描点数         */ \
    , ZONE_UP                   /*!< - 扫描方向         */ \
}


/*******************************************************************************
 * @}
 * @addtogroup              Source
 * @{
 ******************************************************************************/
//typedef enum {                //!< 输出状态
//    SOUR_CLOSED   = 0,        //!< - 已经关闭
//    SOUR_STARTED  = 1,        //!< - 已经启动
//    SOUR_CLOSING  = 2,        //!< - 正在关闭
//    SOUR_STARTING = 3,        //!< - 正在启动
//} SOUR_STAT_E;

typedef struct {                            //!< SENS Info
    SMU_FUNC_E          mFunc;              //!< - 类型{ SMU_VOLT | SMU_CURR }
    bool                bRsens;             //!< - 是否使用4线制测量
    bool                bAZero;             //!< - 是否自动归零
    float               mRang;              //!< - 量程
    bool                bRangA;             //!< - 是否自动量程
    float               mRangHi;            //!< - 自动量程高值
    float               mRangLo;            //!< - 自动量程低值
    bool                bRangRe;            //!< - 自动量程是否恢复
    float               mNplc;              //!< - NPLC 值    - 仅 SMU_DC 模式
    uint32_t            mAvgCnt;            //!< - 滤波平均数 - 仅 SMU_DC 模式
    uint32_t            mSRate;             //!< - 采样速率
} SENS_INFO_T;

typedef struct {                            //!< SOUR Info
    SMU_FUNC_E          mFunc;              //!< - 类型{ SMU_VOLT | SMU_CURR }
    float               mLevVal;            //!< - 源值(V or A)
    RANG_STAT_T  *const mLevRang;           //!< - 源量程
    float               mLimVal;            //!< - 限值(V or A)
    RANG_STAT_T  *const mLimRang;           //!< - 限量程
    float               mDlyDC;             //!< - 直流稳定时间(S)
    float               mDlyPuls;           //!< - 下降沿稳定时间(S)
    float               mPulsWidt;          //!< - 脉冲宽度(S)
    float               mPulsPeri;          //!< - 脉冲周期(S)
    uint32_t            mPulsNum;           //!< - 脉冲序列数
    bool                bPulsAll;           //!< - 是否全周期测量
    float               mBiasLev;           //!< - 偏置源值(V or A)
    float               mBiasLim;           //!< - 偏置限值(V or A)
} SOUR_INFO_T;

//!< SOUR Info 定义
#define SOUR_INFO_DFT(levRang, limRang) {   /*!< 默认(SOUR Info)     */ \
      SMU_VOLT       /* mFunc     */        /*!< - 类型              */ \
    , 0.0f           /* mLevVal   */        /*!< - 源值(V or A)      */ \
    , levRang        /* mLevRang  */        /*!< - 源量程            */ \
    , 0.0f           /* mLimVal   */        /*!< - 限值(V or A)      */ \
    , limRang        /* mLimRang  */        /*!< - 限量程            */ \
    , 1e-6f          /* mDlyDC    */        /*!< - 直流稳定时间(S)   */ \
    , 1e-6f          /* mDlyPuls  */        /*!< - 下降沿稳定时间(S) */ \
    , 1e-3f          /* mPulsWidt */        /*!< - 脉冲宽度(S)       */ \
    , 2e-3f          /* mPulsPeri */        /*!< - 脉冲周期(S)       */ \
    , 1u             /* mPulsNum  */        /*!< - 脉冲序列数        */ \
    , false          /* bPulsAll  */        /*!< - 是否全周期测量    */ \
    , 0.0f           /* mBiasLev  */        /*!< - 偏置源值(V or A)  */ \
    , 0.0f           /* mBiasLim  */        /*!< - 偏置限值(V or A)  */ \
}

typedef struct {                            //!< 输出列表
    uint32_t    mCnt;                       //!< - 输出次数
    double      mVal[SMU_LIST_SIZE];        //!< - 输出源值
} SOUR_LIST_T;

typedef struct {                            //!< SOUR Stat for Run-Time
    SOUR_INFO_T        *mInfo;              //!< - SOUR Info
    SMU_SHAP_E          mShap;              //!< - 输出类型{ SMU_DC | SMU_PULS }
    bool                bOutp;              //!< - 输出状态[ ON(1) | OFF(0) ]
    SMU_MODE_E          mMode;              //!< - 输出模式
    SWE_ZONE_T   *const mZone;              //!< - 区间扫描
    SOUR_LIST_T  *const mList;              //!< - 输出列表
} SOUR_STAT_T;

#define SOUR_STAT_DFT(info, zone, list) {   /*!< 默认(SOUR Stat)    */ \
      info           /* mInfo;  */          /*!< - SOUR Info        */ \
    , SMU_DC         /* mShap;  */          /*!< - 输出类型         */ \
    , false          /* bOutp;  */          /*!< - 输出状态         */ \
    , OUT_FIX        /* mMode;  */          /*!< - 输出模式         */ \
    , zone           /* mZone;  */          /*!< - 区间扫描         */ \
    , list           /* mList   */          /*!< - 输出列表         */ \
}


/*******************************************************************************
 * @}
 * @addtogroup              Sense
 * @{
 ******************************************************************************/



/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __SMU_COMMON_H__
/*****************************  END OF FILE  **********************************/

