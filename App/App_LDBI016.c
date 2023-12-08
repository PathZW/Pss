/*******************************************************************************
 * @file    App_SPWM.c
 * @author  myk(1481974881@qq.com)
 * @version v1.0.0
 * @date    2022/10/24
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
 *      2022/4/24 -- Linghu -- the first version, only support Single-phase SPWM
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                        // Repeated include file 'xxxx'
//lint -e750                        // local macro 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=
//lint -esym(534, cli_cmd_reg, cli_fprintf, cli_fputs, cli_fechof)

#ifdef   APP_CFG
#include APP_CFG                    // Apps configuration
#endif
#define EMB_LOG_PRINT(...)      printf(__VA_ARGS__)
#define  LOG_TAG  "App_SPWM"
#include "emb_log.h"
#include "App_LDBI016.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "cmsis_os2.h"
#include "defines.h"
#include "../Middlewares/cmd/Cmd_UART2.h"
#include "emb_delay.h"
#include "emb_rtcfg.h"
#include "shell/cli_shell.h"

#include "Drv_FpgaPuls.h"

#include "devDriver_DAC.h"
#include "devDriver_ADC.h"


#include "SMU_Source.h"
extern SOUR_STAT_T   _SOUR_Stat;


 //_devDriver_FpgaAD4000_1
extern  devDriver_t    devDriver(FpgaAD4000, 1);
#define FpgaAD4000_1 (&devDriver(FpgaAD4000, 1))
extern  devDriver_t    devDriver(FpgaAD4000, 2);
#define FpgaAD4000_2 (&devDriver(FpgaAD4000, 2))
extern  devDriver_t    devDriver(FpgaAD4000, 3);
#define FpgaAD4000_3 (&devDriver(FpgaAD4000, 3))

//_devDriver_FpgaAD5683_1
extern  devDriver_t    devDriver(FpgaAD5683, 1);
#define FpgaAD5683_1 (&devDriver(FpgaAD5683, 1))
extern  devDriver_t    devDriver(FpgaAD5683, 2);
#define FpgaAD5683_2 (&devDriver(FpgaAD5683, 2))
extern  devDriver_t    devDriver(FpgaAD5683, 3);
#define FpgaAD5683_3 (&devDriver(FpgaAD5683, 3))
extern  devDriver_t    devDriver(FpgaAD5683, 4);
#define FpgaAD5683_4 (&devDriver(FpgaAD5683, 4))

//_devDriver_FpgaAD1259_1
extern  devDriver_t    devDriver(FpgaAD1259, 1);
#define FpgaAD1259_1 (&devDriver(FpgaAD1259, 1))
extern  devDriver_t    devDriver(FpgaAD1259, 2);
#define FpgaAD1259_2 (&devDriver(FpgaAD1259, 2))
extern  devDriver_t    devDriver(FpgaAD1259, 3);
#define FpgaAD1259_3 (&devDriver(FpgaAD1259, 3))

/*******************************************************************************
 * @addtogroup __Config
 * @{
 * @addtogroup              App_SPWM
 * @{
 ******************************************************************************/
 static const osMutexAttr_t  _LDBI16_MutAttr = {
    "App_LDBI16", osMutexRecursive | osMutexPrioInherit | osMutexRobust, NULL, 0
};
       osMutexId_t          _LDBI16_Mutex;              // 重入保护互斥量


/*******************************************************************************
 * @}
 * @}
 * @addtogroup App_SPWM
 * @{
 * @addtogroup              Private_Constants
 * @{
 ******************************************************************************/

extern uint32_t GetBrdNumWithVal(Ldbi016AnlgEnumIIndexT range, double val);
extern PSS_MCU_RANGE_CAL_INDEX_T GetIndexWithIVal(double val, Ldbi016AnlgEnumIIndexT range);
extern PSS_MCU_RANGE_CAL_INDEX_T GetIndexWithVRange(Ldbi016AnlgEnumVIndexT range);
extern PSS_MCU_RANGE_CAL_INDEX_T GetIndexWithOPDRange(Ldbi016AnlgEnumOpmIndexT range);
extern PSS_MCU_RANGE_CAL_INDEX_T GetReadBackIndexWithIVal(double val, Ldbi016AnlgEnumIIndexT range);

/*******************************************************************************
 * @}
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/
#define OS_MUTEX_ENTER(mutex_id)    /* 进入互斥量保护区域 */                   \
        do{ if (osMutexAcquire((mutex_id), osWaitForever) != osOK) while(1) ; \
        } while(0)

#define OS_MUTEX_LEAVE(mutex_id)    /* 离开互斥量保护区域 */  \
        do{ if (osMutexRelease(mutex_id) != osOK) while(1) ; \
        } while(0)

         APP_MCU_T      sApp;
/*******************************************************************************
 * @}
 * @addtogroup              Private_Types
 * @{
 ******************************************************************************/



/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
#ifdef MCU_SOUR
double Mear2Val(uint32_t mear)
{
    uint32_t    M, Ym;

    if((Ym  = 0x07FFFFFF & mear) & 0x04000000) {
        Ym |= 0xF8000000;   // 负数高位补1
    }
    M = mear >> 27;
    return( Ym *  pow(10, M) );
}

/**
 * 获取数据 命令处理函数. :READ？
 */
static int Cmd_ReadFunc(int cli, const char* args)
{
    uint32_t    m, n;
    uint32_t    time, dlyms;
    int32_t     num1 = 0, num2 = 0, num3 = 0;
    uint32_t    mea1 = 0, mea2 = 0, mea3 = 0;
    double   org1, org2, org3, val1, val2, val3;
    PSS_MCU_RANGE_CAL_INDEX_T  index1;
    PSS_MCU_RANGE_CAL_INDEX_T  index2;
    PSS_MCU_RANGE_CAL_INDEX_T  index3;
    dlyms = _SOUR_Stat.mInfo->mDlyDC * 1E3f + 10.5f + 300.0f;
    OS_MUTEX_ENTER(_LDBI16_Mutex);

    switch (_SOUR_Stat.mMode)
    {
    default      :
    case OUT_FIX :
        for(time = GetSysMsec() + dlyms;  GetSysMsec() < time;  )
        {   // 循环读 10ms, 如读到数据则退出循环
            num1 = FpgaAD4000_1->Recv(&mea1, sizeof(mea1), 0x01);
            num2 = FpgaAD4000_2->Recv(&mea2, sizeof(mea2), 0x01);
            num3 = FpgaAD4000_3->Recv(&mea3, sizeof(mea3), 0x01);
            if((num1 == sizeof(mea1)) && (num2 == sizeof(mea2)) && (num3 == sizeof(mea3))) {
                goto OUT_FIX_VALID;
            }
        }
        cli_fprintf(cli, ENDL);
        break;

      OUT_FIX_VALID:
        for(;  (num1 == sizeof(mea1)) && (num2 == sizeof(mea2)) && (num3 == sizeof(mea3));  )
        {   // 已读到测量值, 读空 FIFO
            num1 = FpgaAD4000_1->Recv(&mea1, sizeof(mea1), 0x01);
            num2 = FpgaAD4000_2->Recv(&mea2, sizeof(mea2), 0x01);
            num3 = FpgaAD4000_3->Recv(&mea3, sizeof(mea3), 0x01);
        }
#if 0
        cli_fprintf(cli, "0x%08X,0x%08X,0x%08X,", mea1, mea2, mea3);
#endif
        org1 = mea1 * 5.0 / 65535.0;
        org2 = mea2 * 5.0 / 65535.0;
        org3 = mea3 * 5.0 / 65535.0;
#if 0
        cli_fprintf(cli, "%G,%G,%G,", org1, org2, org3);
#endif
        m      = (_SOUR_Stat.mList->mCnt > 0) ? (_SOUR_Stat.mList->mCnt - 1) : 0;
        index1 = GetIndexWithOPDRange(sApp.AnlgStat.mOpdRange);
        index2 = GetIndexWithVRange(sApp.AnlgStat.mVrange);
        index3 = GetReadBackIndexWithIVal(_SOUR_Stat.mList->mVal[m], sApp.AnlgStat.mIrange);
#if 0
        cli_fprintf(cli, "0x%08X,0x%08X,0x%08X,", index1, index2, index3);
#endif
        val1 = sApp.AnlgStat.mDutVal1 = Load2CalVal(&sApp.AnlgCal, org1, index1);
        val2 = sApp.AnlgStat.mDutVal2 = Load2CalVal(&sApp.AnlgCal, org2, index2);
        val3 = sApp.AnlgStat.mDutVal3 = Load2CalVal(&sApp.AnlgCal, org3, index3);
        cli_fprintf(cli, "%G,%G,%G"ENDL, val1, val2, val3);
        break;

    case SWE_LIN :
        for(time = GetSysMsec() + dlyms;  GetSysMsec() < time;  )
        {   // 循环读 10ms, 如读到数据则退出循环
            num1 = FpgaAD4000_1->Recv(&mea1, sizeof(mea1), 0x01);
            num2 = FpgaAD4000_2->Recv(&mea2, sizeof(mea2), 0x01);
            num3 = FpgaAD4000_3->Recv(&mea3, sizeof(mea3), 0x01);
#if 0
            cli_fprintf(cli, "MEAR: %02X,%02X,%02X,  ", num1 & 0xFF, num2 & 0xFF, num3 & 0xFF);
#endif
            if((num1 == sizeof(mea1)) && (num2 == sizeof(mea2)) && (num3 == sizeof(mea3))) {
                break;
            }
#if 0
            cli_fprintf(cli, ENDL);
#endif
        }
        for(n = 0;  (num1 == sizeof(mea1)) && (num2 == sizeof(mea2)) && (num3 == sizeof(mea3));  n++)
        {   // 已读到测量值, 
#if 0
            cli_fprintf(cli, "0x%04X,0x%04X,0x%04X,  ", mea1 & 0xFFFF, mea2 & 0xFFFF, mea3 & 0xFFFF);
#endif
            org1 = mea1 * 5.0 / 65535.0;
            org2 = mea2 * 5.0 / 65535.0;
            org3 = mea3 * 5.0 / 65535.0;
#if 0
            cli_fprintf(cli, "%-9.8G,%-9.8G,%-9.8G;  ", org1, org2, org3);
#endif
            m      = (_SOUR_Stat.mList->mCnt > 0) ? (_SOUR_Stat.mList->mCnt - 1) : 0;
            index1 =  GetIndexWithOPDRange(sApp.AnlgStat.mOpdRange);
            index2 =  GetIndexWithVRange(sApp.AnlgStat.mVrange);
            index3 =  GetReadBackIndexWithIVal(_SOUR_Stat.mList->mVal[MIN(m, n)], sApp.AnlgStat.mIrange);
#if 0
            cli_fprintf(cli, "%2d,%2d,%2d,  ", index1, index2, index3);
#endif
            val1 = sApp.AnlgStat.mDutVal1 = Load2CalVal(&sApp.AnlgCal, org1, index1);
            val2 = sApp.AnlgStat.mDutVal2 = Load2CalVal(&sApp.AnlgCal, org2, index2);
            val3 = sApp.AnlgStat.mDutVal3 = Load2CalVal(&sApp.AnlgCal, org3, index3);

            cli_fprintf(cli, "%-9.8G,%-9.8G,%-9.8G;", val1, val2, val3);
#if 0
            cli_fprintf(cli, ENDL);
#endif
            num1 = FpgaAD4000_1->Recv(&mea1, sizeof(mea1), 0x01);
            num2 = FpgaAD4000_2->Recv(&mea2, sizeof(mea2), 0x01);
            num3 = FpgaAD4000_3->Recv(&mea3, sizeof(mea3), 0x01);
#if 0
            cli_fprintf(cli, "MEAR: %02X,%02X,%02X,  ", num1 & 0xFF, num2 & 0xFF, num3 & 0xFF);
#endif
        }
        cli_fprintf(cli, ENDL);
        break;
    }

    OS_MUTEX_LEAVE(_LDBI16_Mutex);
    return( 0 );
}

/**
 * 获取SENS模块温度 命令处理函数. :SENS:TEMP?
 */
static int Cmd_GetSensTemp(int cli, const char* args)
{
    float val1[1],val2[1];
    float val1Sum=0,val2Sum=0;
    
    for(int i=0; i<10; i++)
    {
		FpgaAD1259_1->Recv(val1, 1, 1);
		FpgaAD1259_2->Recv(val2, 1, 2);
//        PssMcuDrvsAds1259Read(&sApp.AnlgBrd.ad1259_3, &val3);
        val1Sum+=val1[0];
        val2Sum+=val2[0];
//        val3Sum+=val3;
    }
    val1[0] = val1Sum/10.0f;
    val2[0] = val2Sum/10.0f;
    val1[0] = Load2CalVal(&sApp.AnlgCal, val1[0], PSS_MCU_PT100_A1 + sApp.AnlgStat.mChannel - 1);
    val2[0] = Load2CalVal(&sApp.AnlgCal, val2[0], PSS_MCU_PT100_B1 + sApp.AnlgStat.mChannel - 1);

    sApp.AnlgStat.mTempVal1 = ((2000.0f*val1[0] + 10888.0f)/(134.556f-val1[0])-100.33f)/0.3799f;
    sApp.AnlgStat.mTempVal2 = ((2000.0f*val2[0] + 10888.0f)/(134.556f-val2[0])-100.33f)/0.3799f;
//    sApp.AnlgStat.mTempVal3 = ((2000.0f*(val3Sum/10) + 7020.0f)/(96.49f-(val3Sum/10))-100.33f)/0.3799f;

    cli_fprintf(cli, "%G,%G,%G,%G"ENDL, val1[0],sApp.AnlgStat.mTempVal1 ,val2[0],sApp.AnlgStat.mTempVal2);
    return( 0 );
}

/**
 * 通道设置命令 命令处理函数. :SYST:CHAN <ch>
 */
static int Cmd_SetChannel(int cli, const char* args)
{
    int ch;
    if(sscanf(args, "%*s %d", &ch) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    OS_MUTEX_ENTER(_LDBI16_Mutex);
    sApp.AnlgStat.mChannel = ch;
    LdbiPtPd100Ctrl(ch);
    OS_MUTEX_LEAVE(_LDBI16_Mutex);
    return( 0 );
}

static int Cmd_GetChannel(int cli, const char* args)
{
    cli_fprintf(cli, "%d"ENDL, sApp.AnlgStat.mChannel);
    return (0);
}

/**
 * 获取脉冲采样点数 命令处理函数. :SOUR:SAMP:POIN?
 */
static int Cmd_GetWaterFlow(int cli, const char* args)
{
    OS_MUTEX_ENTER(_LDBI16_Mutex);
    sApp.AnlgStat.mWaterFlow = PssMcuDrvsGetWaterFlow();
//    sApp.AnlgStat.mWaterFlow /= 6.6f;
    PssMcuDrvsMPwmClear();
    OS_MUTEX_LEAVE(_LDBI16_Mutex);
    cli_fprintf(cli, "%G"ENDL, sApp.AnlgStat.mWaterFlow/6.6f);
    return (0);
}

/**
 * 设置获取系统温度 命令处理函数. :SYST:WFLOW?
 */
static int Cmd_SetWaterFlowStatus(int cli, const char* args)
{
    int ch;
    if(sscanf(args, "%*s %d", &ch) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    OS_MUTEX_ENTER(_LDBI16_Mutex);
    /* 低电平导通 */
    if(ch > 0 && ch <9)
    {
        LdbiWaterFlowSwitch(ch, false);
    }
    OS_MUTEX_LEAVE(_LDBI16_Mutex);
    return 0;
}

static int Cmd_GetSourCardID(int cli, const char* args)
{
    cli_fprintf(cli, "%d"ENDL, Ldbi016BrdNumRead()+1);
    return( 0 );
}

static int Cmd_GetFpgaSourVersion(int cli, const char* args)
{
    uint8_t buf[128] = {0};
    PssMcuDrvsFpgaCommGetVersion(&sApp.AnlgBrd.sComm, buf);
    cli_fprintf(cli, "%s"ENDL, buf);
    return( 0 );
}

static int Cmd_GetAdcOriginVal(int cli, const char* args)
{
    int         ch;
    int32_t     orgVal = 0;
    uint32_t    time;
    uint32_t    dlyms = _SOUR_Stat.mInfo->mDlyDC * 1E3f + 10.5f + 280.0f;

    if (sscanf(args, "%*s %u", &ch) < 1) {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    OS_MUTEX_ENTER(_LDBI16_Mutex);
    FpgaAD4000_1->Ctrl(/* DEV_CMD_ADC_MODE_CFG | */ DEV_CMD_FLUSH_RECV, 0x01 /* , DEV_ADC_MODE_NORM */);
    FpgaAD4000_2->Ctrl(/* DEV_CMD_ADC_MODE_CFG | */ DEV_CMD_FLUSH_RECV, 0x01 /* , DEV_ADC_MODE_NORM */);
    FpgaAD4000_3->Ctrl(/* DEV_CMD_ADC_MODE_CFG | */ DEV_CMD_FLUSH_RECV, 0x01 /* , DEV_ADC_MODE_NORM */);

    if(ch == 1)
    {
        for(time = GetSysMsec() + dlyms;  GetSysMsec() < time;  )
        {
            if (FpgaAD4000_1->Recv(&orgVal, sizeof(orgVal), 0x01) == sizeof(orgVal)) {
                break;
            }
        }
        sApp.AnlgStat.mAdcVal1 = orgVal * 5.0 / 65535.0;
        cli_fprintf(cli, "%G"ENDL, sApp.AnlgStat.mAdcVal1);
    }
    else if(ch == 2)
    {
        for(time = GetSysMsec() + dlyms;  GetSysMsec() < time;  )
        {
            if (FpgaAD4000_2->Recv(&orgVal, sizeof(orgVal), 0x01) == sizeof(orgVal)) {
                break;
            }
        }
        sApp.AnlgStat.mAdcVal2 = orgVal * 5.0 / 65535.0;
        cli_fprintf(cli, "%G"ENDL, sApp.AnlgStat.mAdcVal2);
    }
    else if(ch == 3)
    {
        for(time = GetSysMsec() + dlyms;  GetSysMsec() < time;  )
        {
            if (FpgaAD4000_3->Recv(&orgVal, sizeof(orgVal), 0x01) == sizeof(orgVal)) {
                break;
            }
        }
        sApp.AnlgStat.mAdcVal3 = orgVal * 5.0 / 65535.0;
        cli_fprintf(cli, "%G"ENDL, sApp.AnlgStat.mAdcVal3);
    }

    FpgaAD4000_1->Ctrl(/* DEV_CMD_ADC_MODE_CFG | */ DEV_CMD_FLUSH_RECV, 0x01 /* , DEV_ADC_MODE_TRIG */);
    FpgaAD4000_2->Ctrl(/* DEV_CMD_ADC_MODE_CFG | */ DEV_CMD_FLUSH_RECV, 0x01 /* , DEV_ADC_MODE_TRIG */);
    FpgaAD4000_3->Ctrl(/* DEV_CMD_ADC_MODE_CFG | */ DEV_CMD_FLUSH_RECV, 0x01 /* , DEV_ADC_MODE_TRIG */);
    OS_MUTEX_LEAVE(_LDBI16_Mutex);
    return( 0 );
}
#endif

#ifdef MCU_SENS
#if 1    /* 可调电压模块 */
/**
 * 设置保护电压 命令处理函数. :SENS:VOLT:PROT <val>
 */
static int Cmd_SetVoltProfect(int cli, const char* args)
{
#if 1
    float prot;
    if(sscanf(args, "%*s %f", &prot) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    char buf[128] = {0};
    sprintf(buf, ":SENS:VOLT:PROT %f\r\n", prot);
    RS485_2_TxEN();
    UART2_Send(buf, strlen(buf));
    sApp.AnlgStat.mProtVolt = prot;
#endif
    return( 0 );

}

static int Cmd_SetCurrProfect(int cli, const char* args)
{
#if 1
    float prot;
    if(sscanf(args, "%*s %f", &prot) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    char buf[128] = {0};
    sprintf(buf, ":SENS:CURR:PROT %f\r\n", prot);
    UART2_Send(buf, strlen(buf));
    sApp.AnlgStat.mProtVolt = prot;
#endif
    return( 0 );
}

static int Cmd_SetVoltOn(int cli, const char* args)
{
    char fmt[10] = {0};
    if(sscanf(args, "%*s%s", fmt) < 1)
    {
        cli_fechof(cli, "Error of setOutPut command format"ENDL);
        return( 1 );
    }
    OS_MUTEX_ENTER(_LDBI16_Mutex);
    if(cli_strtrimcmp(fmt,"ON") == 0)
    {
        char buf[128] = {0};
        sprintf(buf, ":SPWM:CHNL ON\r\n");
        RS485_2_TxEN();
        UART2_Send(buf, strlen(buf));
    }
    else if(cli_strtrimcmp(fmt,"OFF") == 0)
    {
        char buf[128] = {0};
        sprintf(buf, ":SPWM:CHNL OFF\r\n");
        RS485_2_TxEN();
        UART2_Send(buf, strlen(buf));
    }
    OS_MUTEX_LEAVE(_LDBI16_Mutex);
    return( 0 );
}

/**
 * 获取保护电压 命令处理函数. :SENS:VOLT:PROT?
 */
static int Cmd_GetVoltProfect(int cli, const char* args)
{
    cli_fprintf(cli, "%G"ENDL, sApp.AnlgStat.mProtVolt);
    return( 0 );
}

static int Cmd_GetCurrProfect(int cli, const char* args)
{
    cli_fprintf(cli, "%G"ENDL, sApp.AnlgStat.mProtVolt);
    return( 0 );
}

#endif
/**
 * 设置获取系统温度 命令处理函数. :SYST:TEMP? <ch>
 */

static int Cmd_GetSystTemp(int cli, const char* args)
{
    int ch;
    if(sscanf(args, "%*s %d", &ch) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    } 
    float temp = PssMcuDrvsGetTemp(ch);
    PssMcuDrvsMPwmClear();
    cli_fprintf(cli, "%G"ENDL, temp);
    return (0);
}

/**
 * 设置系统最高工作温度 命令处理函数. :SYST:TEMP:HIGH <val>
 */
static int Cmd_SetHighTemp(int cli, const char* args)
{
    float temp;
    if(sscanf(args, "%*s %f", &temp) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    sApp.AnlgStat.mMaxTemp = temp;
    return( 0 );
}

static int Cmd_GetHighTemp(int cli, const char* args)
{
    cli_fprintf(cli, "%G"ENDL, sApp.AnlgStat.mMaxTemp);
    return( 0 );
}

/**
 * 设置系统最高工作温度 命令处理函数. :SYST:TEMP:HIGH <val>
 */
static int Cmd_SetMinOPD(int cli, const char* args)
{
    float opd;
    if(sscanf(args, "%*s %f", &opd) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    sApp.AnlgStat.mMinOPD = opd;
    return( 0 );
}

static int Cmd_GetMinOPD(int cli, const char* args)
{
    cli_fprintf(cli, "%G"ENDL, sApp.AnlgStat.mMinOPD);
    return( 0 );
}

/**
 * 设置系统最高工作温度 命令处理函数. :SYST:TEMP:HIGH <val>
 */
static int Cmd_SetReadCurr(int cli, const char* args)
{
    float temp;
    if(sscanf(args, "%*s %f", &temp) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    sApp.AnlgStat.mReadCurr = temp;
    return( 0 );
}

static int Cmd_GetReadCurr(int cli, const char* args)
{
    cli_fprintf(cli, "%G"ENDL, sApp.AnlgStat.mReadCurr);
    return( 0 );
}

/**
 * 设置获取系统温度 命令处理函数. :SYST:SLOT?
 */
static int Cmd_GetSlotStatus(int cli, const char* args)
{
    if(LdbiSlotStatus())
        cli_fprintf(cli, "ready"ENDL);
    else
        cli_fprintf(cli, "error"ENDL);
    return 0;
}

///**
// * 设置获取系统温度 命令处理函数. :SYST:LOCK?
// */
//static int Cmd_GetLockStatus(int cli, const char* args)
//{
//    return 0;
//}

/**
 * 设置获取系统温度 命令处理函数. :SYST:WARN?
 */
static int Cmd_GetSystWarn(int cli, const char* args)
{
    if(sApp.AnlgStat.mOverTemp)
    {
        cli_fprintf(cli, "Temp Over"ENDL);
    }
    else if(sApp.AnlgStat.mOverVolt)
    {
        cli_fprintf(cli, "Volt Over"ENDL);
    }
    else
    {
        cli_fprintf(cli, "Normal"ENDL);
    }
    return 0;
}

/**
 * 设置上升沿时间 命令处理函数. :SOUR:PULS:RISE <time>
 */
static int Cmd_SetRiseTime(int cli, const char* args)
{
    uint32_t time;
    if(sscanf(args, "%*s %d", &time) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    OS_MUTEX_ENTER(_LDBI16_Mutex);
    sApp.AnlgStat.mRiseTime = time;
    LdbiSetRiseTime(time);
    OS_MUTEX_LEAVE(_LDBI16_Mutex);
    
    return( 0 );
}

/**
 * 获取上升沿时间 命令处理函数. :SOUR:PULS:RISE?
 */
static int Cmd_GetRiseTime(int cli, const char* args)
{
    cli_fprintf(cli, "%d"ENDL, sApp.AnlgStat.mRiseTime);
    return( 0 );
}

static int Cmd_GetSensCardID(int cli, const char* args)
{
    cli_fprintf(cli, "%d"ENDL, Ldbi016BrdNumRead()+1);
    return( 0 );
}

/**
 * 获取电压采样量程 命令处理函数. :SENS:VOLT:RANG?
 */
static int Cmd_GetVoltRange(int cli, const char* args)
{
    cli_fprintf(cli, (sApp.AnlgStat.mVrange == Ldbi016AnlgEnumVIndex3V)? "3V"ENDL 
                  : ((sApp.AnlgStat.mVrange == Ldbi016AnlgEnumVIndex30V)? "30V"ENDL : "100V"ENDL));
    return( 0 );
}

/**
 * 获取当前最大光功率 命令处理函数. :OPD:POWER:RANG?
 */
static int Cmd_GetMaxPower(int cli, const char* args)
{
    cli_fprintf(cli, "%GmW"ENDL, sApp.AnlgStat.mMaxPower);
    return( 0 );
}

static int Cmd_GetFpgaSensVersion(int cli, const char* args)
{
    uint8_t buf[128] = {0};
    PssMcuDrvsFpgaCommGetVersion(&sApp.AnlgBrd.sComm, buf);
    cli_fprintf(cli, "%s"ENDL, buf);
    return( 0 );
}

#endif
/**
 * 设置电压采样量程 命令处理函数. :SENS:VOLT:RANG <range>
 */
static int Cmd_SetVoltRange(int cli, const char* args)
{
    int range;
    if(sscanf(args, "%*s %d", &range) < 1)
    {
        cli_fechof(cli, "Error of SetVoltRange command format"ENDL);
        return( 1 );
    }

    OS_MUTEX_ENTER(_LDBI16_Mutex);
    switch(range)
    {
        case V_RANGE1:{
            sApp.AnlgStat.mVrange = Ldbi016AnlgEnumVIndex3V;
            break;
        }
        case V_RANGE2:{
            sApp.AnlgStat.mVrange = Ldbi016AnlgEnumVIndex30V;
            break;
        }
        case V_RANGE3:{
            sApp.AnlgStat.mVrange = Ldbi016AnlgEnumVIndex100V;
            break;
        }
        default:{
            /* 根据val匹配量程 */
            break;
        }
    }
#ifdef MCU_SOUR
    LdbiUpdateVoltRange(sApp.AnlgStat.mVrange);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_CALB_CFG, 0x01, 1ul, 0ul, 0ul);
#endif
    OS_MUTEX_LEAVE(_LDBI16_Mutex);
    return( 0 );
}

/**
 * 设置最大光功率 命令处理函数. :OPD:POWER:RANG <range>
 */
static int Cmd_SetMaxPower(int cli, const char* args)
{
    double maxP;
    if(sscanf(args, "%*s %lf", &maxP) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    OS_MUTEX_ENTER(_LDBI16_Mutex);
    sApp.AnlgStat.mMaxPower = maxP;

#ifdef MCU_SOUR
    if(fabs(maxP-0.1) < 0.00001)
    {
        sApp.AnlgStat.mOpdRange = Ldbi016AnlgEnumOpmIndex0_1mW;
        LdbiUpdateOpmRange(Ldbi016AnlgEnumOpmIndex0_1mW);
    }
    else if(fabs(maxP-1.0) < 0.00001)
    {
        sApp.AnlgStat.mOpdRange = Ldbi016AnlgEnumOpmIndex1mW;
        LdbiUpdateOpmRange(Ldbi016AnlgEnumOpmIndex1mW);
    }
    else if(fabs(maxP-10.0) < 0.00001)
    {
        sApp.AnlgStat.mOpdRange = Ldbi016AnlgEnumOpmIndex1_0mW;
        LdbiUpdateOpmRange(Ldbi016AnlgEnumOpmIndex1_0mW);
    }
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_CALB_CFG, 0x01, 1ul, 0ul, 0ul);
#endif
    OS_MUTEX_LEAVE(_LDBI16_Mutex);

    return( 0 );
}

/**
 * 烧录系数 命令处理函数. :PSS:ANLG:CAL:FIRE
 */
static int Cmd_CalFire(int cli, const char* args)
{
    PssMcuCalFire(&sApp.AnlgCal);
    return( 0 );
}

/**
 * 设置系数 命令处理函数. :PSS:ANLG:CAL:PARA <index> <c1> <c0>
 */
static int Cmd_CalSet(int cli, const char* args)
{
    uint32_t index;
    double c1=0, c0=0;
    if(sscanf(args, "%*s" " %u,%lf,%lf", &index, &c1, &c0) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    PssMcuCalSet(&sApp.AnlgCal, index, c1, c0);
    return( 0 );
}

/**
 * 设置系数 命令处理函数. :PSS:ANLG:CAL:PARA <index> <c1> <c0>
 */
static int Cmd_CalRead(int cli, const char* args)
{
#ifdef MCU_SENS
    uint32_t index;
    double c1, c0;
    if(sscanf(args, "%*s %u", &index) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    c1 = PssMcuCalReadC1(&sApp.AnlgCal, index);
    c0 = PssMcuCalReadC0(&sApp.AnlgCal, index);
    cli_fprintf(cli, "%lG,%lG"ENDL, c1, c0);
#endif
    return( 0 );
}

/**
 * 系数复位 命令处理函数. :PSS:ANLG:CAL:RSET
 */
static int Cmd_CalReset(int cli, const char* args)
{
    uint32_t index;
    if(sscanf(args, "%*s %u", &index) < 1)
    {
        cli_fechof(cli, "Error of command format"ENDL);
        return( 1 );
    }
    PssAnlgCalReset(&sApp.AnlgCal, index);
    return( 0 );
}

/**
* 复位设备 命令处理函数
 *
 * @param[in]  cli    命令行接口
 * @param[in]  args   命令字符串, 包括命令名, 但不包括行尾'\r'与'\n'
 *
 * @retval  (>= 0) 命令执行成功
 * @retval  ( < 0) 设备操作异常
 */
static int ResetDevice(int cli, const char* args)
{
//  sApp.AnlgStat.isPulse = false;
//  sApp.AnlgStat.isOutPut = false;
//  sApp.AnlgStat.mPulseWidt = 50;
//  sApp.AnlgStat.mPulsePeriod = 100;
//  sApp.AnlgStat.mPulseNum    = 1;
    sApp.AnlgStat.mSampPonit = 100;
//  sApp.AnlgStat.mOutputDelay = 0;
    sApp.AnlgStat.mRiseTime = 15;
    sApp.AnlgStat.mMaxTemp = 60.0f;
    sApp.AnlgStat.mProtVolt = 45.0f;
    sApp.AnlgStat.mMaxPower = 1;
//  sApp.AnlgStat.mSetCurrVal = 0;
//  sApp.AnlgStat.mSweStart = 0;
//  sApp.AnlgStat.mSweStop = 0;
//  sApp.AnlgStat.mSweStep = 0;
//  sApp.AnlgStat.mSweType = SweTypeTBD;
    sApp.AnlgStat.mDutVal1 = 0;
    sApp.AnlgStat.mDutVal2 = 0;
    sApp.AnlgStat.mDutVal3 = 0;
    sApp.AnlgStat.mTempVal1 = 0.1f;
    sApp.AnlgStat.mTempVal2 = 0.1f;
    sApp.AnlgStat.mTempVal3 = 0.1f;
//  sApp.AnlgStat.mSweType = SweTypeTBD;
    sApp.AnlgStat.mOverTemp = false;
    sApp.AnlgStat.mOverVolt = false;
    sApp.AnlgStat.mVrange = Ldbi016AnlgEnumVIndex3V;
    sApp.AnlgStat.mIrange = Ldbi016AnlgEnumIIndex20A;
    sApp.AnlgStat.mOpdRange = Ldbi016AnlgEnumOpmIndex1mW;
//  sApp.AnlgStat.mRiseDelayMs = 10;
//  sApp.AnlgStat.mRiseStep = 50;
    /* 通道复位 */
#ifdef MCU_SOUR
    LdbiPtPd100Ctrl(0);
    LdbiUpdateVoltRange(sApp.AnlgStat.mVrange);
    LdbiUpdateOpmRange(sApp.AnlgStat.mOpdRange);

    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_CALB_CFG,  0x01, 1ul, 0ul, 0ul);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_CALB_CFG,  0x01, 1ul, 0ul, 0ul);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_CALB_CFG,  0x01, 1ul, 0ul, 0ul);
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_POINT_CFG, 0x01, 1ul);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_POINT_CFG, 0x01, 1ul);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_POINT_CFG, 0x01, 1ul);
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_MODE_CFG | DEV_CMD_FLUSH_RECV, 0x01, DEV_ADC_MODE_TRIG);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_MODE_CFG | DEV_CMD_FLUSH_RECV, 0x01, DEV_ADC_MODE_TRIG);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_MODE_CFG | DEV_CMD_FLUSH_RECV, 0x01, DEV_ADC_MODE_TRIG);
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_RECV_ORGN, 0x01);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_RECV_ORGN, 0x01);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_RECV_ORGN, 0x01);
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_SYNC_CFG,  0x01, 0x01ul, 0x07ul);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_SYNC_CFG,  0x01, 0x01ul, 0x07ul);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_SYNC_CFG,  0x01, 0x01ul, 0x07ul);
    FpgaAD4000_1->Ctrl(DEV_CMD_ADC_START   ,  0x01);
    FpgaAD4000_2->Ctrl(DEV_CMD_ADC_START   ,  0x01);
    FpgaAD4000_3->Ctrl(DEV_CMD_ADC_START   ,  0x01);
#endif
#ifdef MCU_SENS
    LdbiSetRiseTime(sApp.AnlgStat.mRiseTime);
    LdbiUpdateCurrRange(sApp.AnlgStat.mIrange);
#endif
    RS485_RxEN();
    SOUR_Reset(SMU_CHNL_ALL);
    return( 0 );
}

/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/**
 * 初始化
 */
int LDBI016_init(void)
{
    if((_LDBI16_Mutex = osMutexNew(&_LDBI16_MutAttr)) == NULL) {
        return( -1 );
    }
    DelayMS(10);

    /* 板级初始化 */
    DrvLDBI016_Init(&sApp.AnlgBrd);
    DelayMS(10);

#ifdef MCU_SOUR
    FpgaAD4000_1->Init();
    FpgaAD4000_2->Init();
    FpgaAD4000_3->Init();
#endif
    /* 校准模块初始化 */
    PssMcuCalInit(&sApp.AnlgCal);

    /* 命令注册 */
#ifdef MCU_SENS  //控制
    cli_cmd_reg(":SYST:TEMP?",      Cmd_GetSystTemp,        "Get Syst Temp "       ", :SYST:TEMP?");
    cli_cmd_reg(":SENS:ID?",        Cmd_GetSensCardID,          "Get Slot Status "     ", :SYST:SLOT?");

    cli_cmd_reg(":SOUR:PULS:RISE",  Cmd_SetRiseTime,        "Set Rise Time "       ", :SOUR:PULS:RISE <time>");
    cli_cmd_reg(":SOUR:PULS:RISE?", Cmd_GetRiseTime,        "Get Rise Time "       ", :SOUR:PULS:RISE?");

    cli_cmd_reg(":SENS:VOLT:RANG?", Cmd_GetVoltRange,       "Get Volt Range "      ", :SENS:VOLT:RANG?");
    cli_cmd_reg(":OPD:POWER:RANG?", Cmd_GetMaxPower,        "Get MAX Power "       ", :OPD:POWER:RANG?");

    cli_cmd_reg(":SENS:VOLT:PROT",  Cmd_SetVoltProfect,     "Set Volt Profect "    ", :SENS:VOLT:PROT <val>");
    cli_cmd_reg(":SENS:VOLT:PROT?", Cmd_GetVoltProfect,     "Get Volt Profect "    ", :SENS:VOLT:PROT?");
    cli_cmd_reg(":SENS:CURR:PROT",  Cmd_SetCurrProfect,     "Set Volt Profect "    ", :SENS:VOLT:PROT <val>");
    cli_cmd_reg(":SENS:CURR:PROT?", Cmd_GetCurrProfect,     "Get Volt Profect "    ", :SENS:VOLT:PROT?");
    cli_cmd_reg(":SPWM:CHNL" ,      Cmd_SetVoltOn,          "Get Sour Output"      ", :OUTP?");
    
    cli_cmd_reg(":SYST:TEMP:HIGH",  Cmd_SetHighTemp,        "Set Syst Max Temp "   ", :SYST:TEMP:HIGH");
    cli_cmd_reg(":SYST:TEMP:HIGH?", Cmd_GetHighTemp,        "Get Syst Max Temp "   ", :SYST:TEMP:HIGH?");
    cli_cmd_reg(":SYST:OPD:MIN",    Cmd_SetMinOPD,          "Set Syst Min Opd "    ", :SYST1:OPD:MIN");
    cli_cmd_reg(":SYST:OPD:MIN?",   Cmd_GetMinOPD,          "Get Syst Min Opd "   ",  :SYST1:OPD:MIN?");
    cli_cmd_reg(":SYST:CURR:READ",  Cmd_SetReadCurr,        "Set Read Curr Warn"   ", :SYST1:CURR:READ");
    cli_cmd_reg(":SYST:CURR:READ?", Cmd_GetReadCurr,        "Get Read Curr Warn " ",  :SYST1:CURR:READ?");

    cli_cmd_reg(":SYST:WARN?",      Cmd_GetSystWarn,        "Get System Warn "     ", :SYST:WARN?");
    cli_cmd_reg(":SYST:SLOT?",      Cmd_GetSlotStatus,      "Get Slot Status "     ", :SYST:SLOT?");
    cli_cmd_reg(":PSS:FPGA:SENS:VERS?" ,Cmd_GetFpgaSensVersion,        "Get Fpga Version"     ", :FPGA:VERS?");
#endif
#ifdef MCU_SOUR  //采集
    cli_cmd_reg(":READ?",             Cmd_ReadFunc,           "Read data"            ", :READ?");
    cli_cmd_reg(":SENS:TEMP?",        Cmd_GetSensTemp,        "Get Sens Temp "       ", :SENS:TEMP?");

    cli_cmd_reg(":SYST:CHAN",         Cmd_SetChannel,         "Set channel"          ", :SYST:CHAN <ch>");
    cli_cmd_reg(":SYST:CHAN?",        Cmd_GetChannel,         "Get channel"          ", :SYST:CHAN?");
    cli_cmd_reg(":SYST:WFLOW?",       Cmd_GetWaterFlow,       "Get Water Flow"       ", :SYST1:WFLOW?");
    cli_cmd_reg(":SYST:WFLOW",        Cmd_SetWaterFlowStatus, "Set Water Flow Channel"", :SYST1:WFLOW");
    cli_cmd_reg(":SOUR:ID?",          Cmd_GetSourCardID,      "Get ID "              ", :SOUR1:ID?");
    cli_cmd_reg(":PSS:FPGA:SOUR:VERS?" ,  Cmd_GetFpgaSourVersion, "Get Sour Fpga Version" ",:FPGA1:SOUR:VERS?");
    cli_cmd_reg(":PSS:ANLG:VOLT:ADC?", Cmd_GetAdcOriginVal,    "Get ADC Origin "      ", :PSS:ADC:ORIG?");
#endif
    cli_cmd_reg(":SENS:VOLT:RANG",    Cmd_SetVoltRange,       "Set Volt Range "      ", :SENS:VOLT:RANG <range>");
    cli_cmd_reg(":OPD:POWER:RANG",    Cmd_SetMaxPower,        "Set OPD  Range "      ", :OPD:POWER:RANG <val>");
    cli_cmd_reg(":PSS:ANLG:CAL:FIRE", Cmd_CalFire,            "Fire Cal Data "       ", :PSS1:ANLG:CAL:FIRE");
    cli_cmd_reg(":PSS:ANLG:CAL:PARA", Cmd_CalSet,             "Set Cal Data  "       ", :PSS1:ANLG:CAL:PARA");
    cli_cmd_reg(":PSS:ANLG:CAL:RSET", Cmd_CalReset,           "Reset Cal Data "      ", :PSS1:ANLG:CAL:RSET");
    cli_cmd_reg(":PSS:ANLG:CAL:PARA?",Cmd_CalRead,            "Get Cal Data   "      ", :PSS1:ANLG:CAL:PARA?");
 
    cli_cmd_addr(CLI_ADDR_T_UNICAST, (uint16_t)(Ldbi016BrdNumRead()+1));
    ResetDevice(0, NULL);
    return 0;
}

//**********************************   APP FUNC *****************************************//

PSS_MCU_RANGE_CAL_INDEX_T GetIndexWithVRange(Ldbi016AnlgEnumVIndexT range)
{
    PSS_MCU_RANGE_CAL_INDEX_T index;
    switch(range)
    {
        case Ldbi016AnlgEnumVIndex3V:   {index = PSS_MCU_V_RANGE_3V; break; }
        case Ldbi016AnlgEnumVIndex30V : {index = PSS_MCU_V_RANGE_30V; break; }
        case Ldbi016AnlgEnumVIndex100V: {index = PSS_MCU_V_RANGE_100V;break; }
        default : {break;}
    }
    return index;
}

PSS_MCU_RANGE_CAL_INDEX_T GetIndexWithOPDRange(Ldbi016AnlgEnumOpmIndexT range)
{
    PSS_MCU_RANGE_CAL_INDEX_T index;
    switch(range)
    {
        case Ldbi016AnlgEnumOpmIndex0_1mW:  { index = PSS_MCU_OPD_RANGE_0_1mW; break;}
        case Ldbi016AnlgEnumOpmIndex1mW :   { index = PSS_MCU_OPD_RANGE_1_0mW; break; }
        case Ldbi016AnlgEnumOpmIndex1_0mW:  { index = PSS_MCU_OPD_RANGE_10mW;  break; }
        default : {break;}
    }
    return index;
}

PSS_MCU_RANGE_CAL_INDEX_T GetReadBackIndexWithIVal(double val, Ldbi016AnlgEnumIIndexT range)
{
    uint32_t brdNum = 0;
    PSS_MCU_RANGE_CAL_INDEX_T index;
    brdNum = GetBrdNumWithVal(range, val);

    if(Ldbi016AnlgEnumIIndex20A == range)
    {
        index = (PSS_MCU_RANGE_CAL_INDEX_T)(brdNum+33);
    }
    else if(Ldbi016AnlgEnumIIndex60A == range)
    {
        index = (PSS_MCU_RANGE_CAL_INDEX_T)(brdNum+37);
    }
    else if(Ldbi016AnlgEnumIIndex600A == range)
    {
        index = (PSS_MCU_RANGE_CAL_INDEX_T)(brdNum+49);
    }
    return index;
}

       uint32_t GetBrdNumWithVal(Ldbi016AnlgEnumIIndexT range, double val)
{
    float step = 0; 
    uint32_t brdNum = 0;
//    if(Ldbi016AnlgEnumIIndex20A == range)
//    {
//        step = 1.67;
//    }
    if(Ldbi016AnlgEnumIIndex60A == range || Ldbi016AnlgEnumIIndex20A == range)
    {
        step = 5.0;
    }
    else if(Ldbi016AnlgEnumIIndex600A == range)
    {
        step = 50.0;
    }
    for(int i=0; i<11; i++)
    {
        if( val < step || (fabs(val - step) < IMIN))  
        {
            brdNum = 1;
            break;
        }
        else if( (val > ((i+1)*step)  && val < ((i+2)*step))  || (fabs(val - ((i+2)*step)) < IMIN))
        {
            brdNum = i+2;
            break;
        }
    }
    return brdNum;
}


PSS_MCU_RANGE_CAL_INDEX_T GetIndexWithIVal(double val, Ldbi016AnlgEnumIIndexT range)
{
    uint32_t brdNum = 0;
    PSS_MCU_RANGE_CAL_INDEX_T index;
    brdNum = GetBrdNumWithVal(range, val);

    if(Ldbi016AnlgEnumIIndex20A == range)
    {
        index = (PSS_MCU_RANGE_CAL_INDEX_T)(brdNum+5);
    }
    else if(Ldbi016AnlgEnumIIndex60A == range)
    {
        index = (PSS_MCU_RANGE_CAL_INDEX_T)(brdNum+9);
    }
    else if(Ldbi016AnlgEnumIIndex600A == range)
    {
        index = (PSS_MCU_RANGE_CAL_INDEX_T)(brdNum+21);
    }
    return index;
}

/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

