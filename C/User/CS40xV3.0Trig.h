/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS40X_V3_0_TRIG_H__
#define __CS40X_V3_0_TRIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/***********************************************************************
 * @}
 * @addtogroup              Trigger
 * @{
 **********************************************************************/
//!< 触发线编号
typedef enum TRIG_lineNum_{                  //!< 触发线编号
    LINE_NUM_TBD  = 0,                       //!< - 未定义
    LINE_NUM_0    = 1 ,                      //!< - 触发线0 
    LINE_NUM_1    = 2 ,                      //!< - 触发线1 
    LINE_NUM_2    = 3 ,                      //!< - 触发线2 
    LINE_NUM_3    = 4 ,                      //!< - 触发线3 
    LINE_NUM_4    = 5 ,                      //!< - 触发线4 
    LINE_NUM_5    = 6 ,                      //!< - 触发线5 
    LINE_NUM_6    = 7 ,                      //!< - 触发线6 
    LINE_NUM_7    = 8 ,                      //!< - 触发线7 
    LINE_NUM_8    = 9 ,                      //!< - 触发线8 
    LINE_NUM_9    = 10,                      //!< - 触发线9 
    LINE_NUM_10   = 11,                      //!< - 触发线10
    LINE_NUM_11   = 12,                      //!< - 触发线11
    LINE_NUM_12   = 13,                      //!< - 触发线12
    LINE_NUM_13   = 14,                      //!< - 触发线13
    LINE_NUM_14   = 15,                      //!< - 触发线14
    LINE_NUM_15   = 16,                      //!< - 触发线15
} TRIG_lineNum_e;

//!< 触发线模式
typedef enum TRIG_lineMode_{                //!< 触发线模式
    LINE_MODE_IDLE         = 0 ,            //!< - 空闲
    LINE_MODE_RIS_IN       = 1 ,            //!< - 上升沿输入
    LINE_MODE_FALL_IN      = 2 ,            //!< - 下降沿输入
    LINE_MODE_RIS_FALL_IN  = 3 ,            //!< - 双边沿输入
    LINE_MODE_HI_LEV_IN    = 4 ,            //!< - 高电平输入
    LINE_MODE_LO_LEV_IN    = 5 ,            //!< - 低电平输入
    LINE_MODE_RIS_OUT      = 6 ,            //!< - 上升沿输出
    LINE_MODE_FALL_OUT     = 7 ,            //!< - 下降沿输出
    LINE_MODE_RIS_FALL_OUT = 8 ,            //!< - 双边沿输出
    LINE_MODE_HI_LEV_OUT   = 9 ,            //!< - 高电平输出
    LINE_MODE_LO_LEV_OUT   = 10,            //!< - 低电平输出
} TRIG_lineMode_e;

typedef struct TRIG_info_{                  //!< 触发参数
    TRIG_lineNum_e     mLine;               //!< - 触发线编号
    TRIG_lineMode_e    mMode;               //!< - 触发线模式
    bool               bLineEn;             //!< - 触发线使能
//    bool               bIsDio;              //!< - 是否为DIO模式
    uint32_t           mNum;                //!< - 触发脉冲输出个数
    float              mWidt;               //!< - 触发脉冲宽度(S)
    float              mPeri;               //!< - 触发脉冲周期(S)
    uint32_t           mDlyIn;              //!< - 触发输入延时
    uint32_t           mDlyOut;             //!< - 触发输出延时
} TRIG_info_t;







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
extern int SMU_trigSetPara(const TRIG_info_t *anlgDev, void *txIndex, void *txData);

/**
  * @功能: 回读源参数
  * @参数: [in ] anlgDev 模拟板设备句柄
           [in ] txIndex 查询索引
           [out] rxdata 读取数据
  * @返回值: (>= 0) 成功
             (<= -1) 失败
  * @其它: None
  **/
extern int SMU_trigReadPara(const TRIG_info_t *anlgDev, void *txIndex, void *rxdata);


#ifdef __cplusplus
}
#endif
#endif /*__CS40X_V3_0_TRIG_H__ */

