/*******************************************************************************
 * @file    App_SPWM.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2022/4/24
 * @brief   SPWM application
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
 *      2022/4/24 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __APP_LPLX_H__
#define __APP_LPLX_H__
/******************************************************************************/
#include "Drv_LDBI016.h"
#include "App_mCal.h"
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup App_SPWM
 * @{
 * @addtogroup              Exported_Constants
 * @{
 ******************************************************************************/
#define                                  V_RANGE1                         (3)
#define                                  V_RANGE2                         (30)
#define                                  V_RANGE3                         (100)
#define                                  I_RANGE1                         (20)
#define                                  I_RANGE2                         (60)
#define                                  I_RANGE3                         (600)
#define                                  IMIN                             (0.002)
#define                                  TEMP_CH                          (8)
typedef enum _ModeTypeTag_
{
    SweTypeTBD                           = 0x00,                          /* ¶ƒ???? */
    SweLineType                          = 0x01,                          /* ??????? */
    SweLogType                           = 0x02,                          /* ??????? */
    SweCustomType                        = 0x03,                          /* ???????? */
} SweMode;

typedef struct AnlgStat_Tag
{
//  bool                        isPulse;                                  /* ????Or??? */
//  bool                        isOutPut;                                 /* ?????? */
//  uint32_t                    mPulseWidt;                               /* ??????? */
//  uint32_t                    mPulsePeriod;                             /* ???????? */
//  uint32_t                    mPulseNum;                                /* ??????? */
    uint32_t                    mSampPonit;                               /* ???????? */
//  uint32_t                    mSampDelay;                               /* ??????? */
//  uint32_t                    mOutputDelay;                             /* ?????? */
    uint32_t                    mRiseTime;                                /* ????????? */
    float                       mMaxTemp;                                 /* ??????????ßÿ? */
    float                       mMinOPD;                                  /* ??ß≥?????ßÿ? */
    float                       mReadCurr;                                /* ??????????ßÿ? */
    float                       mProtVolt;                                /* ??????? */
    float                       mMaxPower;                                /* ??????W */
//  double                      mSetCurrVal;                              /* ???????? */
//  double                      mDacVal;                                  /* DAC??? */
//  float                       mSweStart;                                /* ?????? */
//  float                       mSweStop;                                 /* ?????? */
//  float                       mSweStep;                                 /* ??ŸO?? */
    float                       mDutVal1;                                 /* DUT1? */
    float                       mDutVal2;                                 /* DUT2? */
    float                       mDutVal3;                                 /* DUT3? */
    float                       mAdcVal1;                                 /* ADC1??? */
    float                       mAdcVal2;                                 /* ADC2??? */
    float                       mAdcVal3;                                 /* ADC3??? */
    float                       mTempVal1;                                /* Temp1? */
    float                       mTempVal2;                                /* Temp2? */
    float                       mTempVal3;                                /* Temp3? */
//  SweMode                     mSweType;                                 /* ????????? */
    bool                        mOverTemp;                                /* ???°¿??? */
    bool                        mOverVolt;                                /* ??????? */
    uint8_t                     mChannel;                                 /* ?????????? */
    float                       mWaterFlow;                               /* ????? */
    Ldbi016AnlgEnumVIndexT      mVrange;                                  /* ??????? */
    Ldbi016AnlgEnumIIndexT      mIrange;                                  /* ???????? */
    Ldbi016AnlgEnumOpmIndexT    mOpdRange;                                /* ???????? */
//  float                       mRiseStep;                                /* ???????????? */
//  uint32_t                    mRiseDelayMs;                             /* ??????????? */
} AnlgStat_T;


typedef struct APP_MCU_TAG_
{
    PSS_MCU_DRVS_BRD_T    AnlgBrd;              /* ?èâ?????? */
    AnlgStat_T            AnlgStat;             /* ????????? */
    PSS_MCU_CAL_T         AnlgCal;              /* ????ßµ???? */
}APP_MCU_T;
/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/


int LDBI016_init(void);
/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __APP_LPLX_H__
/*****************************  END OF FILE  **********************************/

