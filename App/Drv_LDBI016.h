/*******************************************************************************
 * @file    Drv_SPWM.h
 * @author  mayk(1481974881@qq.com)
 * @version v1.0.0
 * @date    2022/10/18
 * @brief   SPWM driver for STM32
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
 *      2022/4/18 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DRV_LDBI016_H__
#define __DRV_LDBI016_H__
/******************************************************************************/
#include <stdint.h>
#include "Drv_FpgaGpio.h"
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
// #include "Drv_FpgaAd4000.h"
#include "devDriver_FpgaAD1259.h"
#include "devDriver_FpgaAD5683.h"
#include "Drv_Board.h"
#include "Drv_FpgaPwmTemp.h"
#include "Drv_FpgaComm.h"
#include "devDriver_Common.h"
/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
 #if 0
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
#endif



typedef enum Ldbi016AnlgEnumGpioIndexTag_
{
#ifdef MCU_SOUR
    Ldbi016GpioIndexPT100A               = 0,
    Ldbi016GpioIndexPT100A1              = 1,
    Ldbi016GpioIndexPT100B               = 2,
    Ldbi016GpioIndexPT100B1              = 3,
    Ldbi016GpioIndexPT100C               = 4,
    Ldbi016GpioIndexPT100C1              = 5,
    Ldbi016GpioIndexPT100D               = 6,
    Ldbi016GpioIndexPT100D1              = 7,
    Ldbi016GpioIndexPT100E               = 8,
    Ldbi016GpioIndexPT100E1              = 9,
    Ldbi016GpioIndexPT100F               = 10,
    Ldbi016GpioIndexPT100F1              = 11,
    Ldbi016GpioIndexPT100G               = 12,
    Ldbi016GpioIndexPT100G1              = 13,
    Ldbi016GpioIndexPT100H               = 14,
    Ldbi016GpioIndexPT100H1              = 15,
    Ldbi016GpioIndexPT100I               = 16,
    Ldbi016GpioIndexPT100I1              = 17,
    Ldbi016GpioIndexPT100J               = 18,
    Ldbi016GpioIndexPT100J1              = 19,
    Ldbi016GpioIndexPT100K               = 20,
    Ldbi016GpioIndexPT100K1              = 21,
    Ldbi016GpioIndexPT100L               = 22,
    Ldbi016GpioIndexPT100L1              = 23,
    /* 8路水流量 */
    Ldbi016GpioIndexLA                   = 24,
    Ldbi016GpioIndexLB                   = 25,
    Ldbi016GpioIndexLC                   = 26,
    Ldbi016GpioIndexLD                   = 27,
    Ldbi016GpioIndexLE                   = 28,
    Ldbi016GpioIndexLF                   = 29,
    Ldbi016GpioIndexLG                   = 30,
    Ldbi016GpioIndexLH                   = 31,
    /* 16路PD */
    Ldbi016GpioIndexPD_A                 = 32,
    Ldbi016GpioIndexPD_A1                = 33,
    Ldbi016GpioIndexPD_B                 = 34,
    Ldbi016GpioIndexPD_B1                = 35,
    Ldbi016GpioIndexPD_C                 = 36,
    Ldbi016GpioIndexPD_C1                = 37,
    Ldbi016GpioIndexPD_D                 = 38,
    Ldbi016GpioIndexPD_D1                = 39,
#endif
#ifdef MCU_SENS
    Ldbi016GpioIndexRunLed               = 0,
    Ldbi016GpioIndexAlarmLed             = 1,
    Ldbi016GpioIndexPowerLed             = 2,
    Ldbi016GpioIndexRiseCtrl1            = 3,
    Ldbi016GpioIndexRiseCtrl2            = 4,
    Ldbi016GpioIndexRiseCtrl3            = 5,
    Ldbi016GpioIndexRiseCtrl4            = 6,
    Ldbi016GpioIndexPower15P             = 7,
    Ldbi016GpioIndexPower15N             = 8,
    Ldbi016GpioIndexIRead20              = 9,
    Ldbi016GpioIndexIRead60              = 10,
    Ldbi016GpioIndexIRead600             = 11,
    Ldbi016GpioIndexRRead20              = 12,
    Ldbi016GpioIndexRRead60              = 13,
    Ldbi016GpioIndexRRead600             = 14,
    Ldbi016GpioIndexVVin                 = 15,
    Ldbi016GpioIndexVVout                = 16,
    Ldbi016GpioIndexIVin                 = 17,
    Ldbi016GpioIndexIVout                = 18,
    Ldbi016GpioIndexSlotCheck            = 19,
 #endif
} EnumGpioIndexT;


typedef enum  Ldbi016AnlgEnumVoltIndexTag_
{
    Ldbi016AnlgEnumVIndexTBD              = 0x00,                          /* 未定义 */
    Ldbi016AnlgEnumVIndex3V               = 0x01,                          /* 3V量程 */
    Ldbi016AnlgEnumVIndex30V              = 0x02,                          /* 30V量程 */
    Ldbi016AnlgEnumVIndex100V             = 0x03,                          /* 100V量程 */
} Ldbi016AnlgEnumVIndexT;

typedef enum  Ldbi016AnlgEnumCurrIndexTag_
{
    Ldbi016AnlgEnumIIndexTBD              = 0x00,                          /* 未定义 */
    Ldbi016AnlgEnumIIndex20A              = 0x01,                          /* 20A量程 */
    Ldbi016AnlgEnumIIndex60A              = 0x02,                          /* 60A量程 */
    Ldbi016AnlgEnumIIndex600A             = 0x03,                          /* 600A量程 */
} Ldbi016AnlgEnumIIndexT;

typedef enum  Ldbi016AnlgEnumOpmIndexTag_
{
    Ldbi016AnlgEnumOpmIndexTBD           = 0x00,                          /* 未定义 */
    Ldbi016AnlgEnumOpmIndex0_1mW         = 0x01,                          /* 0.1mW量程 */
    Ldbi016AnlgEnumOpmIndex1mW           = 0x02,                          /* 1mW量程 */
    Ldbi016AnlgEnumOpmIndex1_0mW         = 0x03,                          /* 10mW量程 */
} Ldbi016AnlgEnumOpmIndexT;

typedef struct PSS_MCU_DRVS_BRD_TAG_
{
#ifdef MCU_SOUR
    //PSS_MCU_DRVS_ADS1259_T ad1259_1;
    //PSS_MCU_DRVS_ADS1259_T ad1259_2;
    //PSS_MCU_DRVS_ADS1259_T ad1259_3;
//    PSS_MCU_DRVS_AD4000_T  ad4000_1;
//    PSS_MCU_DRVS_AD4000_T  ad4000_2;
//    PSS_MCU_DRVS_AD4000_T  ad4000_3;
#endif
#ifdef MCU_SENS
    //PSS_MCU_DRVS_AD5683_T  sAd5683;
#endif

    PSS_MCU_DRVS_MPWM_T    sMPwm;
    PSS_MCU_DRVS_COMM_T    sComm;

}PSS_MCU_DRVS_BRD_T;


#ifdef MCU_SENS
void LdbiSetRiseTime(uint32_t time);
bool LdbiSlotStatus(void);
void LdbiUpdateICtrl(Ldbi016AnlgEnumIIndexT iRange, bool isEnable);
void LdbiPowerDisCharge(bool isEnable);
void LdbiPowerCharge(bool isEnable);
#endif
#ifdef MCU_SOUR
bool LdbiOutpStaGet(void);
void LdbiUpdateVoltRange(Ldbi016AnlgEnumVIndexT vRange);
void LdbiUpdateOpmRange(Ldbi016AnlgEnumOpmIndexT opmRange);
void LdbiPtPd100Ctrl(uint8_t ch);
void LdbiWaterFlowSwitch(uint8_t ch, bool on);
#endif
void LdbiUpdateCurrRange(Ldbi016AnlgEnumIIndexT iRange);
int DrvLDBI016_Init(PSS_MCU_DRVS_BRD_T *brd);
uint8_t Ldbi016BrdNumRead(void);

/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __DRV_LDBI016_H__
/*****************************  END OF FILE  **********************************/

