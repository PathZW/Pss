/******************************************************************************
 *
 * 文件名  ： PssMcuDrvsFpgaComm.c
 * 负责人  ： zwj
 * 创建日期： 20210802
 * 版本号  ： v1.0
 * 文件描述： PSS 公共实现,基于GOWIN_M1驱动
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/


/*---------------------------------- 预处理区 ---------------------------------*/

/************************************ 头文件 ***********************************/
#include "stdlib.h"
#include "stdio.h"
#include "Drv_FpgaComm.h"

/*----------------------------------- 声明区 ----------------------------------*/

/********************************** 变量声明区 *********************************/
static PSS_MCU_DRVS_COMM_T *sComm = NULL;
/********************************** 函数声明区 *********************************/

/********************************** 变量实现区 *********************************/

/********************************** 函数实现区 *********************************/
void PssMcuDrvsFpgaCommInit(PSS_MCU_DRVS_COMM_T *comm,  uint32_t addr)
{
    comm->commHw = (Comm_TypeDef *)addr;
    sComm = comm;
}

void PssMcuDrvsFpgaCommGetVersion(PSS_MCU_DRVS_COMM_T *comm, uint8_t *pData)
{
    uint32_t temp = 0;
    Comm_TypeDef *commHw = comm->commHw;
    for (uint8_t i = 0; i < 32; i++)
    {
        temp = commHw->fpgaVersion[i];
        pData[4 * i + 0] = (uint8_t)(temp >> 24);
        pData[4 * i + 1] = (uint8_t)(temp >> 16);
        pData[4 * i + 2] = (uint8_t)(temp >> 8);
        pData[4 * i + 3] = (uint8_t)(temp >> 0);
    }
}

uint32_t PssMcuDrvsFpgaCommGetBoardAddr(PSS_MCU_DRVS_COMM_T *comm)
{
    Comm_TypeDef *commHw = comm->commHw;
    return commHw->frameAddrReg;
}

void PssMcuDrvsFpgaCommGetInformation(PSS_MCU_DRVS_COMM_T *comm, uint32_t *pData)
{
    uint8_t i;
    Comm_TypeDef *commHw = comm->commHw;

    for (i = 0; i < 28; i++)
    {
        pData[i] = commHw->debugReg[i];
    }
}

void PssMcuDrvsFpgaCommDelayUs(uint32_t delayUs)
{
    if((!sComm) || (delayUs == 0))
    {
        return;
    }

    Comm_TypeDef *commHw = sComm->commHw;

    /* 清除计数器,写此寄存器可将计数值清0 */
    commHw->counterReg = 0;

    uint32_t startTime = commHw->counterReg;
    /* 计数器单位为25ns,延时单位为us */
    uint32_t delayTime = delayUs * 40;
    if (delayTime < 1)
    {
        delayTime = 1;
    }
    while(1)
    {
        uint32_t nowTime = commHw->counterReg;
        if(nowTime - startTime >= delayTime-1)
        {
            break;
        }
    }
}

uint32_t PssMcuDrvsFpgaCommGetDealyUs(void)
{
    Comm_TypeDef *commHw = sComm->commHw;
    return commHw->counterReg;
}
