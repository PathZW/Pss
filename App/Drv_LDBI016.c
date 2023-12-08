/*******************************************************************************
 * @file    Drv_SPWM.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2022/4/18
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
 *      2022/4/18 -- Linghu -- the first version, only support Single-phase SPWM
 ******************************************************************************/
#ifdef   APP_CFG
#include APP_CFG                    // Apps configuration
#endif
#define  LOG_TAG  "Drv_SPWM"
#include "emb_log.h"
#include "Drv_LDBI016.h"
#include "stm32f4xx_conf.h"
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "defines.h"

#include "shell/cli_shell.h"
#include "board.h"                  // Device's defines
//#include "Board_Device.h"


#define RS485_ADD1_STATUS                GPIO_ReadInputDataBit(READ_BRDID_BIT0_Port,READ_BRDID_BIT0_Pin)
#define RS485_ADD2_STATUS                GPIO_ReadInputDataBit(READ_BRDID_BIT1_Port,READ_BRDID_BIT1_Pin)
#define RS485_ADD3_STATUS                GPIO_ReadInputDataBit(READ_BRDID_BIT2_Port,READ_BRDID_BIT2_Pin)
#define RS485_ADD4_STATUS                GPIO_ReadInputDataBit(READ_BRDID_BIT3_Port,READ_BRDID_BIT3_Pin)


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
 * @addtogroup Drv_SPWM
 * @{
 * @addtogroup              Private_Constants
 * @{
 ******************************************************************************/
static void LdbiGpioCtrl(EnumGpioIndexT pin, bool isH);
static void DrvLDBI016_Gpio_Init(void);

/*  FPGA GPIO控制 */
static PSS_MCU_DRVS_GPIO_OF_GOWINM1_PHY_T sGpioCtrlPhys[] =
{
#ifdef MCU_SENS
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_CHARGE_FULL },           /* 0 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_CHARGE_EMPTY},           /* 1 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_LD_HEAT     },           /* 2 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_R_CTRL_20   },           /* 3 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_R_CTRL_60   },           /* 4 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_R_CTRL_600  },           /* 5 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_I_CTRL_20   },           /* 6 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_I_CTRL_60   },           /* 7 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_I_CTRL_600  },           /* 8 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_DISCHARGE   },           /* 9 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_CHARGE      },           /* 10 L */
#endif
#ifdef MCU_SOUR
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_A_CTR},         /* 0 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_B_CTR},         /* 1 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_C_CTR},         /* 2 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_D_CTR},         /* 3 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_E_CTR},         /* 4 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_F_CTR},         /* 5 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_G_CTR},         /* 6 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_H_CTR},         /* 7 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_I_CTR},         /* 8 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_J_CTR},         /* 9 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_K_CTR},         /* 10 L */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_L_CTR},         /* 11 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_M_CTR},         /* 12 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_N_CTR},         /* 13 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_O_CTR},         /* 14 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, PSS_MCU_FPGA_GPIO_FPGA_DUT_P_CTR},         /* 15 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 16 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 17 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 18 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 19 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 20 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 21 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 22 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 23 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 24 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 25 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 26 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 27 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 28 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 29 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 30 H */
    {GPIO_PORT0_DIR, GPIO_PORT0_VAL, 0u                              },         /* 31 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 32 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 33 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 34 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 35 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 36 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 37 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 38 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 39 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 40 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 41 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 42 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 43 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 44 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 45 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 46 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 47 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 48 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 49 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 50 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 51 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 52 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 53 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 54 H */
    {GPIO_PORT1_DIR, GPIO_PORT0_VAL, 0u                              },         /* 55 H */
    {GPIO_PORT1_DIR, GPIO_PORT1_VAL, PSS_MCU_FPGA_GPIO_PD_0_1MA      },         /* 56 H */
    {GPIO_PORT1_DIR, GPIO_PORT1_VAL, PSS_MCU_FPGA_GPIO_PD___1MA      },         /* 57 H */
    {GPIO_PORT1_DIR, GPIO_PORT1_VAL, PSS_MCU_FPGA_GPIO_PD__10MA      },         /* 58 H */
    {GPIO_PORT1_DIR, GPIO_PORT1_VAL, PSS_MCU_FPGA_GPIO_DUT___3V      },         /* 59 H */
    {GPIO_PORT1_DIR, GPIO_PORT1_VAL, PSS_MCU_FPGA_GPIO_DUT__30V      },         /* 60 H */
    {GPIO_PORT1_DIR, GPIO_PORT1_VAL, PSS_MCU_FPGA_GPIO_DUT_100V      },         /* 61 H */
    {GPIO_PORT1_DIR, GPIO_PORT1_VAL, PSS_MCU_FPGA_GPIO_OUTP_DIS_O    },         /* 62 H */
    {GPIO_PORT1_DIR, GPIO_PORT1_VAL, PSS_MCU_FPGA_GPIO_OUTP_STA_I    },         /* 63 H */
#endif
};
static PSS_MCU_DRVS_FPGA_GPIO_T sGpios[GPIO_SIZE];


/**
 * 初始化 底层相关驱动函数
 */


PSS_MCU_DRVS_BRD_T *mBrd = NULL;

int DrvLDBI016_Init(PSS_MCU_DRVS_BRD_T *brd)
{
    mBrd = brd;
    /* FSMC初始化 */
    Fsmc_init();
    /* GPIO初始化 */
    DrvLDBI016_Gpio_Init();
#ifdef MCU_SOUR
    /* AD/DA初始化 */
    //PssMcuDrvsAds1259Init(&brd->ad1259_1, MCU_DRVS_ADS1259_BASE1);
   // PssMcuDrvsAds1259Init(&brd->ad1259_2, MCU_DRVS_ADS1259_BASE2);
//  PssMcuDrvsAds1259Init(&brd->ad1259_3, MCU_DRVS_ADS1259_BASE3);
    FpgaAD1259_1->Init();
	FpgaAD1259_2->Init();
#endif
#ifdef MCU_SENS
    //PssMcuDrvsAd5683Init(&brd->sAd5683);
     FpgaAD5683_1->Init();
	 FpgaAD5683_1->Init();
	 FpgaAD5683_1->Init();
#endif
    PssMcuDrvsMPwmInit(&brd->sMPwm, PSS_MCU_DRVS_MEAS_PWM_BASE);
    PssMcuDrvsFpgaCommInit(&brd->sComm, PSS_MCU_DRVS_COMM_BASE_ADDR);
    return 0;
}

static void Ldbi016FpgaGpioInit(void)
{
#ifdef MCU_SENS
    PssMcuDrvsGpioInit(&sGpios[ 0], &sGpioCtrlPhys[ 0], PSS_MCU_DRVS_GPIO_DIR_IN,  PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 1], &sGpioCtrlPhys[ 1], PSS_MCU_DRVS_GPIO_DIR_IN,  PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 2], &sGpioCtrlPhys[ 2], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 3], &sGpioCtrlPhys[ 3], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 4], &sGpioCtrlPhys[ 4], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 5], &sGpioCtrlPhys[ 5], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 6], &sGpioCtrlPhys[ 6], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 7], &sGpioCtrlPhys[ 7], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 8], &sGpioCtrlPhys[ 8], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 9], &sGpioCtrlPhys[ 9], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[10], &sGpioCtrlPhys[10], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
#endif
#ifdef MCU_SOUR
    PssMcuDrvsGpioInit(&sGpios[ 0], &sGpioCtrlPhys[ 0], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 1], &sGpioCtrlPhys[ 1], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 2], &sGpioCtrlPhys[ 2], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 3], &sGpioCtrlPhys[ 3], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 4], &sGpioCtrlPhys[ 4], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 5], &sGpioCtrlPhys[ 5], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 6], &sGpioCtrlPhys[ 6], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 7], &sGpioCtrlPhys[ 7], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 8], &sGpioCtrlPhys[ 8], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[ 9], &sGpioCtrlPhys[ 9], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[10], &sGpioCtrlPhys[10], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[11], &sGpioCtrlPhys[11], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[12], &sGpioCtrlPhys[12], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[13], &sGpioCtrlPhys[13], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[14], &sGpioCtrlPhys[14], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[15], &sGpioCtrlPhys[15], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[56], &sGpioCtrlPhys[56], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[57], &sGpioCtrlPhys[57], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[58], &sGpioCtrlPhys[58], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[59], &sGpioCtrlPhys[59], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[60], &sGpioCtrlPhys[60], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[61], &sGpioCtrlPhys[61], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[62], &sGpioCtrlPhys[62], PSS_MCU_DRVS_GPIO_DIR_OUT, PSS_MCU_DRVS_GPIO_STATE_L);
    PssMcuDrvsGpioInit(&sGpios[63], &sGpioCtrlPhys[63], PSS_MCU_DRVS_GPIO_DIR_IN , PSS_MCU_DRVS_GPIO_STATE_L);
#endif
}

static void DrvLDBI016_Gpio_Init(void)
{
	//初始化用到的所有GPIO管脚
    GpioInit();
	//初始化FPGA_GPIO驱动PSS_MCU_DRVS_FPGA_GPIO_T sGpios[GPIO_SIZE]的值
    Ldbi016FpgaGpioInit();
}

uint8_t Ldbi016BrdNumRead(void)
{
    uint8_t brdID = 0;
    brdID |= RS485_ADD4_STATUS;
    brdID <<= 1;
    brdID |= RS485_ADD3_STATUS;
    brdID <<= 1;
    brdID |= RS485_ADD2_STATUS;
    brdID <<= 1;
    brdID |= RS485_ADD1_STATUS;
    return brdID;
}

#ifdef MCU_SENS

void LdbiSetRiseTime(uint32_t time)
{
    LdbiGpioCtrl(Ldbi016GpioIndexRiseCtrl1, true);
    LdbiGpioCtrl(Ldbi016GpioIndexRiseCtrl2, true);
    LdbiGpioCtrl(Ldbi016GpioIndexRiseCtrl3, true);
    LdbiGpioCtrl(Ldbi016GpioIndexRiseCtrl4, true);
}

void LdbiUpdateICtrl(Ldbi016AnlgEnumIIndexT iRange, bool isEnable)
{
    /* 配合FPGA 21路FB管脚使用 */
    switch(iRange)
    {
        case Ldbi016AnlgEnumIIndex20A:
        {
        //  PssMcuDrvsI20ACtrl(&mBrd->sPwm, isEnable);
            PssMcuDrvsGpioWrite(&sGpios[6], (isEnable ? PSS_MCU_DRVS_GPIO_STATE_H : PSS_MCU_DRVS_GPIO_STATE_L));
            PssMcuDrvsGpioWrite(&sGpios[7],             PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[8],             PSS_MCU_DRVS_GPIO_STATE_L);
            break;
        }
        case Ldbi016AnlgEnumIIndex60A:
        {
         // PssMcuDrvsI60ACtrl(&mBrd->sPwm, isEnable);
            PssMcuDrvsGpioWrite(&sGpios[6],             PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[7], (isEnable ? PSS_MCU_DRVS_GPIO_STATE_H : PSS_MCU_DRVS_GPIO_STATE_L));
            PssMcuDrvsGpioWrite(&sGpios[8],             PSS_MCU_DRVS_GPIO_STATE_L);
            break;
        }
        case Ldbi016AnlgEnumIIndex600A:
        {
        //  PssMcuDrvsI600ACtrl(&mBrd->sPwm, isEnable);
            PssMcuDrvsGpioWrite(&sGpios[6],             PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[7],             PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[8], (isEnable ? PSS_MCU_DRVS_GPIO_STATE_H : PSS_MCU_DRVS_GPIO_STATE_L));
            break;
        }
        default:{break;}
    }
//    LOG_INFO("ICtrl = 0x%x\n", PwmRead(&mBrd->sPwm, 2));
}

void LdbiLdHeat(bool isEnable)
{
    if(isEnable)
    PssMcuDrvsGpioWrite(&sGpios[0], PSS_MCU_DRVS_GPIO_STATE_L);
    else
    PssMcuDrvsGpioWrite(&sGpios[0], PSS_MCU_DRVS_GPIO_STATE_H);
}

void LdbiPowerDisCharge(bool isEnable)
{
    if(isEnable)
    PssMcuDrvsGpioWrite(&sGpios[9], PSS_MCU_DRVS_GPIO_STATE_L);
    else
    PssMcuDrvsGpioWrite(&sGpios[9], PSS_MCU_DRVS_GPIO_STATE_H);
}

void LdbiPowerCharge(bool isEnable)
{
    if(isEnable)
    PssMcuDrvsGpioWrite(&sGpios[10], PSS_MCU_DRVS_GPIO_STATE_H);
    else
    PssMcuDrvsGpioWrite(&sGpios[10], PSS_MCU_DRVS_GPIO_STATE_L);
}

bool LdbiSlotStatus(void)
{
   return  GPIO_PIN_READ(GPIOB, GPIO_PIN_0);
}
#endif

#ifdef MCU_SOUR
// 返回控制板输出状态
bool LdbiOutpStaGet(void)
{
    return( PssMcuDrvsGpioRead(&sGpios[63]) != PSS_MCU_DRVS_GPIO_STATE_L );
}


void LdbiUpdateOpmRange(Ldbi016AnlgEnumOpmIndexT opmRange)
{
    switch(opmRange)
    {
        case Ldbi016AnlgEnumOpmIndex0_1mW:
        {
        //  PssMcuDrvsRange0_1MWCtrl(&mBrd->sPwm);
            PssMcuDrvsGpioWrite(&sGpios[56], PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[57], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[58], PSS_MCU_DRVS_GPIO_STATE_H);
            break;
        }
        case Ldbi016AnlgEnumOpmIndex1mW:
        {
        //  PssMcuDrvsRange1MWCtrl(&mBrd->sPwm);
            PssMcuDrvsGpioWrite(&sGpios[56], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[57], PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[58], PSS_MCU_DRVS_GPIO_STATE_H);
            break;
        }
        case Ldbi016AnlgEnumOpmIndex1_0mW:
        {
        //  PssMcuDrvsRange10MWCtrl(&mBrd->sPwm);
            PssMcuDrvsGpioWrite(&sGpios[56], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[57], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[58], PSS_MCU_DRVS_GPIO_STATE_L);
            break;
        }
        default:{break;}
    }
//    LOG_INFO("Opd range 0x%x\n", PwmRead(&mBrd->sPwm, 1));
}

void LdbiUpdateVoltRange(Ldbi016AnlgEnumVIndexT vRange)
{
    switch(vRange)
    {
        case Ldbi016AnlgEnumVIndex3V:
        {
        //  PssMcuDrvsRange3VCtrl(&mBrd->sPwm);
            PssMcuDrvsGpioWrite(&sGpios[59], PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[60], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[61], PSS_MCU_DRVS_GPIO_STATE_H);
            break;
        }
        case Ldbi016AnlgEnumVIndex30V:
        {
        //  PssMcuDrvsRange30VCtrl(&mBrd->sPwm);
            PssMcuDrvsGpioWrite(&sGpios[59], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[60], PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[61], PSS_MCU_DRVS_GPIO_STATE_H);
            break;
        }
        case Ldbi016AnlgEnumVIndex100V:
        {
        //  PssMcuDrvsRange300VCtrl(&mBrd->sPwm);
            PssMcuDrvsGpioWrite(&sGpios[59], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[60], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[61], PSS_MCU_DRVS_GPIO_STATE_L);
            break;
        }
        default:{break;}
    }
//    LOG_INFO("Vrange = 0x%x\n", PwmRead(&mBrd->sPwm, 1));
}

void Ldbi016DutCtrl(bool on, u8 ch)
{
    for(int i = 0;  i < 16;  i++)
    {
        PssMcuDrvsGpioWrite(&sGpios[i], PSS_MCU_DRVS_GPIO_STATE_L);
    }
    if(on && (1 <= ch) && (ch <= 16))
    {
        PssMcuDrvsGpioWrite(&sGpios[ch-1], PSS_MCU_DRVS_GPIO_STATE_H);
    }
}

/* 4组 AA1,BB1,CC1,DD1 IO控制激光器16选1  EE1选择一路常导通，FF1选择1路常导通 */
void LdbiPtPd100Ctrl(uint8_t ch)
{
    {
        /* 8选1控制 AD1259_1 */
        LdbiGpioCtrl(Ldbi016GpioIndexPT100L,   false);
        LdbiGpioCtrl(Ldbi016GpioIndexPT100L1,  true);
        /* LV74_1 */
        LdbiGpioCtrl(Ldbi016GpioIndexPT100E,   true);
        LdbiGpioCtrl(Ldbi016GpioIndexPT100E1,  true);

        /* 4选1控制 AD1259_2 */
        LdbiGpioCtrl(Ldbi016GpioIndexPT100I,   true);
        LdbiGpioCtrl(Ldbi016GpioIndexPT100I1,  true);
    }
    Ldbi016DutCtrl(true, ch);
    switch (ch)
    {
        case 1:{
            /* pt */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100A,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100A1,  true);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_A,     true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_A1,    true);
            break;
        }
        case 2:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100A,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100A1,  false);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_A,     false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_A1,    false);
            break;
        }
        case 3:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100A,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100A1,  false);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_A,     true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_A1,    false);
            break;
        }
        case 4:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100A,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100A1,  true);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_A,     false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_A1,    true);
            break;
        }
        case 5:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100B,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100B1,  true);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_B,     true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_B1,    true);
            break;
        }
        case 6:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100B,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100B1,  false);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_B,     false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_B1,    false);
            break;
        }
        case 7:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100B,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100B1,  false);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_B,     true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_B1,    false);
            break;
        }
        case 8:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100B,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100B1,  true);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_B,     false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_B1,    true);
            break;
        }
        case 9:{
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100C,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100C1,  true);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_C,     true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_C1,    true);
            break;
        }
        case 10:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100C,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100C1,  false);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_C,     false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_C1,    false);
            break;
        }
        case 11:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100C,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100C1,  false);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_C,     true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_C1,    false);
            break;
        }
        case 12:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100C,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100C1,  true);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_C,     false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_C1,    true);
            break;
        }
        case 13:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100D,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100D1,  true);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_D,     true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_D1,    true);
            break;
        }
        case 14:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100D,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100D1,  false);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_D,     false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_D1,    false);
            break;
        }
        case 15:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100D,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100D1,  false);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_D,     true);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_D1,    false);
            break;
        }
        case 16:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100D,   false);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100D1,  true);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_D,     false);
            LdbiGpioCtrl(Ldbi016GpioIndexPD_D1,    true);
            break;
        }
        default:
        {
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100G1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100H1,  true);
            /* pd */
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100K1,  true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J,   true);
            LdbiGpioCtrl(Ldbi016GpioIndexPT100J1,  true);
            break;
        }
    }
}


void LdbiWaterFlowSwitch(u8 ch, bool on)
{
    for(int i=0; i<8; i++)
    {
        LdbiGpioCtrl((EnumGpioIndexT)(Ldbi016GpioIndexPT100L1+i+1), true);
    }
     LdbiGpioCtrl((EnumGpioIndexT)(Ldbi016GpioIndexPT100L1+ch), on);
}
#endif

void LdbiUpdateCurrRange(Ldbi016AnlgEnumIIndexT iRange)
{
    /* 配合FPGA 21路FB管脚使用 */
    switch(iRange)
    {
        case Ldbi016AnlgEnumIIndex20A:
        {
        //  PssMcuDrvsCurrRange20ACtrl(&mBrd->sPwm);
            PssMcuDrvsGpioWrite(&sGpios[3], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[4], PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[5], PSS_MCU_DRVS_GPIO_STATE_L);
            break;
        }
        case Ldbi016AnlgEnumIIndex60A:
        {
        //  PssMcuDrvsCurrRange60ACtrl(&mBrd->sPwm);
            PssMcuDrvsGpioWrite(&sGpios[3], PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[4], PSS_MCU_DRVS_GPIO_STATE_H);
            PssMcuDrvsGpioWrite(&sGpios[5], PSS_MCU_DRVS_GPIO_STATE_L);
            break;
        }
        case Ldbi016AnlgEnumIIndex600A:
        {
        //  PssMcuDrvsCurrRange600ACtrl(&mBrd->sPwm);
            PssMcuDrvsGpioWrite(&sGpios[3], PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[4], PSS_MCU_DRVS_GPIO_STATE_L);
            PssMcuDrvsGpioWrite(&sGpios[5], PSS_MCU_DRVS_GPIO_STATE_H);
            break;
        }
        default:{break;}
    }
//    LOG_INFO("currrange = 0x%x\n", PwmRead(&mBrd->sPwm, 3));
}

void LdbiGpioCtrl(EnumGpioIndexT pin, bool isH)
{
    if(isH)
    {
        DrvsGpioCtrl(pin, true);
    }
    else
    {
        DrvsGpioCtrl(pin, false);
    }
}
/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

