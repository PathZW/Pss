/******************************************************************************
 *
 * 文件名  ： PssMcuDrvsAd5683GoWIN_M1.c
 * 负责人  ： myk
 * 创建日期： 20221020
 * 版本号  ： v1.0
 * 文件描述： 
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/


/*---------------------------------- 预处理区 ---------------------------------*/

/************************************ 头文件 ***********************************/
/************************************ 头文件 ***********************************/
#include <string.h>
#include <math.h>
#include "Drv_FpgaPwmTemp.h"
#include "emb_delay.h"
/*----------------------------------- 声明区 ----------------------------------*/

/*----------------------------------- 声明区 ----------------------------------*/
#define    MIN_TEMP               0.00001
#define    MAX_TEMP               90.0
#define    ERROR_TEMP             -1.0
/********************************** 变量声明区 *********************************/
MPwm_TypeDef *pwmHw = NULL;
/********************************** 函数声明区 *********************************/
static float CalculateTemperature(float pwmHz);

float const MCU_HZ_TEMP_MAP[91] = 
{
    2485.010118, 2596.641111, 2712.276419, 2832.013016, 2955.948173, // 0~4
    3084.179362, 3216.804144, 3353.920057, 3495.624504, 3642.014625, // 5~9
    3793.187178, 3949.238404, 4110.2639  , 4276.358473, 4447.616006, // 10~14
    4624.129307, 4805.989967, 4993.2882  , 5186.112695, 5384.550454, // 15~19
    5588.686632, 5798.604379, 6014.384666, 6236.10613 , 6463.844897, // 20~24
    6697.674419, 6937.665302, 7183.885139, 7436.39834 , 7695.265964, // 25~29
    7960.545552, 8232.29096 , 8510.5522  , 8795.375275, 9086.802021, // 30~34
    9384.869957, 9689.612133, 10001.05698, 10319.22819, 10644.14455, // 35~39
    10975.81982, 11314.26266, 11659.47643, 12011.45917, 12370.20344, // 40~44
    12735.69626, 13107.91901, 13486.84738, 13872.4513 , 14264.69487, // 45~49
    14663.53637, 15068.92816, 15480.81673, 15899.14263, 16323.84054, // 50~54
    16754.83923, 17192.06161, 17635.4248 , 18084.84012, 18540.21322, // 55~59
    19001.4441 , 19468.42726, 19941.05173, 20419.20124, 20902.75432, // 60~64
    21391.58445, 21885.56018, 22384.54529, 22888.399  , 23396.9761 , // 65~69
    23910.12713, 24427.69863, 24949.53328, 25475.47016, 26005.34491, // 70~74
    26538.99003, 27076.23504, 27616.90675, 28160.82948, 28707.82534, // 75~79
    29257.71442, 29810.31509, 30365.4442 , 30922.91739, 31482.54928, // 80~84
    32044.15376, 32607.54423, 33172.53384, 33738.93575, 34306.56333, // 85~89
    34875.23047,                                                     // 90
};

void PssMcuDrvsMPwmInit(PSS_MCU_DRVS_MPWM_T *pwm, uint32_t addr)
{
    pwm->pwmHw = (MPwm_TypeDef *)addr;
    pwmHw = pwm->pwmHw;
}

void PssMcuDrvsMPwmClear(void)
{
    pwmHw->clearReg = 1;
}


float PssMcuDrvsGetTemp(int ch)
{
    /* 1.读取频率值 */
    /* 单位25Ns */
    uint32_t HTime = 0, LTime = 0;
    switch(ch)
    {
        case 1:
        {
            HTime = pwmHw->pwm1HighLevelLength;
            LTime = pwmHw->pwm1LowLevelLength;
            break;
        }
        case 2:
        {
            HTime = pwmHw->pwm2HighLevelLength;
            LTime = pwmHw->pwm2LowLevelLength;
            break;
        }
        case 3:
        {
            HTime = pwmHw->pwm3HighLevelLength;
            LTime = pwmHw->pwm3LowLevelLength;
            break;
        }
        case 4:
        {
            HTime = pwmHw->pwm4HighLevelLength;
            LTime = pwmHw->pwm4LowLevelLength;
            break;
        }
        case 5:
        {
            HTime = pwmHw->pwm5HighLevelLength;
            LTime = pwmHw->pwm5LowLevelLength;
            break;
        }
        case 6:
        {
            HTime = pwmHw->pwm6HighLevelLength;
            LTime = pwmHw->pwm6LowLevelLength;
            break;
        }
        case 7:
        {
            HTime = pwmHw->pwm7HighLevelLength;
            LTime = pwmHw->pwm7LowLevelLength;
            break;
        }
        case 8:
        {
            HTime = pwmHw->pwm8HighLevelLength;
            LTime = pwmHw->pwm8LowLevelLength;
            break;
        }
    }
    HTime *= 25;
    LTime *= 25;
    if(HTime == 0 || LTime == 0)
    {
        return ERROR_TEMP;
    }

    float pwmHz =  1 / (float)((HTime + LTime)*1e-9);

    return CalculateTemperature(pwmHz);
}

float PssMcuDrvsGetWaterFlow(void)
{
    /* 1.读取频率值 */
    /* 单位25Ns */
    uint32_t HTime = 0, LTime = 0;

    HTime = pwmHw->pwm9HighLevelLength;
    LTime = pwmHw->pwm9LowLevelLength;

    HTime *= 25;
    LTime *= 25;
    if(HTime == 0 || LTime == 0)
    {
        return ERROR_TEMP;
    }

    float pwmHz =  1 / (float)((HTime + LTime)*1e-9);

    return pwmHz;
}

static float CalculateTemperature(float pwmHz)
{
    uint8_t cBottom = 0, cTop = 0;
    uint8_t i = 0, iTem = 0;
    float fLowRValue = 0;
    float fHighRValue = 0;
    float fTem = 0;

    if(pwmHz > MCU_HZ_TEMP_MAP[90])
    {
        return MAX_TEMP;
    }

    if(pwmHz < MCU_HZ_TEMP_MAP[0])
    {
        return MIN_TEMP;
    }
    cBottom = 0;
    cTop = 90;

    /*升序2分法查表 */
    for(i = 45; (cTop - cBottom) != 1; )
    {
        if(pwmHz > MCU_HZ_TEMP_MAP[i])
        {
            cBottom = i;
            i = (cTop + cBottom) / 2;
        }
        else if(pwmHz < MCU_HZ_TEMP_MAP[i])
        {
            cTop = i;
            i = (cTop + cBottom) / 2;
        }
        else
        {
            iTem = i;
            fTem = (float)iTem;
            return fTem;
        }
    }
    iTem = i;
    
    fLowRValue  = MCU_HZ_TEMP_MAP[cBottom];
    fHighRValue = MCU_HZ_TEMP_MAP[cTop];

    fTem = ((fLowRValue - pwmHz) / (fLowRValue - fHighRValue) ) + iTem;
    
    return fTem;
}

