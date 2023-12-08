/********************************************************************************
 *
 * 文件名  ： PssMcuDrvsGpio.c
 * 负责人  :  Myk
 * 创建日期:  2022.11.17
 * 版本号  :  v1.1
 * 文件描述： PSS GowinM1 GPIO驱动 实现
 * 其    他:  无
 * 修改日志:  初始版本       v1.0
 *
 *******************************************************************************/


/*---------------------------------- 预处理区 ---------------------------------*/

/************************************ 头文件 ***********************************/
/************************************ 头文件 ***********************************/
#include "Drv_FpgaGpio.h"
#include "emb_log.h"
/*----------------------------------- 声明区 ----------------------------------*/

/********************************** 变量声明区 *********************************/


/********************************** 函数声明区 *********************************/
static bool IsOut(const PSS_MCU_DRVS_FPGA_GPIO_T *gpio);

/********************************** 变量实现区 *********************************/

/********************************** 函数实现区 *********************************/
void PssMcuDrvsGpioInit(PSS_MCU_DRVS_FPGA_GPIO_T *gpio, void *phy, PSS_MCU_DRVS_GPIO_DIR_T dir, PSS_MCU_DRVS_GPIO_STATE_T state)
{
    gpio->phy   = phy;
    gpio->dir   = dir;
    gpio->state = state;
    PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T *hw = (PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T *)phy;
    if(PSS_MCU_DRVS_GPIO_DIR_OUT == dir)
    {
        *hw->regDir |= hw->pin;
        PssMcuDrvsGpioWrite(gpio, state);
    }
}

bool PssMcuDrvsGpioWrite(PSS_MCU_DRVS_FPGA_GPIO_T *gpio, PSS_MCU_DRVS_GPIO_STATE_T state)
{
    PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T *hw = (PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T *)gpio->phy;
    if(!IsOut(gpio))
    {
        return false;
    }

    if(PSS_MCU_DRVS_GPIO_STATE_H == state)
    {
        *hw->regVal |= hw->pin;
    }
    else
    {
        *hw->regVal &= (~hw->pin);
    }
    gpio->state = state;

    return true;
}

bool PssMcuDrvsGpioToggle(PSS_MCU_DRVS_FPGA_GPIO_T *gpio)
{
//    PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T *hw = (PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T *)gpio->phy;
    if(!IsOut(gpio))
    {
        return false;
    }
    
    PSS_MCU_DRVS_GPIO_STATE_T state = PssMcuDrvsGpioRead(gpio);
    if(PSS_MCU_DRVS_GPIO_STATE_H == state)
    {
        PssMcuDrvsGpioWrite(gpio, PSS_MCU_DRVS_GPIO_STATE_L);
    }
    else
    {
        PssMcuDrvsGpioWrite(gpio, PSS_MCU_DRVS_GPIO_STATE_H);
    }

    return true;
}

PSS_MCU_DRVS_GPIO_STATE_T PssMcuDrvsGpioRead(const PSS_MCU_DRVS_FPGA_GPIO_T *gpio)
{
//    PSS_MCU_DRVS_GPIO_STATE_T state;
    PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T *hw = (PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T *)gpio->phy;
    if(IsOut(gpio))
    {
        return gpio->state;
    }

    /* 输入腿 */
    uint32_t val = (hw->pin & *hw->regVal);
    if(val)
    {
        return PSS_MCU_DRVS_GPIO_STATE_H;
    }
    else
    {
        return PSS_MCU_DRVS_GPIO_STATE_L;
    }
}

bool IsOut(const PSS_MCU_DRVS_FPGA_GPIO_T *gpio)
{
    if(PSS_MCU_DRVS_GPIO_DIR_OUT == gpio->dir)
    {
        return true;
    }
    else
    {
        return false;
    }
}
