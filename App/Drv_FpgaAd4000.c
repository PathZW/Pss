/********************************************************************************
 *
 * 文件名  ： PssMcuDrvsFpgaAd4000.c
 * 负责人  :  zwj
 * 创建日期:  2021.10.18
 * 版本号  :  v1.0
 * 文件描述： PSS Mcu AD4000 接口实现
 * 其    他:  无
 * 修改日志:  无
 *
 *******************************************************************************/


/*---------------------------------- 预处理区 ---------------------------------*/

/************************************ 头文件 ***********************************/
/************************************ 头文件 ***********************************/
#include "Drv_FpgaAd4000.h"
/*----------------------------------- 声明区 ----------------------------------*/
#define _START_SAMPLE_MODE_TRIG_          (0x00)
#define _START_SAMPLE_MODE_FRAME_         (0x01)
#define _DELAY_MIN_UINT_NS_               (25)
#define _CONFIG_FINISH_FLAG_              (0x01)
#define _START_                           (0x01)
#define _STOP_                            (0x01)
#define _CLEAR_                           (0x01)
/********************************** 变量声明区 *********************************/


/********************************** 函数声明区 *********************************/

/********************************** 变量实现区 *********************************/

/********************************** 函数实现区 *********************************/
int PssMcuDrvsAd4000Init(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t addr)
{
    if(ad4000->isSynchronousCtrAd)
    {
        ad4000->phy = (Ad4000_Synchronous_Ctrl_Typedef *)addr;
    }
    else
    {
        ad4000->phy = (Ad4000_TypeDef *)addr;
    }
    PssMcuDrvsAd4000SetStartMode(ad4000, true);
//    PssMcuDrvsAd4000SetWaitSampleTime(ad4000, 2000);
//    PssMcuDrvsAd4000SetSampleGroupNum(ad4000, 1000);
    return 0;
}

int PssMcuDrvsAd4000SyncInit(void)
{
    Ad4000_Synchronous_Ctrl_Typedef *ad4000sync = (Ad4000_Synchronous_Ctrl_Typedef*)PSS_MCU_DRVS_AD4000_SYNCHRONOUS_CTRL_BASE_ADDR;
    ad4000sync->adSynchronousStart = 0x0111;
    return 0;
}

int PssMcuDrvsAd4000SetRangeInfo(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t val)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->rangeInfo = val;
    return 0;
}
int PssMcuDrvsAd4000SetCalPara(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t c0, uint32_t c1)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->c0Adc = c0;
    hw->c1Adc = c1;
    return 0;
}
int PssMcuDrvsAd4000SetWaitSampleTime(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t timeNs)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->outpStableDelay = timeNs / _DELAY_MIN_UINT_NS_;
    return 0;
}
int PssMcuDrvsAd4000SetSampleNum(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t sampleNum)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->sampleNum = sampleNum;
    return 0;
}
int PssMcuDrvsAd4000SetClearConfig(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->clear = _CLEAR_;
    return 0;
}

int PssMcuDrvsAd4000SetSampleGroupNum(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t num)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->sampleGroupNum = num;
    return 0;
}
int PssMcuDrvsAd4000SetSamplePara(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t sampleFrequency, uint32_t extractionRate, uint32_t sampleNum)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->sampleFrequency = sampleFrequency;
    hw->extractionRate = extractionRate;
    hw->sampleNum = sampleNum;
    return 0;
}
int PssMcuDrvsAd4000SetConfigFinish(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->configFinish = _CONFIG_FINISH_FLAG_;
    return 0;
}
int PssMcuDrvsAd4000SetTrig(PSS_MCU_DRVS_AD4000_T *ad4000, TrigConfig *trig)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    /* 具体意义参考《高云FPGA的APB总线说明》中的Ad4000模块 */
    uint32_t val = 0;
    uint8_t checkVal = 0;
    val |= trig->trigMode;
    if(trig->isNeedSign)
    {
        checkVal |= 0x01 << 7;
    }
    else
    {
        checkVal |= 0x00 << 7;
    }
    checkVal |= (uint8_t)trig->signNum;
    val |= (checkVal << 8);
    val |= (uint8_t)trig->trigLine << 16;

    hw->trigConfig = val;
    return 0;
}
int PssMcuDrvsAd4000SetStartMode(PSS_MCU_DRVS_AD4000_T *ad4000, bool isTrig)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    if(isTrig)
    {
        hw->startMode = _START_SAMPLE_MODE_TRIG_;
    }
    else
    {
        hw->startMode = _START_SAMPLE_MODE_FRAME_;
    }
    return 0;
}
int PssMcuDrvsAd4000SetSampleCycTimes(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t val)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->sampleCycTimes = val;
    return 0;
}

bool PssMcuDrvsIsAd4000IntArrived(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    if((hw->interrupt & 0x02) == 0x02)
    {
        return 1;
    }
    return 0;
}

int PssMcuDrvsAd4000ClearInt(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->interrupt = 1;
    return 0;
}
int PssMcuDrvsAd4000SetVoltFlaot(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t val)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->voltFlaot = val;
    return 0;
}
uint32_t PssMcuDrvsAd4000GetMaxVal(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    return hw->maxVal;
}
uint32_t PssMcuDrvsAd4000GetStableVal(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    return hw->stableVal;
}
float PssMcuDrvsAd4000GetAdVal(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    bool isP = false;
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    if((hw->fifoState & 0x01) != 0x01)
    {
        uint32_t adc = hw->adValFifo;
        /* 16位AD,设置范围为0-5V */
        if(adc & 0x8000)
        {
            adc = ~adc + 1;
            isP = true;
        }
        adc &= 0x7fff;  //去除符号位
        float val = adc * (2.5f / 32767.0f);
        if(isP) val = -val;
        return val;
    }
    return -1;
}

float PssMcuDrvsAd4000GetRealAdVal(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    bool isP = false;
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    //if((hw->fifoState & 0x01) != 0x01)
    {
        uint32_t adc = hw->realVal;
        /* 16位AD,设置范围为0-5V */
//        if(adc & 0x8000)
//        {
//            adc &= 0x7fff; 
//            adc = ~adc + 1;
//            isP = true;
//        }
//        adc &= 0x7fff;  //去除符号位
        float val = adc * (5.0f / 65535.0f);
        if(isP) val = -val;
        return val;
    }
}

uint32_t PssMcuDrvsAd4000GetCalVal(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    if((hw->fifoState & 0x04) != 0x04)
    {
        return hw->calValFifo;
    }
    return 0;
}

void PssMcuDrvsAd4000ClearFifo(PSS_MCU_DRVS_AD4000_T *ad4000)
{
    Ad4000_TypeDef *hw = (Ad4000_TypeDef *)ad4000->phy;
    hw->clearFifo = 0x03;
    hw->clearFifo = 0x0;
}

int PssMcuDrvsAd4000SetStartSample(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t type)
{
    Ad4000_Synchronous_Ctrl_Typedef *hw = (Ad4000_Synchronous_Ctrl_Typedef *)ad4000->phy;
    hw->adSynchronousStart = type;
    return 0;
}
int PssMcuDrvsAd4000SetStopSample(PSS_MCU_DRVS_AD4000_T *ad4000, uint32_t type)
{
    Ad4000_Synchronous_Ctrl_Typedef *hw = (Ad4000_Synchronous_Ctrl_Typedef *)ad4000->phy;
    hw->adSynchronousStop = type;
    return 0;
}
