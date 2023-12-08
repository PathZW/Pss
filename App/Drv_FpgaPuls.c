/*******************************************************************************
 * @file    Drv_FpgaPuls.c
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
//lint +libclass(all)
//lint -$
//lint -e537                        // Repeated include file 'xxxx'
//lint -e750                        // local macro 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Drv_FpgaPuls"
#include "emb_log.h"
#include "Drv_FpgaPuls.h"

#include <stdint.h>
#include "defines.h"

#include "board.h"              // Device's defines
#include "emb_delay.h"


/*******************************************************************************
 * @addtogroup Drv_FpgaPuls
 * @{
 * @addtogroup              Private_Constants
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/

#define FPGA_PULS   ((FpgaPuls_T *)FPGA_PULS_ADDR)      // FPGA Puls 模块


/*******************************************************************************
 * @}
 * @addtogroup              Private_Types
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup              Private_Prototypes
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * FPGA Puls 初始化
 */
int FpgaPuls_Init(FpgaPuls_MSK chMsk, void (*EventFun)(uint32_t))
{
    UNUSED_VAR(chMsk);

    GPIO_PIN_INIT     (FSMC_INT1_Port, FSMC_INT1_Pin, GPIO_M_IT_FT | GPIO_P_PU);
    GPIO_PIN_INTR_PRIO(FSMC_INT1_Port, FSMC_INT1_Pin, FSMC_INT1_IRQ_Prio      );
    GPIO_PIN_INTR_CTRL(FSMC_INT1_Port, FSMC_INT1_Pin, EventFun                );

    FPGA_PULS->mTrigMode      = FPGA_TRIG_M_SOUR;   //lint !e641
    FPGA_PULS->mTrigOutNum    = 1;
    FPGA_PULS->mTrigOutDlyCyc = 0;
    FPGA_PULS->mTrigOutHiCyc  = 40;
    FPGA_PULS->mTrigOutLoCyc  = 40;

    return( 0 );
}


/**
 * 设置脉冲或直流模式
 */
int FpgaPuls_ShapSet(FpgaPuls_MSK chMsk, int bfPuls)
{
    UNUSED_VAR(chMsk);
    FPGA_PULS->mDCorPULS  = ((bfPuls) ? 0x00 : 0x01);   // 设置脉冲或直流模式
    return( 0 );
}


/**
 * FPGA Puls 设置脉宽与周期时间
 */
int FpgaPuls_TimeSet(FpgaPuls_MSK chMsk, uint32_t widthUS, uint32_t periodUS)
{
    UNUSED_VAR(chMsk);

    uint32_t    widthCyc  = FPGA_PULS_US_TO_NUM(widthUS);
    uint32_t    periodCyc = FPGA_PULS_US_TO_NUM(periodUS);

    if (periodCyc < (widthCyc + FPGA_PULS_US_TO_NUM(2 * 20))) {
        periodCyc = (widthCyc + FPGA_PULS_US_TO_NUM(2 * 20));
    }
    FPGA_PULS->mPulsHiCyc =  widthCyc;  // FPGA 脉冲低最少 800 个时钟
    FPGA_PULS->mPulsLoCyc = ((periodCyc - widthCyc) > (800 + 10)) ? (periodCyc - widthCyc) : (800 + 10);
    return( 0 );
}


/**
 * FPGA Puls 设置是否为无限输出
 */
int FpgaPuls_OutSet(FpgaPuls_MSK chMsk, int bfInfinite)
{
    UNUSED_VAR(chMsk);  UNUSED_VAR(bfInfinite);
    FPGA_PULS->mInfiniteOut = bfInfinite ? 0x01 : 0x00;
    return( 0 );
}


/**
 * FPGA Puls 启动脉冲输出
 */
int FpgaPuls_OutStart(FpgaPuls_MSK chMsk, uint32_t PulsNum)
{
    UNUSED_VAR(chMsk);

    FPGA_PULS->mPulsOutNum   = PulsNum;                  // 设置输出脉冲个数
//  DelayUS(10);
    FPGA_PULS->mPulsOutStart = 0x01;                     // 启动脉冲输出
    return( 0 );
}


/**
 * FPGA Puls 停止脉冲输出
 */
int FpgaPuls_OutStop(FpgaPuls_MSK chMsk)
{
    UNUSED_VAR(chMsk);

//  FPGA_PULS->mPulsOutStop = 0x00;     // step1: 持续输出低电平
//  DelayUS(10);
    FPGA_PULS->mPulsOutStop = 0x02;     // step1: 清空计数器

    return( 0 );
}


/**
 * FPGA Puls 设置脉冲延时
 */
int FpgaPuls_DelaySet(FpgaPuls_MSK chMsk, uint32_t delayNS)
{
    UNUSED_VAR(chMsk);

    FPGA_PULS->mPulsOutDlyCyc = FPGA_PULS_NS_TO_NUM(delayNS);

    return( 0 );
}


/**
 * FPGA Puls 获取是否正在输出脉冲
 */
int FpgaPuls_BusyGet(FpgaPuls_MSK chMsk)
{
    UNUSED_VAR(chMsk);

    return( (FPGA_PULS->mPulsState & 0x1) ? 1 : 0 );
}


/**
 * FPGA Puls 设置 ShutOut 数量
 */
int FpgaPuls_ShutOutSet(FpgaPuls_MSK chMsk, uint32_t outNum)
{
    unsigned    i;
    uint32_t    val;
    UNUSED_VAR(chMsk);

    for(i = 0, val = 0;  i < outNum;  i++) {
        val |= 1u << i;
    }
    FPGA_PULS->mPulsShutOut = val;

    return( 0 );
}


/**
 * FPGA Puls 设置触发模式
 */
int FpgaTrig_ModeSet(FpgaPuls_MSK chMsk, FpgaTrig_MODE trigMode)
{
    UNUSED_VAR(chMsk);
    FPGA_PULS->mTrigMode = trigMode; //lint !e641
    return( 0 );
}


int FpgaTrig_NumSet(FpgaPuls_MSK chMsk, uint32_t num)
{
    UNUSED_VAR(chMsk);
    FPGA_PULS->mTrigOutNum = num;
    return( 0 );
}


int FpgaTrig_OutSet(FpgaPuls_MSK chMsk, uint32_t delayNS, uint32_t hiNS, uint32_t loNS)
{
    UNUSED_VAR(chMsk);
    FPGA_PULS->mTrigOutDlyCyc = FPGA_PULS_NS_TO_NUM(delayNS);
    FPGA_PULS->mTrigOutHiCyc  = FPGA_PULS_NS_TO_NUM(hiNS);
    FPGA_PULS->mTrigOutLoCyc  = FPGA_PULS_NS_TO_NUM(loNS);
    return( 0 );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

