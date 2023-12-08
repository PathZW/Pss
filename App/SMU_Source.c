/*******************************************************************************
 * @file    SMU_Source.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/23
 * @brief   Source subsystem
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
#include APP_CFG                    // Apps configuration
#endif
#define EMB_LOG_PRINT(...)      printf(__VA_ARGS__)
#define  LOG_TAG  "SOUR"
#include "emb_log.h"
#include "SMU_Source.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "defines.h"
#include "cmsis_os2.h"
#include "emb_delay.h"
#include "emb_rtcfg.h"
#include "shell/cli_shell.h"

#include "Drv_FpgaPuls.h"
#include "board.h"


#define SMU_SENS_RATE       1000000     // 采样率


#ifdef LDBI_SENS
#include "devDriver_ADC.h"
extern  devDriver_t    devDriver(FpgaAD4000, 1);
#define FpgaAD4000_1 (&devDriver(FpgaAD4000, 1))
extern  devDriver_t    devDriver(FpgaAD4000, 2);
#define FpgaAD4000_2 (&devDriver(FpgaAD4000, 2))
extern  devDriver_t    devDriver(FpgaAD4000, 3);
#define FpgaAD4000_3 (&devDriver(FpgaAD4000, 3))
#endif

#ifdef LDBI_SOUR
#include "devDriver_ADC.h"
extern  devDriver_t    devDriver(FpgaAD5683, 1);
#define FpgaAD5683_1 (&devDriver(FpgaAD5683, 1))
extern  devDriver_t    devDriver(FpgaAD5683, 2);
#define FpgaAD5683_2 (&devDriver(FpgaAD5683, 2))
extern  devDriver_t    devDriver(FpgaAD5683, 3);
#define FpgaAD5683_3 (&devDriver(FpgaAD5683, 3))
extern  devDriver_t    devDriver(FpgaAD5683, 4);
#define FpgaAD5683_4 (&devDriver(FpgaAD5683, 4))

#endif


#include "App_LDBI016.h"
#ifdef LDBI_SOUR
uint32_t                  GetBrdNumWithVal(Ldbi016AnlgEnumIIndexT range, double val);
PSS_MCU_RANGE_CAL_INDEX_T GetIndexWithIVal(double val, Ldbi016AnlgEnumIIndexT range);
#endif

float                      _SOUR_LevStep = 10E-3f;  // 输出改变步进 .mRiseStep
extern APP_MCU_T             sApp;
#define _Broad              (sApp.AnlgBrd)
#define _SourStat           (sApp.AnlgStat)
#define _SourStat_Irange    (sApp.AnlgStat.mIrange)

extern osMutexId_t          _LDBI16_Mutex;
#define _SOUR_Mutex         _LDBI16_Mutex


/*******************************************************************************
 * @addtogroup SMU_Source
 * @{
 * @addtogroup              Private_Constants
 * @{
 ******************************************************************************/

#define SOUR_FLAG_MSK           0x07u
#define SOUR_FLAG_OUTP_ON       0x01u
#define SOUR_FLAG_OUTP_OFF      0x02u
#define SOUR_FLAG_OUTP_END      0x04u


/*******************************************************************************
 * @}
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/



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

// 区间扫描
static       SWE_ZONE_T    _SOUR_Zone    = SWE_ZONE_DFT;
static const SWE_ZONE_T    _SOUR_ZoneDFT = SWE_ZONE_DFT;

// SOUR Info
static       SOUR_INFO_T   _SOUR_Info    = SOUR_INFO_DFT(NULL, NULL);
static const SOUR_INFO_T   _SOUR_InfoDFT = SOUR_INFO_DFT(NULL, NULL);

// SOUR List
static       SOUR_LIST_T   _SOUR_List    = { 0 };

// SOUR Stat for Run-Time
             SOUR_STAT_T   _SOUR_Stat    = SOUR_STAT_DFT(&_SOUR_Info, &_SOUR_Zone, &_SOUR_List);
static const SOUR_STAT_T   _SOUR_StatDFT = SOUR_STAT_DFT(&_SOUR_Info, &_SOUR_Zone, &_SOUR_List);

// 线程
static const osThreadAttr_t _SOUR_ThreadAttr = {
    "SOUR_Thread", osThreadDetached,  NULL, 0, NULL, 1024, osPriorityNormal1, 0, 0
};
static osThreadId_t        _SOUR_Thread = NULL;

#if 0
// 重入保护互斥量
static const osMutexAttr_t _SOUR_MutexAttr = {
    "App_LDBI16", osMutexRecursive | osMutexPrioInherit | osMutexRobust, NULL, 0
};
static osMutexId_t         _SOUR_Mutex;
#endif


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
 * 直流恒定输出
 */
static void SOUR_OutpDcFix(void)
{   //lint --e{534}
    double    start =  0;
    double    stop  = _SOUR_Stat.mInfo->mLevVal;
    double    step  =  fabs(_SOUR_LevStep);  //lint !e747
    uint32_t  poin  =  0;

    if (start < stop)
    {
        for(start += step; start < stop; start += step)
        {
            _SOUR_Stat.mList->mVal[poin++] = start;
        }
        {   _SOUR_Stat.mList->mVal[poin++] = stop;
            _SOUR_Stat.mList->mCnt         = poin;
        }
    }
    else
    {
        for(start -= step; start > stop; start -= step)
        {
            _SOUR_Stat.mList->mVal[poin++] = start;
        }
        {   _SOUR_Stat.mList->mVal[poin++] = stop;
            _SOUR_Stat.mList->mCnt         = poin;
        }
    }

#ifdef LDBI_SOUR    // 脉宽 = 滤波平均次数 / 采样率 + 直流稳定时间
    uint32_t  time = ((float)_SourStat.mSampPonit / SMU_SENS_RATE + _SOUR_Stat.mInfo->mDlyDC) * 1.15E6f + 0.5f; //lint !e524 !e732
    FpgaPuls_TimeSet(FPGA_PULS_CH_1, time, time);
    FpgaPuls_OutSet (FPGA_PULS_CH_1, true);
#endif
#ifdef LDBI_SENS
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
#endif
}

/**
 * 直流线性扫描
 */
static void SOUR_OutpDcLin(void)
{   //lint --e{534}
    double    start = _SOUR_Stat.mZone->mStart;   // 扫描起点
    double    stop  = _SOUR_Stat.mZone->mStop;    // 扫描终点
    uint32_t  poin  = _SOUR_Stat.mZone->mPoin;    // 扫描点数
    double    step  = (poin > 1) ? ((stop - start) / (poin - 1)) : (poin = 1, 0);

    for(uint32_t i = 0;  i < poin;  start += step, i++)
    {
        _SOUR_Stat.mList->mVal[i] = start;
    }
    _SOUR_Stat.mList->mCnt = poin;

#ifdef LDBI_SOUR    // 脉宽 = 滤波平均次数 / 采样率 + 直流稳定时间
    uint32_t  time = ((float)_SourStat.mSampPonit / SMU_SENS_RATE + _SOUR_Stat.mInfo->mDlyDC) * 1.15E6f + 0.5f; //lint !e524 !e732
    FpgaPuls_TimeSet(FPGA_PULS_CH_1, time, time);
    FpgaPuls_OutSet (FPGA_PULS_CH_1, false);
#endif
#ifdef LDBI_SENS
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
#endif
}

/**
 * 脉冲恒定输出
 */
static void SOUR_OutpPulsFix(void)
{   //lint --e{534}
    _SOUR_Stat.mList->mVal[0] = _SOUR_Stat.mInfo->mLevVal;  // 输出值
    _SOUR_Stat.mList->mCnt    = 1;

#ifdef LDBI_SOUR
    FpgaPuls_TimeSet(FPGA_PULS_CH_1, _SOUR_Stat.mInfo->mPulsWidt * 1E6f + 0.5f
                                   , _SOUR_Stat.mInfo->mPulsPeri * 1E6f + 0.5f); //lint !e524 !e732 !e747
    FpgaPuls_OutSet (FPGA_PULS_CH_1, true);
#endif
#ifdef LDBI_SENS    // 滤波平均次数 = (脉宽 - 上升沿稳定时间) * 采样率
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
#endif
}

/**
 * 脉冲线性扫描
 */
static void SOUR_OutpPulsLin(void)
{   //lint --e{534}
    double    start = _SOUR_Stat.mZone->mStart;   // 扫描起点
    double    stop  = _SOUR_Stat.mZone->mStop;    // 扫描终点
    uint32_t  poin  = _SOUR_Stat.mZone->mPoin;    // 扫描点数
    uint32_t  num   = _SOUR_Stat.mInfo->mPulsNum; // 脉冲序列数
    double    step  = (poin > 1) ? ((stop - start) / (poin - 1)) : (poin = 1, 0);

    for(uint32_t i = 0;  i < poin;  start += step, i++)
    {
        for(uint32_t j = 0;  j < num;  j++)
        {
            _SOUR_Stat.mList->mVal[num * i  + j] = start;
        }
    }
    _SOUR_Stat.mList->mCnt = poin * num;

#ifdef LDBI_SOUR
    FpgaPuls_TimeSet(FPGA_PULS_CH_1, _SOUR_Stat.mInfo->mPulsWidt * 1E6f + 0.5f
                                   , _SOUR_Stat.mInfo->mPulsPeri * 1E6f + 0.5f); //lint !e524 !e732 !e747
    FpgaPuls_OutSet (FPGA_PULS_CH_1, false);
#endif
#ifdef LDBI_SENS    // 滤波平均次数 = (脉宽 - 上升沿稳定时间) * 采样率
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_FILT_CFG, 0x01, DEV_ADC_FILT_AVG, _SourStat.mSampPonit);
#endif
}

/**
 * 打开输出
 */
static void SOUR_OutpON(SMU_CHNL_T chnl)
{   //lint --e{534, 641, 747}
    UNUSED_VAR(chnl);

#ifdef LDBI_SOUR
    if (_SOUR_Stat.bOutp) {
        return;
    }
#endif
#ifdef LDBI_SENS
    if (LdbiOutpStaGet()) {
        return;
    }
#endif
    osMutexAcquire(_SOUR_Mutex, osWaitForever);

#ifdef LDBI_SOUR
    FpgaPuls_OutStop(FPGA_PULS_CH_1);
    //PssMcuDrvsAd5683Reset(&_Broad.sAd5683);     // 复位 DAC, 包括 FIFO
    LdbiPowerDisCharge(false);                  // 禁止 DisCharge
    osMutexRelease(_SOUR_Mutex);
    DelayMS(10);
    osMutexAcquire(_SOUR_Mutex, osWaitForever);

    LdbiPowerCharge(true);                      // 允许 Charge
    LdbiUpdateICtrl(_SourStat_Irange, true);    // 更新电流回读量程
#endif

    if((_SOUR_Stat.mMode == SWE_LIN) && (_SOUR_Stat.mShap == SMU_DC)) {
        SOUR_OutpDcLin();   // 直流线性扫描
    }
    else
    if((_SOUR_Stat.mMode == SWE_LIN) && (_SOUR_Stat.mShap == SMU_PULS)) {
        SOUR_OutpPulsLin(); // 脉冲线性扫描
    }
    else
    if((_SOUR_Stat.mMode == OUT_FIX) && (_SOUR_Stat.mShap == SMU_PULS)) {
        SOUR_OutpPulsFix(); // 脉冲恒定输出
    }
    else {
  //if((_SOUR_Stat.mMode == OUT_FIX) && (_SOUR_Stat.mShap == SMU_DC)) {
        SOUR_OutpDcFix();   // 直流恒定输出
    }

#ifdef LDBI_SOUR
    for(uint32_t i = 0;  i < _SOUR_Stat.mList->mCnt;  i++)
    {
        double   val    = _SOUR_Stat.mList->mVal[i];
        uint32_t brdNum =  GetBrdNumWithVal(_SourStat_Irange, val);  // 分板数量
        val = Load2CalVal(&sApp.AnlgCal, val / brdNum, GetIndexWithIVal(val, _SourStat_Irange));
        FpgaPuls_ShutOutSet(FPGA_PULS_CH_1, brdNum);                 // 分板控制
        //PssMcuDrvsChoseAd5683SetVal(&_Broad.sAd5683, PSS_MCU_DRVS_AD5683_2_MASK, true, val);
        FpgaAD5683_3->Ctrl(DEV_CMD_DAC_SEND_REAL,3);
        FpgaAD5683_3->Send(&val, 1,PSS_MCU_DRVS_AD5683_2_MASK);
    }
#endif

    osMutexRelease(_SOUR_Mutex);
    DelayMS(200);
    osMutexAcquire(_SOUR_Mutex, osWaitForever);

#ifdef LDBI_SENS
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_MODE_CFG | DEV_CMD_FLUSH_RECV, 0x01, DEV_ADC_MODE_TRIG);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_MODE_CFG | DEV_CMD_FLUSH_RECV, 0x01, DEV_ADC_MODE_TRIG);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_MODE_CFG | DEV_CMD_FLUSH_RECV, 0x01, DEV_ADC_MODE_TRIG);
#endif
#ifdef LDBI_SOUR
    FpgaTrig_NumSet  (FPGA_PULS_CH_1, _SOUR_Stat.mList->mCnt);
    FpgaPuls_OutStart(FPGA_PULS_CH_1, _SOUR_Stat.mList->mCnt);
#endif

    _SOUR_Stat.bOutp = true;
    osMutexRelease(_SOUR_Mutex);
}


/**
 * 结束输出
 */
static void SOUR_OutpEND(SMU_CHNL_T chnl)
{   //lint --e{534, 747}
	double   val =0;
    UNUSED_VAR(chnl);

    osMutexAcquire(_SOUR_Mutex, osWaitForever);

#ifdef LDBI_SOUR
    FpgaPuls_OutStop(FPGA_PULS_CH_1);
    osMutexRelease(_SOUR_Mutex);
    DelayMS(2);
    osMutexAcquire(_SOUR_Mutex, osWaitForever);

    //PssMcuDrvsChoseAd5683SetVal(&_Broad.sAd5683, PSS_MCU_DRVS_AD5683_2_MASK, false, 0);
    FpgaAD5683_3->Ctrl(DEV_CMD_DAC_SEND_DATA,3);
    FpgaAD5683_3->Send(&val, 1,PSS_MCU_DRVS_AD5683_2_MASK);
    LdbiUpdateICtrl(_SourStat_Irange, false);
    LdbiPowerCharge(false);
    osMutexRelease(_SOUR_Mutex);
    DelayMS(10);
    osMutexAcquire(_SOUR_Mutex, osWaitForever);

    LdbiPowerDisCharge(true);
#endif

    _SOUR_Stat.bOutp = false;
    osMutexRelease(_SOUR_Mutex);
}


/**
 * 关闭输出
 */
static void SOUR_OutpOFF(SMU_CHNL_T chnl)
{   //lint --e{534, 641}
    UNUSED_VAR(chnl);

#ifdef LDBI_SOUR
    if (!_SOUR_Stat.bOutp) {
        return;
    }
#endif
#ifdef LDBI_SENS
    if (!LdbiOutpStaGet()) {
        return;
    }
#endif
    osMutexAcquire(_SOUR_Mutex, osWaitForever);

    // 非直流测量模式 ---> 直接结束输出
    if((_SOUR_Stat.mMode != OUT_FIX) || (_SOUR_Stat.mShap != SMU_DC)) {
        osMutexRelease(_SOUR_Mutex);
        SOUR_OutpEND(chnl);
        return;
    }

    // 是直流测量模式 ---> 缓降后结束输出
    double    start = _SOUR_Stat.mInfo->mLevVal;
    double    stop  =  0;
    double    step  =  fabs(_SOUR_LevStep);  //lint !e747
    uint32_t  poin  =  0;

    if (start < stop)
    {
        for(start += step; start < stop; start += step)
        {
            _SOUR_Stat.mList->mVal[poin++] = start;
        }
        {   _SOUR_Stat.mList->mVal[poin++] = stop;
            _SOUR_Stat.mList->mCnt         = poin;
        }
    }
    else
    {
        for(start -= step; start > stop; start -= step)
        {
            _SOUR_Stat.mList->mVal[poin++] = start;
        }
        {   _SOUR_Stat.mList->mVal[poin++] = stop;
            _SOUR_Stat.mList->mCnt         = poin;
        }
    }

#ifdef LDBI_SOUR
    for(uint32_t i = 0;  i < _SOUR_Stat.mList->mCnt;  i++)
    {
        double   val    = _SOUR_Stat.mList->mVal[i];
        uint32_t brdNum =  GetBrdNumWithVal(_SourStat_Irange, val);  // 分板数量
        val = Load2CalVal(&sApp.AnlgCal, val / brdNum, GetIndexWithIVal(val, _SourStat_Irange));
        FpgaPuls_ShutOutSet(FPGA_PULS_CH_1, brdNum);                 // 分板控制
        //PssMcuDrvsChoseAd5683SetVal(&_Broad.sAd5683, PSS_MCU_DRVS_AD5683_2_MASK, true, val); //lint !e747
        FpgaAD5683_3->Ctrl(DEV_CMD_DAC_SEND_REAL,3);
    	FpgaAD5683_3->Send(&val, 1,PSS_MCU_DRVS_AD5683_2_MASK);
    }
    FpgaPuls_OutSet  (FPGA_PULS_CH_1, false);
    FpgaTrig_NumSet  (FPGA_PULS_CH_1, _SOUR_Stat.mList->mCnt);
    FpgaPuls_OutStart(FPGA_PULS_CH_1, _SOUR_Stat.mList->mCnt);
#endif

    osMutexRelease(_SOUR_Mutex);
}

/**
 * 中断响应(脉冲输出完成)
 */
void SOUR_IntrCallback(uint32_t LineStatus)
{  //lint --e{534}
    UNUSED_VAR(LineStatus);

    if (_SOUR_Thread != NULL) {
        osThreadFlagsSet(_SOUR_Thread, SOUR_FLAG_OUTP_END);
    }
}

/**
 * SOUR 线程
 */
static void SOUR_Thread(void *arg)
{   //lint --e{534, 713, 737}
    UNUSED_VAR(arg);

    while (1)   //lint !e716
    {
        int32_t  flags = osThreadFlagsWait(SOUR_FLAG_MSK, osFlagsWaitAny, osWaitForever);

        if (flags <= 0) {
            continue;
        }
        if (flags & SOUR_FLAG_OUTP_ON) {    // 开启输出
            SOUR_OutpON(SMU_CHNL(0));
        }
        if (flags & SOUR_FLAG_OUTP_OFF) {   // 关闭输出
            SOUR_OutpOFF(SMU_CHNL(0));
        }
        if (flags & SOUR_FLAG_OUTP_END) {   // 输出结束
            SOUR_OutpEND(SMU_CHNL(0));
        }
    }
}


/*----------------------------------------------------------------------------*/
/**
 * 源输出控制 命令处理函数. :OUTP {ON | OFF}
 */
static int Cmd_OutpSet(int cli, const char* args)
{   //lint --e{534}
    char    fmt[12] = {0};

    if (sscanf(args, "%*s %11s", fmt) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }

    if(cli_strtrimcmp(fmt,"ON") == 0)
    {
        if (_SOUR_Thread != NULL) {
            osThreadFlagsSet(_SOUR_Thread, SOUR_FLAG_OUTP_ON);
        }
    }
    else if(cli_strtrimcmp(fmt,"OFF") == 0)
    {
        if (_SOUR_Thread != NULL) {
            osThreadFlagsSet(_SOUR_Thread, SOUR_FLAG_OUTP_OFF);
        }
    }
    return( 0 );
}

/**
 * 源输出查询 命令处理函数. :OUTP?
 */
static int Cmd_OutpGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, _SOUR_Stat.bOutp ? "ON"ENDL : "OFF"ENDL);
#endif
    return( 0 );
}

/**
 * 设置源输出值(A) 命令处理函数. :SOUR:CURR:LEV <val>
 */
static int Cmd_CurrLevSet(int cli, const char* args)
{   //lint --e{534}
    float   val;

#ifdef LDBI_SOUR
    if (_SOUR_Stat.bOutp) {
        return( 0 );
    }
#endif
#ifdef LDBI_SENS
    if (LdbiOutpStaGet()) {
        return( 0 );
    }
#endif

    if (sscanf(args, "%*s %f", &val) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);
    _SOUR_Stat.mInfo->mLevVal = val;
    osMutexRelease(_SOUR_Mutex);

    return( 0 );
}

/**
 * 获取源输出值(A) 命令处理函数. :SOUR:CURR:LEV?
 */
static int Cmd_CurrLevGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, "%G"ENDL, _SOUR_Stat.mInfo->mLevVal);
#endif
    return( 0 );
}

/**
* 设置电流量程(A) 命令处理函数. :SOUR:CURR:RANG <curr>
 */
static int Cmd_CurrRangSet(int cli, const char* args)
{   //lint --e{534}
    float   val;

#ifdef LDBI_SOUR
    if (_SOUR_Stat.bOutp) {
        return( 0 );
    }
#endif
#ifdef LDBI_SENS
    if (LdbiOutpStaGet()) {
        return( 0 );
    }
#endif

    if (sscanf(args, "%*s %f", &val) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);

    if (val <= I_RANGE1) {
        _SourStat_Irange = Ldbi016AnlgEnumIIndex20A;
    } else
    if (val <= I_RANGE2) {
        _SourStat_Irange = Ldbi016AnlgEnumIIndex60A;
    } else
    if (val <= I_RANGE3) {
        _SourStat_Irange = Ldbi016AnlgEnumIIndex600A;
    }
#ifdef LDBI_SOUR
    LdbiUpdateCurrRange(_SourStat_Irange);
#endif
#ifdef LDBI_SENS
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_CALB_CFG, 0x01, 1ul, 0ul, 0ul);
#endif

    osMutexRelease(_SOUR_Mutex);
    return( 0 );
}

/**
 * 获取输出电流量程(A) 命令处理函数. :SOUR:CURR:RANG?
 */
static int Cmd_CurrRangGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli,  (_SourStat_Irange == Ldbi016AnlgEnumIIndex20A) ? "20" ENDL
                    : (_SourStat_Irange == Ldbi016AnlgEnumIIndex60A) ? "60" ENDL
                    :                                                  "600"ENDL
               );
#endif
    return( 0 );
}

/**
 * 获取源输出DAC原始值 命令处理函数. PSS:ANLG:CURR:DAC?
 */
static int Cmd_OrigDACget(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    double      val    = _SOUR_Stat.mInfo->mLevVal;
    uint32_t                                                \
    brdNum = GetBrdNumWithVal(_SourStat_Irange, val);                   // use brd unm
    PSS_MCU_RANGE_CAL_INDEX_T                               \
    index = GetIndexWithIVal(val, _SourStat_Irange);                    // 获取系数标号
    val = Load2CalVal(&sApp.AnlgCal, val / brdNum, index);//lint !e641  // 获取校准系数后的值
    cli_fprintf(cli, "%+-.6E"ENDL, val);                                // 單位：V
#endif
    return( 0 );
}

/**
 * 设置源类型 命令处理函数. :SOUR:FUNC:SHAP {DC | PULS}
 */
static int Cmd_FuncShapSet(int cli, const char* args)
{   //lint --e{534}
    char    fmt[12] = {0};

#ifdef LDBI_SOUR
    if (_SOUR_Stat.bOutp) {
        return( 0 );
    }
#endif
#ifdef LDBI_SENS
    if (LdbiOutpStaGet()) {
        return( 0 );
    }
#endif

    if (sscanf(args, "%*s %11s", fmt) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);

    if(cli_strtrimcmp(fmt,"DC") == 0)
    {
#ifdef LDBI_SOUR
        FpgaPuls_ShapSet(FPGA_PULS_CH_1, false);
#endif
        _SOUR_Stat.mShap = SMU_DC;
    }
    else
    if(cli_strtrimcmp(fmt,"PULS") == 0)
    {
#ifdef LDBI_SOUR
        FpgaPuls_ShapSet(FPGA_PULS_CH_1, true);
#endif
        _SOUR_Stat.mShap = SMU_PULS;
    }

    osMutexRelease(_SOUR_Mutex);
    return( 0 );
}


/**
 * 获取源类型 命令处理函数. :SOUR:FUNC:SHAP?
 */
static int Cmd_FuncShapGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, ((_SOUR_Stat.mShap == SMU_PULS) ? "PULS"ENDL : "DC"ENDL));
#endif
    return( 0 );
}

/**
 * 设置扫描类型 命令处理函数. :SOUR:<func>:MODE {FIX | SWE | LIST}
 */
static int Cmd_funcModeSet(int cli, const char* args)
{   //lint --e{534}
    char    fmt[12] = {0};

#ifdef LDBI_SOUR
    if (_SOUR_Stat.bOutp) {
        return( 0 );
    }
#endif
#ifdef LDBI_SENS
    if (LdbiOutpStaGet()) {
        return( 0 );
    }
#endif

    if (sscanf(args, "%*s %11s", fmt) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);

    if(cli_strtrimcmp(fmt,"FIX") == 0) {
        _SOUR_Stat.mMode = OUT_FIX;
    }
    else
    if(cli_strtrimcmp(fmt,"SWE") == 0) {
        _SOUR_Stat.mMode = SWE_LIN;
    }
//  else
//  if(cli_strtrimcmp(fmt,"LOG") == 0) {
//      _SOUR_Stat.mMode = SWE_LOG;
//  }
//  else
//  if(cli_strtrimcmp(fmt,"LIST") == 0) {
//      _SOUR_Stat.mMode = SWE_LIST;
//  }

    osMutexRelease(_SOUR_Mutex);
    return( 0 );
}

/**
 * 获取扫描类型 命令处理函数. :SOUR:<func>:MODE?
 */
static int Cmd_funcModeGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, (   (_SOUR_Stat.mMode == SWE_LIST) ? "LIST"ENDL
                       : (_SOUR_Stat.mMode == SWE_LIN ) ? "SWE" ENDL
                       : (_SOUR_Stat.mMode == SWE_LOG ) ? "LOG" ENDL
                       :                                  "FIX" ENDL
                     )
               );
#endif
    return( 0 );
}

/**
 * 设置扫描起点(A) 命令处理函数. :SOUR:CURR:STAR <start>
 */
static int Cmd_CurrStarSet(int cli, const char* args)
{   //lint --e{534}
    float   val;

    if (sscanf(args, "%*s %f", &val) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);
    _SOUR_Stat.mZone->mStart = val;
    osMutexRelease(_SOUR_Mutex);

    return( 0 );
}

/**
 * 获取扫描起点(A) 命令处理函数. :SOUR:CURR:STAR?
 */
static int Cmd_CurrStarGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, "%G"ENDL, _SOUR_Stat.mZone->mStart);
#endif
    return( 0 );
}

/**
 * 设置扫描终点(A) 命令处理函数. :SOUR:CURR:STOP <stop>
 */
static int Cmd_CurrStopSet(int cli, const char* args)
{   //lint --e{534}
    float   val;

    if (sscanf(args, "%*s %f", &val) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);
    _SOUR_Stat.mZone->mStop = val;
    osMutexRelease(_SOUR_Mutex);

    return( 0 );
}

/**
 * 获取扫描终点(A) 命令处理函数. :SOUR:CURR:STOP?
 */
static int Cmd_CurrStopGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, "%G"ENDL, _SOUR_Stat.mZone->mStop);
#endif
    return( 0 );
}

/**
 * 设置扫描点数 命令处理函数. :SOUR:SWE:POIN <points>
 */
static int Cmd_SwePoinSet(int cli, const char* args)
{   //lint --e{534}
    uint32_t    val;

    if (sscanf(args, "%*s %u", &val) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    if (val > (SMU_LIST_SIZE / _SOUR_Stat.mInfo->mPulsNum)) {
        return( 1 );    // 最大扫描点数
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);
    _SOUR_Stat.mZone->mPoin = val;
    osMutexRelease(_SOUR_Mutex);

    return( 0 );
}

/**
 * 获取扫描点数 命令处理函数. :SOUR:SWE:POIN?
 */
static int Cmd_SwePoinGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, "%u"ENDL, _SOUR_Stat.mZone->mPoin);
#endif
    return( 0 );
}

/**
 * 设置脉冲宽度(S) 命令处理函数. :SOUR:PULS:WIDT <val>
 */
static int Cmd_PulsWidtSet(int cli, const char* args)
{   //lint --e{534}
    float   val;

    if (sscanf(args, "%*s %f", &val) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    if(!(FLT_LEQU(100E-6f, val) && FLT_LESS(val, _SOUR_Stat.mInfo->mPulsPeri))) {
        return( 1 );    // 最小(100uS), 最大(周期 - 100uS)
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);
    _SOUR_Stat.mInfo->mPulsWidt = val;
    osMutexRelease(_SOUR_Mutex);

    return( 0 );
}

/**
 * 获取脉冲宽度(S) 命令处理函数. :SOUR:PULS:WIDT?
 */
static int Cmd_PulsWidtGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, "%G"ENDL, _SOUR_Stat.mInfo->mPulsWidt);
#endif
    return( 0 );
}

/**
 * 设置脉冲周期(S) 命令处理函数. :SOUR:PULS:PERI <val>
 */
static int Cmd_PulsPeriSet(int cli, const char* args)
{   //lint --e{534}
    float   val;

    if (sscanf(args, "%*s %f", &val) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    if(!(FLT_LEQU(100E-6f, val) && FLT_LEQU(val, 10.0f))) {
        return( 1 );    // 10KHz ~ 0.1Hz
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);
//  if (FLT_MORE(_SOUR_Stat.mInfo->mPulsWidt, val - 100E-6f)) {
//      _SOUR_Stat.mInfo->mPulsWidt = val / 2.0f;   // 修正脉宽
//  }
    _SOUR_Stat.mInfo->mPulsPeri = val;
    osMutexRelease(_SOUR_Mutex);

    return( 0 );
}

/**
 * 获取脉冲周期(S) 命令处理函数. :SOUR:PULS:PERI?
 */
static int Cmd_PulsPeriGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, "%G"ENDL, _SOUR_Stat.mInfo->mPulsPeri);
#endif
    return( 0 );
}

/**
 * 设置脉冲个数 命令处理函数. :SOUR:PULS:COUN <time>
 */
static int Cmd_PulsCounSet(int cli, const char* args)
{   //lint --e{534}
    uint32_t    num;

    if (sscanf(args, "%*s %u", &num) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);
    _SOUR_Stat.mInfo->mPulsNum = num;
    osMutexRelease(_SOUR_Mutex);

    return( 0 );
}

/**
 * 获取脉冲个数 命令处理函数. :SOUR:PULS:COUN?
 */
static int Cmd_PulsCounGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, "%d"ENDL, _SOUR_Stat.mInfo->mPulsNum);
#endif
    return( 0 );
}

/**
 * 设置采样延时(S) 命令处理函数. :SOUR:DEL <val>
 */
static int Cmd_SourDlySet(int cli, const char* args)
{   //lint --e{534}
    float   val;

    if (sscanf(args, "%*s %f", &val) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    if (FLT_MORE(val, 1.0f)) {
        return( 1 );    // 采样延时不能大于1秒
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);
    _SOUR_Stat.mInfo->mDlyDC = val;

#ifdef LDBI_SENS
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_DELAY_CFG, 0x01, (uint32_t)(val * 1E9f + 0.5f));
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_DELAY_CFG, 0x01, (uint32_t)(val * 1E9f + 0.5f));
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_DELAY_CFG, 0x01, (uint32_t)(val * 1E9f + 0.5f));
#endif

    osMutexRelease(_SOUR_Mutex);
    return( 0 );
}

/**
 * 获取采样延时(S) 命令处理函数. :SOUR:DEL?
 */
static int Cmd_SourDlyGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, "%G"ENDL, _SOUR_Stat.mInfo->mDlyDC);
#endif
    return( 0 );
}

/**
 * 设置直流恒定输出时源改变步进(V or A) 命令处理函数. :SOUR:<func>:LEV:STEP <val>
 */
static int Cmd_funcLevStepSet(int cli, const char* args)
{   //lint --e{534}
    float   step;

    if (sscanf(args, "%*s %f", &step) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    _SOUR_LevStep = step;
    return( 0 );
}

/**
 * 获取直流恒定输出时源改变步进(V or A) 命令处理函数. :SOUR:<func>:LEV:STEP?
 */
static int Cmd_funcLevStepGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SOUR
    cli_fprintf(cli, "%G"ENDL, _SOUR_LevStep);
#endif
    return( 0 );
}

#if 0
static int Cmd_SetCurrRiseDelay(int cli, const char* args)
{
    uint32_t    delay;

    if (sscanf(args, "%*s %u", &delay) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    _SourStat.mRiseDelayMs = delay;
    return( 0 );
}
#endif

/**
 * 设置采样滤波平均次数 命令处理函数. :SENS:<func>:AVER:COUN <n>
 */
static int Cmd_funcAverCounSet(int cli, const char* args)
{   //lint --e{534}
    uint32_t    num;

    if (sscanf(args, "%*s %u", &num) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    osMutexAcquire(_SOUR_Mutex, osWaitForever);
    _SourStat.mSampPonit = num;
    osMutexRelease(_SOUR_Mutex);

    return( 0 );
}

/**
 * 设置采样滤波平均次数 命令处理函数. :SENS:<func>:AVER:COUN?
 */
static int Cmd_funcAverCounGet(int cli, const char* args)
{   //lint --e{534}
    UNUSED_VAR(cli);  UNUSED_VAR(args);

#ifdef LDBI_SENS
    cli_fprintf(cli, "%u"ENDL, _SourStat.mSampPonit);
#endif
    return( 0 );
}

/**
 * 复位
 */
void SOUR_Reset(SMU_CHNL_T chnl)
{   //lint --e{534}
    UNUSED_VAR(chnl);

    memcpy(&_SOUR_Zone, &_SOUR_ZoneDFT, sizeof(_SOUR_Zone));
    memcpy(&_SOUR_Info, &_SOUR_InfoDFT, sizeof(_SOUR_Info));
    memset(&_SOUR_List,              0, sizeof(_SOUR_List));
    memcpy(&_SOUR_Stat, &_SOUR_StatDFT, sizeof(_SOUR_Stat));

#ifdef LDBI_SOUR
    SOUR_OutpEND(SMU_CHNL_ALL);
    FpgaPuls_ShapSet(FPGA_PULS_CH_1, (_SOUR_Stat.mShap == SMU_PULS) ? true : false);
#endif

#ifdef LDBI_SENS
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_DELAY_CFG, 0x01, (uint32_t)(_SOUR_Stat.mInfo->mDlyDC * 1E9f + 0.5f));
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_DELAY_CFG, 0x01, (uint32_t)(_SOUR_Stat.mInfo->mDlyDC * 1E9f + 0.5f));
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_DELAY_CFG, 0x01, (uint32_t)(_SOUR_Stat.mInfo->mDlyDC * 1E9f + 0.5f));
#endif
}

/**
 * 初始化
 */
int SOUR_Init(void)
{   //lint --e{534}
    SOUR_Reset(SMU_CHNL_ALL);

    if((_SOUR_Thread = osThreadNew(SOUR_Thread, NULL, &_SOUR_ThreadAttr)) == NULL) {
        LOG_ASSERT("osThreadNew(LdbiThread, NULL, &_LdbiSour_ThreadAttr)");
    }
#ifdef LDBI_SOUR
    FpgaPuls_Init(FPGA_PULS_CH_ALL, SOUR_IntrCallback); // FPGA Puls 初始化
#endif

    cli_cmd_reg(":OUTP"               , Cmd_OutpSet     , "Set Output stat"   ", :OUTP {ON | OFF}"                   );
    cli_cmd_reg(":OUTP?"              , Cmd_OutpGet     , "Get Output stat"   ", :OUTP?"                             );
    cli_cmd_reg(":SOUR:CURR:LEV"      , Cmd_CurrLevSet  , "Set Source Level"  ", :SOUR:CURR:LEV <curr>"              );
    cli_cmd_reg(":SOUR:CURR:LEV?"     , Cmd_CurrLevGet  , "Get Source Level"  ", :SOUR:CURR:LEV?"                    );
//  cli_cmd_reg(":SOUR:VOLT:LEV"      , Cmd_VoltLevSet  , "Set Source Level"  ", :SOUR:VOLT:LEV <curr>"              );
//  cli_cmd_reg(":SOUR:VOLT:LEV?"     , Cmd_VoltLevGet  , "Get Source Level"  ", :SOUR:VOLT:LEV?"                    );
    cli_cmd_reg(":SOUR:CURR:RANG"     , Cmd_CurrRangSet , "Set Source Range"  ", :SOUR:CURR:RANG <curr>"             );
    cli_cmd_reg(":SOUR:CURR:RANG?"    , Cmd_CurrRangGet , "Get Source Range"  ", :SOUR:CURR:RANG?"                   );
//  cli_cmd_reg(":SOUR:VOLT:RANG"     , Cmd_VoltRangSet , "Set Source Range"  ", :SOUR:VOLT:RANG <curr>"             );
//  cli_cmd_reg(":SOUR:VOLT:RANG?"    , Cmd_VoltRangGet , "Get Source Range"  ", :SOUR:VOLT:RANG?"                   );
    cli_cmd_reg(":PSS:ANLG:CURR:DAC?" , Cmd_OrigDACget  , "Get DAC Origin"    ", :PSS:ANLG:CURR:DAC?"                );

    cli_cmd_reg(":SOUR:FUNC:SHAP"     , Cmd_FuncShapSet , "Set Output Mode"   ", :SOUR:FUNC:SHAP {PULS|DC}"          );
    cli_cmd_reg(":SOUR:FUNC:SHAP?"    , Cmd_FuncShapGet , "Get Output Mode"   ", :SOUR:FUNC:SHAP?"                   );
    cli_cmd_reg(":SOUR:CURR:MODE"     , Cmd_funcModeSet , "Set Source Mode"   ", :SOUR:CURR:MODE {FIX | SWE | LIST}" );
    cli_cmd_reg(":SOUR:CURR:MODE?"    , Cmd_funcModeGet , "Get Source Mode"   ", :SOUR:CURR:MODE?"                   );
//  cli_cmd_reg(":SOUR:VOLT:MODE"     , Cmd_funcModeSet , "Set Source Mode"   ", :SOUR:VOLT:MODE {FIX | SWE | LIST}" );
//  cli_cmd_reg(":SOUR:VOLT:MODE?"    , Cmd_funcModeGet , "Get Source Mode"   ", :SOUR:VOLT:MODE?"                   );
    cli_cmd_reg(":SOUR:CURR:STAR"     , Cmd_CurrStarSet , "Set Sweep Start"   ", :SOUR:CURR:STAR <start>"            );
    cli_cmd_reg(":SOUR:CURR:STAR?"    , Cmd_CurrStarGet , "Get Sweep Start"   ", :SOUR:CURR:STAR?"                   );
//  cli_cmd_reg(":SOUR:VOLT:STAR"     , Cmd_VoltStarSet , "Set Sweep Start"   ", :SOUR:VOLT:STAR <start>"            );
//  cli_cmd_reg(":SOUR:VOLT:STAR?"    , Cmd_VoltStarGet , "Get Sweep Start"   ", :SOUR:VOLT:STAR?"                   );
    cli_cmd_reg(":SOUR:CURR:STOP"     , Cmd_CurrStopSet , "Set Sweep Stop"    ", :SOUR:CURR:STOP <stop>"             );
    cli_cmd_reg(":SOUR:CURR:STOP?"    , Cmd_CurrStopGet , "Get Sweep Stop"    ", :SOUR:CURR:STOP?"                   );
//  cli_cmd_reg(":SOUR:VOLT:STOP"     , Cmd_VoltStopSet , "Set Sweep Stop"    ", :SOUR:VOLT:STOP <stop>"             );
//  cli_cmd_reg(":SOUR:VOLT:STOP?"    , Cmd_VoltStopGet , "Get Sweep Stop"    ", :SOUR:VOLT:STOP?"                   );
    cli_cmd_reg(":SOUR:SWE:POIN"      , Cmd_SwePoinSet  , "Set Sweep Point"   ", :SOUR:SWE:POIN <points>"            );
    cli_cmd_reg(":SOUR:SWE:POIN?"     , Cmd_SwePoinGet  , "Get Sweep Point"   ", :SOUR:SWE:POIN?"                    );

    cli_cmd_reg(":SOUR:PULS:WIDT"  , Cmd_PulsWidtSet , "Set Pulse Width"  ", :SOUR:PULS:WIDT <val>" );
    cli_cmd_reg(":SOUR:PULS:WIDT?" , Cmd_PulsWidtGet , "Get Pulse Width"  ", :SOUR:PULS:WIDT?"      );
    cli_cmd_reg(":SOUR:PULS:PERI"  , Cmd_PulsPeriSet , "Set Pulse Peri"   ", :SOUR:PULS:PERI <val>" );
    cli_cmd_reg(":SOUR:PULS:PERI?" , Cmd_PulsPeriGet , "Get Pulse Peri"   ", :SOUR:PULS:PERI?"      );
    cli_cmd_reg(":SOUR:PULS:COUN"  , Cmd_PulsCounSet , "Set Pulse Point"  ", :SOUR:PULS:COUN <num>" );
    cli_cmd_reg(":SOUR:PULS:COUN?" , Cmd_PulsCounGet , "Get Pulse Point"  ", :SOUR:PULS:COUN?"      );
    cli_cmd_reg(":SOUR:DEL"        , Cmd_SourDlySet  , "Set Source Delay" ", :SOUR:DEL <val>"       );
    cli_cmd_reg(":SOUR:DEL?"       , Cmd_SourDlyGet  , "Get Source Delay" ", :SOUR:DEL?"            );
    cli_cmd_reg(":SOUR:PULS:DEL"   , Cmd_SourDlySet  , "Set Pulse Delay"  ", :SOUR:PULS:DEL <val>"  );
    cli_cmd_reg(":SOUR:PULS:DEL?"  , Cmd_SourDlyGet  , "Get Pulse Delay"  ", :SOUR:PULS:DEL?"       );

//  cli_cmd_reg(":SOUR:RISE:STEP"      , Cmd_funcLevStepSet  , "Set Curr Rise Step"    ", :SOUR:RISE:STEP"           );
//  cli_cmd_reg(":SOUR:RISE:DELAY"     , Cmd_SetCurrRiseDelay, "Set Curr Rise Delay"   ", :SOUR:RISE:DELAY"          );
    cli_cmd_reg(":SOUR:CURR:LEV:STEP"  , Cmd_funcLevStepSet  , "Set Source Level step" ", :SOUR:CURR:LEV:STEP <val>" );
    cli_cmd_reg(":SOUR:CURR:LEV:STEP?" , Cmd_funcLevStepGet  , "Set Source Level step" ", :SOUR:CURR:LEV:STEP?"      );
//  cli_cmd_reg(":SOUR:VOLT:LEV:STEP"  , Cmd_funcLevStepSet  , "Set Source Level step" ", :SOUR:VOLT:LEV:STEP <val>" );
//  cli_cmd_reg(":SOUR:VOLT:LEV:STEP?" , Cmd_funcLevStepGet  , "Set Source Level step" ", :SOUR:VOLT:LEV:STEP?"      );

//  cli_cmd_reg(":SOUR:SAMP:POIN"      , Cmd_funcAverCounSet , "Set Samp Point"        ", :SOUR:SAMP:POIN <point>"   );
//  cli_cmd_reg(":SOUR:SAMP:POIN?"     , Cmd_funcAverCounGet , "Get Samp Point"        ", :SOUR:SAMP:POIN?"          );
    cli_cmd_reg(":SENS:CURR:AVER:COUN" , Cmd_funcAverCounSet , "Set Filter avg number" ", :SENS:CURR:AVER:COUN <n>"  );
    cli_cmd_reg(":SENS:CURR:AVER:COUN?", Cmd_funcAverCounGet , "Get Filter avg number" ", :SENS:CURR:AVER:COUN?"     );
//  cli_cmd_reg(":SENS:VOLT:AVER:COUN" , Cmd_funcAverCounSet , "Set Filter avg number" ", :SENS:VOLT:AVER:COUN <n>"  );
//  cli_cmd_reg(":SENS:VOLT:AVER:COUN?", Cmd_funcAverCounGet , "Get Filter avg number" ", :SENS:VOLT:AVER:COUN?"     );

    return( 0 );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

