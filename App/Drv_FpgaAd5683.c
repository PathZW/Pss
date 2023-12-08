/******************************************************************************
 *
 * 文件名  ： PssMcuDrvsAd5683GoWIN_M1.c
 * 负责人  ： zwj
 * 创建日期： 20200827
 * 版本号  ： v1.0
 * 文件描述： PSS AD5683驱动
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/


/*---------------------------------- 预处理区 ---------------------------------*/

/************************************ 头文件 ***********************************/
/************************************ 头文件 ***********************************/
#include <string.h>
#include <math.h>
#include "Drv_FpgaAd5683.h"
#include "emb_log.h"

/*----------------------------------- 声明区 ----------------------------------*/

/********************************** 变量声明区 *********************************/

/********************************** 函数声明区 *********************************/
static void WriteCtrlReg(Ad5683_TypeDef *ad5683Hw, const PSS_MCU_DRVS_AD5683_PARA_T *para);
static bool IsFullFifo(Ad5683_TypeDef *ad5683Hw);
static bool IsBusyFifo(Ad5683_TypeDef *ad5683Hw);
static void WriteValToReg(Ad5683_TypeDef *ad5683Hw, uint8_t chipNum, bool isPulse, uint16_t val);
/********************************** 变量实现区 *********************************/

/********************************** 函数实现区 *********************************/

void PssMcuDrvsAd5683Init(PSS_MCU_DRVS_AD5683_T *ad5683)
{
    PSS_MCU_DRVS_AD5683_PARA_T para = {0};

    ad5683->ad5683Hw = (Ad5683_TypeDef *)PSS_MCU_DRVS_AD5683_CMD_BASE;
    Ad5683_TypeDef *ad5683Hw = ad5683->ad5683Hw;

    /* step1: 给定芯片的配置 */
    para.ovr = PSS_MCU_DRVS_AD5683_CTRL_OVR_DI;
    para.b2c = PSS_MCU_DRVS_AD5683_CTRL_B2Z_SB;
    para.ets = PSS_MCU_DRVS_AD5683_CTRL_ETS_ON;
    para.pv  = PSS_MCU_DRVS_AD5683_CTRL_PV_ZERO;
    para.ra  = PSS_MCU_DRVS_AD5683_CTRL_RA_0_P5;
    para.cv  = PSS_MCU_DRVS_AD5683_CTRL_CV_ZERO;

    /* 上电初始化 */
    WriteCtrlReg(ad5683Hw, &para);
}

static void WriteCtrlReg(Ad5683_TypeDef *ad5683Hw, const PSS_MCU_DRVS_AD5683_PARA_T *para)
{
    uint32_t regVal = ad5683Hw->cmdReg;
    /* 暂时只用到了一个：只初始化第一个5683 */
    regVal |= (PSS_MCU_DRVS_AD5683_0_MASK << PSS_MCU_DRVS_AD5683_NUM_CTRL_OFFSET);
    regVal |= (PSS_MCU_DRVS_AD5683_1_MASK << PSS_MCU_DRVS_AD5683_NUM_CTRL_OFFSET);
    regVal |= (PSS_MCU_DRVS_AD5683_2_MASK << PSS_MCU_DRVS_AD5683_NUM_CTRL_OFFSET);
    regVal |= 0x00498000; 

    ad5683Hw->cmdReg = regVal;
}

void PssMcuDrvsAd5683SetVal(PSS_MCU_DRVS_AD5683_T *ad5683, bool isPulse, double val)
{
    Ad5683_TypeDef *ad5683Hw = ad5683->ad5683Hw;

    uint16_t setVal = (uint16_t)(val * 26214);
    if(val < 0)
    {
        setVal = 0;
    }
    else if(val > 2.5)
    {
        setVal = 2.5 * 26214;
    }

    if(isPulse && !IsFullFifo(ad5683Hw))
    {
        WriteValToReg(ad5683Hw, PSS_MCU_DRVS_AD5683_ALL_MASK, true, setVal);
    }
    else if(!isPulse && !IsBusyFifo(ad5683Hw))
    {
        WriteValToReg(ad5683Hw, PSS_MCU_DRVS_AD5683_ALL_MASK, false, setVal);
    }
}

void PssMcuDrvsChoseAd5683SetVal(PSS_MCU_DRVS_AD5683_T *ad5683, uint8_t chipNum, bool isPulse, double val)
{
    Ad5683_TypeDef *ad5683Hw = ad5683->ad5683Hw;

    uint16_t setVal = (uint16_t)(val * 26214);
    if(val < 0)
    {
        setVal = 0;
    }
    else if(val > 2.5)
    {
        setVal = 2.5 * 26214;
    }

    if(isPulse && !IsFullFifo(ad5683Hw))
    {
        WriteValToReg(ad5683Hw, chipNum, true, setVal);
    }
    else if(!isPulse && !IsBusyFifo(ad5683Hw))
    {
        WriteValToReg(ad5683Hw, chipNum, false, setVal);
    }
}

static bool IsFullFifo(Ad5683_TypeDef *ad5683Hw)
{
/*
Bit0：cmd fifo full
Bit1：cmd fifo empty
Bit2：ad5683 controller busy
*/
    uint32_t state = ad5683Hw->fifoStateReg & PSS_MCU_DRVS_AD5683_CMD_FIFO_FULL;
    if(PSS_MCU_DRVS_AD5683_CMD_FIFO_FULL == state)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool IsBusyFifo(Ad5683_TypeDef *ad5683Hw)
{
    uint32_t state = ad5683Hw->fifoStateReg & PSS_MCU_DRVS_AD5683_CMD_FIFO_BUSY;
    if(PSS_MCU_DRVS_AD5683_CMD_FIFO_BUSY == state)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void WriteValToReg(Ad5683_TypeDef *ad5683Hw, uint8_t chipNum, bool isPulse, uint16_t val)
{
    uint32_t regVal = 0;
    if(isPulse)
    {
        regVal = ad5683Hw->fifoReg;
    }
    else
    {
        regVal = ad5683Hw->cmdReg;
    }

    /* 5683[16:19] : Write to input register (no update) */
    regVal |= (PSS_MCU_DRVS_AD5683_WRITE_REG_ADDR << PSS_MCU_DRVS_AD5683_CTRL_REG_OFFSET);
    regVal = val;
    regVal <<= 4;
    regVal |= (chipNum << PSS_MCU_DRVS_AD5683_NUM_CTRL_OFFSET);
    regVal |= (0x03 << 20);

    if(isPulse)
    {
        ad5683Hw->fifoReg = regVal;
    }
    else
    {
        ad5683Hw->cmdReg = regVal;
    }
}

void PssMcuDrvsAd5683Reset(PSS_MCU_DRVS_AD5683_T *ad5683)
{
    Ad5683_TypeDef *ad5683Hw = ad5683->ad5683Hw;
    ad5683Hw->resetReg = PSS_MCU_DRVS_AD5683_RESET_FLAG;
}
