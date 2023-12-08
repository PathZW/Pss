/******************************************************************************
 *
 * 文件名  ： PssMcuDrvsFpgaComm.h
 * 负责人  ： zwj
 * 创建日期： 20210802
 * 版本号  ： v1.0
 * 文件描述： PSS 操作FPGA内部FIFO驱动实现,基于GOWIN_M1驱动头文件
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/

/*---------------------------------- 预处理区 ---------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _PSS_MCU_DRVS_FPGA_COMM_H_
#define _PSS_MCU_DRVS_FPGA_COMM_H_

/************************************ 头文件 ***********************************/
#include <stdint.h>

/************************************ 宏定义 ***********************************/
#define PSS_MCU_DRVS_COMM_BASE_ADDR                         (0x6000F000)

/*********************************** 类型定义 **********************************/
typedef struct Comm_TypeDef_Tag
{
    volatile uint32_t hexVerSion1Reg;                                       /* 1. 进制版本信息 秒分 */
    volatile uint32_t hexVerSion2Reg;                                       /* 2. 进制版本信息 年月日时 */
    volatile uint32_t counterReg;                                           /* 3. 计数器 单位25ns */
    volatile uint32_t debugReg[28];                                         /* 4. 调试寄存器 */
    volatile uint32_t frameAddrReg;                                         /* 5. 帧地址寄存器 */
    volatile uint32_t fpgaVersion[32];                                      /* 6. FPGA版本信息128Bytes */
}Comm_TypeDef;

typedef struct PSS_MCU_DRVS_COMM_TAG
{
    char                    *name;                                          /* 名字 */
    Comm_TypeDef            *commHw;                                        /* 公共信息 */
} PSS_MCU_DRVS_COMM_T;


/*--------------------------------- 接口声明区 --------------------------------*/

/*********************************** 全局变量 **********************************/

/*********************************** 接口函数 **********************************/
void PssMcuDrvsFpgaCommInit(PSS_MCU_DRVS_COMM_T *comm,  uint32_t addr);
void PssMcuDrvsFpgaCommGetVersion(PSS_MCU_DRVS_COMM_T *comm, uint8_t *pData);
uint32_t PssMcuDrvsFpgaCommGetBoardAddr(PSS_MCU_DRVS_COMM_T *comm);
void PssMcuDrvsFpgaCommGetInformation(PSS_MCU_DRVS_COMM_T *comm, uint32_t *pData);
void PssMcuDrvsFpgaCommDelayUs(uint32_t delayUs);
uint32_t PssMcuDrvsFpgaCommGetDealyUs(void);
#endif

#ifdef __cplusplus
}
#endif

