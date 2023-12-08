/*******************************************************************************
 * @file    Drv_FpgaPuls.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/23
 * @brief   Driver for FPGA Puls Modul
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
 *      2023/3/23 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DRV_FPGAPULS_H__
#define __DRV_FPGAPULS_H__
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Drv_FpgaPuls
 * @{
 * @addtogroup              Exported_Constants
 * @{
 ******************************************************************************/


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Macros
 * @{
 ******************************************************************************/

#define FPGA_PULS_ADDR                  0x6000A000      // FPGA Puls 模块地址
#define FPGA_PULS_FREQ               (40 * 1000 * 1000) // 时钟频率
#define FPGA_PULS_US_TO_NUM(usTime)  ( (usTime) * (1000 / (1000*1000*1000 / FPGA_PULS_FREQ)) )
#define FPGA_PULS_NS_TO_NUM(nsTime)  ( (nsTime)         / (1000*1000*1000 / FPGA_PULS_FREQ)  )

#define FPGA_PULS_NUM_INFINITE          0xFFFFFFFF      // 输出脉冲个数为无限


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Types
 * @{
 ******************************************************************************/

typedef struct {
    volatile        uint32_t    mPulsHiCyc;         // 00 Puls 输出高电平时钟个数
    volatile        uint32_t    mPulsLoCyc;         // 04 Puls 输出低电平时钟个数
    volatile        uint32_t    mPulsOutNum;        // 08 Puls 输出个数
    volatile        uint32_t    mPulsOutStart;      // 0C Puls 输出开始
    volatile        uint32_t    mPulsOutStop;       // 10 Puls 输出停止
    volatile        uint32_t    mReserved1[7];      // 14-2C 未使用
    volatile        uint32_t    mTrigMode;          // 30 Trig 模式
    volatile        uint32_t    mTrigOutHiCyc;      // 34 Trig 输出高电平时钟个数
    volatile        uint32_t    mTrigOutLoCyc;      // 38 Trig 输出低电平时钟个数
    volatile        uint32_t    mTrigOutNum;        // 3C Trig 输出个数
    volatile        uint32_t    mPulsOutDlyCyc;     // 40 Puls 输出延时时钟个数
    volatile        uint32_t    mTrigOutDlyCyc;     // 44 Trig 输出延时时钟个数
    volatile const  uint32_t    mPulsState;         // 48 Puls 状态寄存器, (!= 0)表示正在输出
    volatile        uint32_t    mReserved2[7];      // 4C-64 未使用
    volatile        uint32_t    mDCorPULS;          // 68 直流或脉冲模式
    volatile        uint32_t    mPulsShutOut;       // 6C Puls OUT 和 SHUT
    volatile        uint32_t    mInfiniteOut;       // 70 无限输出(最后一个源值是否一直输出)
} FpgaPuls_T;

typedef enum {
    FPGA_PULS_CH_1   = 0x1,     // Puls 通道 1
//  FPGA_PULS_CH_2   = 0x2,     // Puls 通道 2
//  FPGA_PULS_CH_3   = 0x4,     // Puls 通道 3
//  FPGA_PULS_CH_4   = 0x8,     // Puls 通道 4
    FPGA_PULS_CH_ALL = 0xF      // Puls 通道 All
} FpgaPuls_MSK;

typedef enum {
    FPGA_TRIG_M_SOUR   = 0,     // 触发模式: 主机
    FPGA_TRIG_M_ACC_RT = 1,     // 触发模式: 从机, 上升沿触发
    FPGA_TRIG_M_ACC_FT = 2,     // 触发模式: 从机, 下降沿触发
    FPGA_TRIG_M_ACC_DT = 3      // 触发模式: 从机, 双边沿触发
} FpgaTrig_MODE;


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
/**
 * FPGA Puls 初始化
 *
 * @param[in]  chMsk        通道选择
 * @param[in]  EventFun     事件处理回调函数
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaPuls_Init(FpgaPuls_MSK chMsk, void (*EventFun)(uint32_t));

/**
 * 设置脉冲或直流模式
 *
 * @param[in]  chMsk    通道选择
 * @param[in]  bfPuls   (1)脉冲, (0)直流
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaPuls_ShapSet(FpgaPuls_MSK chMsk, int bfPuls);

/**
 * FPGA Puls 设置脉宽与周期时间
 *
 * @param[in]  chMsk      通道选择
 * @param[in]  widthUS    脉冲宽度时间, 以微妙(uS)为单位
 * @param[in]  periodUS   脉冲周期时间, 以微妙(uS)为单位
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaPuls_TimeSet(FpgaPuls_MSK chMsk, uint32_t widthUS, uint32_t periodUS);

/**
 * FPGA Puls 设置是否为无限输出
 *
 * @param[in]  chMsk      通道选择
 * @param[in]  bfInfinite (1)输出无限脉冲个数, (0)输出指定脉冲个数
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaPuls_OutSet(FpgaPuls_MSK chMsk, int bfInfinite);

/**
 * FPGA Puls 启动脉冲输出
 *
 * @param[in]  chMsk      通道选择
 * @param[in]  pulsNum    输出脉冲个数
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaPuls_OutStart(FpgaPuls_MSK chMsk, uint32_t pulsNum);

/**
 * FPGA Puls 停止脉冲输出
 *
 * @param[in]  chMsk    通道选择 
 * 
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaPuls_OutStop(FpgaPuls_MSK chMsk);

/**
 * FPGA Puls 设置脉冲延时
 *
 * @param[in]  chMsk     通道选择
 * @param[in]  delayNS   延时时间, 以纳秒(nS)为单位
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaPuls_DelaySet(FpgaPuls_MSK chMsk, uint32_t delayNS);


/**
 * FPGA Puls 获取是否正在输出脉冲
 *
 * @param[in]  chMsk    通道选择 
 * 
 * @retval  (!= 0) 是正在输出
 * @retval  (== 0) 非正在输出
 */
int FpgaPuls_BusyGet(FpgaPuls_MSK chMsk);

/**
 * FPGA Puls 设置 ShutOut 数量
 *
 * @param[in]  chMsk    通道选择
 * @param[in]  outNum   ShutOut 数量
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaPuls_ShutOutSet(FpgaPuls_MSK chMsk, uint32_t outNum);

/**
 * FPGA Puls 设置触发模式
 *
 * @param[in]  chMsk      通道选择
 * @param[in]  trigMode   触发模式
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaTrig_ModeSet(FpgaPuls_MSK chMsk, FpgaTrig_MODE trigMode);

/**
 * FPGA Puls 设置触发输出参数
 *
 * @param[in]  trigNum    触发输出个数
 * @param[in]  delayNS    触发输出延时, 以纳秒(nS)为单位
 * @param[in]  widthNS    触发脉冲宽度, 以纳秒(nS)为单位
 * @param[in]  periodNS   触发脉冲周期, 以纳秒(nS)为单位
 *
 * @retval  (== 0) 成功
 * @retval  (!= 0) 失败
 */
int FpgaTrig_ModeSet(FpgaPuls_MSK chMsk, FpgaTrig_MODE trigMode);
int FpgaTrig_NumSet(FpgaPuls_MSK chMsk, uint32_t num);
int FpgaTrig_OutSet(FpgaPuls_MSK chMsk, uint32_t delayNS, uint32_t hiNS, uint32_t loNS);

/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __DRV_FPGAPULS_H__
/*****************************  END OF FILE  **********************************/

