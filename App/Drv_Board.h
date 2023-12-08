/*******************************************************************************
 * @file    Drv_Board.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/6/29
 * @brief   Driver for board
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/6/29 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DRV_BOARD_H__
#define __DRV_BOARD_H__
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "Board_Device.h"
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Driver_Board
 * @{
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
 
typedef struct _PSS_MCU_DRVS_GPIO_TAG_
{
    const char*                 name;   /* Ãû×Ö */
    GPIO_TypeDef*               port;   /* ¶Ë¿Ú */
    unsigned                    PinNum; /* ¹Ü½Å */
    unsigned                    Conf;   /* ÅäÖÃ */
}PSS_MCU_DRVS_GPIO_T;

void RunLED_Init(void);
void RunLED_ON  (void);
void RunLED_OFF (void);

void LinkLED_Init(void);
void LinkLED_ON  (void);
void LinkLED_OFF (void);

void AlarmLED_Init(void);
void AlarmLED_ON  (void);
void AlarmLED_OFF (void);

void    RS485_DirInit(void);
void    RS485_RxEN   (void);
void    RS485_TxEN   (void);
uint8_t RS485_GetAddr(void);

void ETH_PHY_Init (void);
void ETH_PHY_Reset(void);

void GpioInit(void);
void Fsmc_init(void);
void DrvsGpioCtrl(uint32_t num, bool isH);
void FSMC_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize);
void SRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize);
/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __DRV_BOARD_H__
/*****************************  END OF FILE  **********************************/

