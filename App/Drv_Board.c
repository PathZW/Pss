/*******************************************************************************
 * @file    Drv_Board.c
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
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#define  LOG_TAG  "Board"
#include "emb_log.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_conf.h"
#include "board.h"              // Device's defines
#include "Drv_Board.h"


/*******************************************************************************
 * @addtogroup Driver_Board
 * @{
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/
static PSS_MCU_DRVS_GPIO_T sLDBI_Gpio[] = {
#ifdef MCU_SOUR /* 采集板 */
    
    {"MCU_PT100A",      MCU_PT100A_port,      MCU_PT100A_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 0 */
    {"MCU_PT100A1",     MCU_PT100A1_port,     MCU_PT100A1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 1 */
    {"MCU_PT100B",      MCU_PT100B_port,      MCU_PT100B_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 2 */
    {"MCU_PT100B1",     MCU_PT100B1_port,     MCU_PT100B1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 3 */
    {"MCU_PT100C",      MCU_PT100C_port,      MCU_PT100C_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 4 */
    {"MCU_PT100C1",     MCU_PT100C1_port,     MCU_PT100C1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 5 */
    {"MCU_PT100D",      MCU_PT100D_port,      MCU_PT100D_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 6 */
    {"MCU_PT100D1",     MCU_PT100D1_port,     MCU_PT100D1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 7 */
    
    {"MCU_PT100E",      MCU_PT100E_port,      MCU_PT100E_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 8 */
    {"MCU_PT100E1",     MCU_PT100E1_port,     MCU_PT100E1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 9 */
    {"MCU_PT100F",      MCU_PT100F_port,      MCU_PT100F_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 10 */
    {"MCU_PT100F1",     MCU_PT100F1_port,     MCU_PT100F1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 11 */
    {"MCU_PT100G",      MCU_PT100G_port,      MCU_PT100G_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 12 */
    {"MCU_PT100G1",     MCU_PT100G1_port,     MCU_PT100G1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 13 */
    {"MCU_PT100H",      MCU_PT100H_port,      MCU_PT100H_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 14 */
    {"MCU_PT100H1",     MCU_PT100H1_port,     MCU_PT100H1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 15 */
    
    {"MCU_PT100I",      MCU_PT100I_port,      MCU_PT100I_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 16 */
    {"MCU_PT100I1",     MCU_PT100I1_port,     MCU_PT100I1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 17 */
    {"MCU_PT100J",      MCU_PT100J_port,      MCU_PT100J_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 18 */
    {"MCU_PT100J1",     MCU_PT100J1_port,     MCU_PT100J1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 19 */
    {"MCU_PT100K",      MCU_PT100K_port,      MCU_PT100K_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 20 */
    {"MCU_PT100K1",     MCU_PT100K1_port,     MCU_PT100K1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 21 */
    {"MCU_PT100L",      MCU_PT100L_port,      MCU_PT100L_pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 22 */
    {"MCU_PT100L1",     MCU_PT100L1_port,     MCU_PT100L1_pin,      GPIO_M_OUT | GPIO_S_LOW}, /* 23 */

    {"MCU_L_A",         MCU_L_A_port,         MCU_L_A_pin,          GPIO_M_OUT | GPIO_S_LOW}, /* 24 */
    {"MCU_L_B",         MCU_L_B_port,         MCU_L_B_pin,          GPIO_M_OUT | GPIO_S_LOW}, /* 25 */
    {"MCU_L_C",         MCU_L_C_port,         MCU_L_C_pin,          GPIO_M_OUT | GPIO_S_LOW}, /* 26 */
    {"MCU_L_D",         MCU_L_D_port,         MCU_L_D_pin,          GPIO_M_OUT | GPIO_S_LOW}, /* 27 */
    {"MCU_L_E",         MCU_L_E_port,         MCU_L_E_pin,          GPIO_M_OUT | GPIO_S_LOW}, /* 28 */
    {"MCU_L_F",         MCU_L_F_port,         MCU_L_F_pin,          GPIO_M_OUT | GPIO_S_LOW}, /* 29 */
    {"MCU_L_G",         MCU_L_G_port,         MCU_L_G_pin,          GPIO_M_OUT | GPIO_S_LOW}, /* 30 */
    {"MCU_L_H",         MCU_L_H_port,         MCU_L_H_pin,          GPIO_M_OUT | GPIO_S_LOW}, /* 31 */

    {"MCU_PD_A",        MCU_PD_A_port,        MCU_PD_A_pin,         GPIO_M_OUT | GPIO_S_LOW}, /* 32 */
    {"MCU_PD_A1",       MCU_PD_A1_port,       MCU_PD_A1_pin,        GPIO_M_OUT | GPIO_S_LOW}, /* 33 */
    {"MCU_PD_B",        MCU_PD_B_port,        MCU_PD_B_pin,         GPIO_M_OUT | GPIO_S_LOW}, /* 34 */
    {"MCU_PD_B1",       MCU_PD_B1_port,       MCU_PD_B1_pin,        GPIO_M_OUT | GPIO_S_LOW}, /* 35 */
    {"MCU_PD_C",        MCU_PD_C_port,        MCU_PD_C_pin,         GPIO_M_OUT | GPIO_S_LOW}, /* 36 */
    {"MCU_PD_C1",       MCU_PD_C1_port,       MCU_PD_C1_pin,        GPIO_M_OUT | GPIO_S_LOW}, /* 37 */
    {"MCU_PD_D",        MCU_PD_D_port,        MCU_PD_D_pin,         GPIO_M_OUT | GPIO_S_LOW}, /* 38 */
    {"MCU_PD_D1",       MCU_PD_D1_port,       MCU_PD_D1_pin,        GPIO_M_OUT | GPIO_S_LOW}, /* 39 */
#endif
    
#ifdef MCU_SENS /* 控制板 */

    {"Run_Led",         RUN_LED_GPIO_Port,      RUN_LED_Pin,       GPIO_M_OUT | GPIO_S_LOW}, /* 0 */
    {"Alarm_Led",       ALARM_LED_GPIO_Port,    ALARM_LED_Pin,     GPIO_M_OUT | GPIO_S_LOW}, /* 1 */
    {"Power_Led",       POWER_LED_GPIO_Port,    POWER_LED_Pin,     GPIO_M_OUT | GPIO_S_LOW}, /* 2 */
                                                                                            
    {"Rise_Time_Ctrl1", RISE_TIME_CTRL1_Port,   RISE_TIME_CTRL1,   GPIO_M_OUT | GPIO_S_LOW}, /* 3 */
    {"Rise_Time_Ctrl2", RISE_TIME_CTRL2_Port,   RISE_TIME_CTRL2,   GPIO_M_OUT | GPIO_S_LOW}, /* 4 */
    {"Rise_Time_Ctrl3", RISE_TIME_CTRL3_Port,   RISE_TIME_CTRL3,   GPIO_M_OUT | GPIO_S_LOW}, /* 5 */
    {"Rise_Time_Ctrl4", RISE_TIME_CTRL4_Port,   RISE_TIME_CTRL4,   GPIO_M_OUT | GPIO_S_LOW}, /* 6 */
    {"Power_15_P",      Power_15V_P_Port,       Power_15V_P_Pin,   GPIO_M_OUT | GPIO_S_LOW}, /* 7 */
    {"Power_15_N",      Power_15V_N_Port,       Power_15V_N_Pin,   GPIO_M_OUT | GPIO_S_LOW}, /* 8 */

    {"I_ReadBack20",    I_ReadBack20_Port,      I_ReadBack20_pin,  GPIO_M_OUT | GPIO_S_LOW}, /* 9 */
    {"I_ReadBack60",    I_ReadBack60_Port,      I_ReadBack20_pin,  GPIO_M_OUT | GPIO_S_LOW}, /* 10 */
    {"I_ReadBack600",   I_ReadBack600_Port,     I_ReadBack20_pin,  GPIO_M_OUT | GPIO_S_LOW}, /* 11 */
    {"R_ReadBack20",    R_ReadBack20_Port,      R_ReadBack20_pin,  GPIO_M_OUT | GPIO_S_LOW}, /* 12 */
    {"R_ReadBack60",    R_ReadBack60_Port,      R_ReadBack60_pin,  GPIO_M_OUT | GPIO_S_LOW}, /* 13 */
    {"R_ReadBack600",   R_ReadBack600_Port,     R_ReadBack600_pin, GPIO_M_OUT | GPIO_S_LOW}, /* 14 */
                                                                                            
    {"MCU_VoVin",       McuVoVin_Port,          McuVoVin_Pin,      GPIO_M_IN  | GPIO_S_HIGH}, /* 15 */
    {"MCU_VoVout",      McuVoVout_Port,         McuVoVout_Pin,     GPIO_M_IN  | GPIO_S_HIGH}, /* 16 */
    {"MCU_VoIin",       McuVoIin_Port,          McuVoIin_Pin,      GPIO_M_IN  | GPIO_S_HIGH}, /* 17 */
    {"MCU_VoIout",      McuVoIout_Port,         McuVoIout_Pin,     GPIO_M_IN  | GPIO_S_HIGH}, /* 18 */

    {"MCU_SLot_Check",  McuSlotStatus_Port,     McuSlotStatus_Pin, GPIO_M_IN  | GPIO_S_HIGH}, /* 23 */

#endif
};

static uint32_t sGpioSize = sizeof(sLDBI_Gpio) / sizeof(sLDBI_Gpio[0]);
void RS485_Send_Data(u8 *buf,u8 len);
/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
void GpioInit(void)
{
    for(int i=0; i<sGpioSize; i++)
    {
        GPIO_PIN_INIT(sLDBI_Gpio[i].port, sLDBI_Gpio[i].PinNum, sLDBI_Gpio[i].Conf);
        /* high/low */
        GPIO_PIN_SETBIT(sLDBI_Gpio[i].port, sLDBI_Gpio[i].PinNum);
    }
    /* 默认低电平 */
//    GPIO_PIN_SETBIT(sLDBI_Gpio[12].port, sLDBI_Gpio[12].PinNum);
//    GPIO_PIN_SETBIT(sLDBI_Gpio[13].port, sLDBI_Gpio[13].PinNum);
//    GPIO_PIN_SETBIT(sLDBI_Gpio[14].port, sLDBI_Gpio[14].PinNum);
//    RS485_Init(115200);
    
}

void DrvsGpioCtrl(uint32_t num, bool isH)
{
    if(isH)
    {
        GPIO_PIN_SETBIT(sLDBI_Gpio[num].port, sLDBI_Gpio[num].PinNum);
    }
    else
    {
        GPIO_PIN_CLRBIT(sLDBI_Gpio[num].port, sLDBI_Gpio[num].PinNum);
    }
}

void RunLED_Init(void)
{
#ifdef SENS_LED_Port
    GPIO_PIN_INIT  (SENS_LED_Port, SENS_LED_Pin, GPIO_M_OUT | GPIO_S_LOW);
    GPIO_PIN_SETBIT(SENS_LED_Port, SENS_LED_Pin);    // Output Level High
#ifdef MCU_SENS
    GPIO_PIN_INIT  (RUN_LED_GPIO_Port, RUN_LED_Pin, GPIO_M_OUT | GPIO_S_LOW);
    GPIO_PIN_SETBIT(RUN_LED_GPIO_Port, RUN_LED_Pin);    // Output Level High
#endif
#endif
}
void RunLED_ON(void)
{
#ifdef SENS_LED_Port
    GPIO_PIN_SETBIT(SENS_LED_Port, SENS_LED_Pin);    // Output Level High
#ifdef MCU_SENS
    GPIO_PIN_SETBIT(RUN_LED_GPIO_Port, RUN_LED_Pin);    // Output Level High
#endif
#endif
}
void RunLED_OFF(void)
{
#ifdef SENS_LED_Port
    GPIO_PIN_CLRBIT(SENS_LED_Port, SENS_LED_Pin);    // Output Level Low
#ifdef MCU_SENS
    GPIO_PIN_CLRBIT(RUN_LED_GPIO_Port, RUN_LED_Pin);    // Output Level High
#endif
#endif
}

void LinkLED_Init(void)
{
#ifdef POWER_LED_GPIO_Port
    GPIO_PIN_INIT(POWER_LED_GPIO_Port, POWER_LED_Pin, GPIO_M_OUT | GPIO_S_LOW);
#endif
}
void LinkLED_ON(void)
{
#ifdef POWER_LED_GPIO_Port
    GPIO_PIN_SETBIT(POWER_LED_GPIO_Port, POWER_LED_Pin);  // Output Level High
#endif
}
void LinkLED_OFF(void)
{
#ifdef POWER_LED_GPIO_Port
    GPIO_PIN_CLRBIT(POWER_LED_GPIO_Port, POWER_LED_Pin);  // Output Level Low
#endif
}

void AlarmLED_Init(void)
{
#ifdef ALARM_LED_GPIO_Port
    GPIO_PIN_INIT(ALARM_LED_GPIO_Port, ALARM_LED_Pin, GPIO_M_OUT | GPIO_S_LOW);
#endif
}
void AlarmLED_ON(void)
{
#ifdef ALARM_LED_GPIO_Port
    GPIO_PIN_SETBIT(ALARM_LED_GPIO_Port, ALARM_LED_Pin);  // Output Level High
#endif
}
void AlarmLED_OFF(void)
{
#ifdef ALARM_LED_GPIO_Port
    GPIO_PIN_CLRBIT(ALARM_LED_GPIO_Port, ALARM_LED_Pin);  // Output Level Low
#endif
}

void RS485_DirInit(void)
{
    /**
     * Note: Initialize on MX_GPIO_Init()
     */

#ifdef RS485_RE_GPIO_Port
    GPIO_PIN_CLRBIT(RS485_RE_GPIO_Port, RS485_RE_Pin);  // Output Level Low
    GPIO_PIN_INIT  (RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_M_OUT | GPIO_S_MEDIUM);
    GPIO_PIN_CLRBIT(RS485_RE_GPIO_Port, RS485_RE_Pin);  // Output Level Low
#endif
}


/**
 * RS485 switch to receive
 */
void RS485_RxEN(void)
{
#ifdef RS485_RE_GPIO_Port
    GPIO_PIN_CLRBIT(RS485_RE_GPIO_Port, RS485_RE_Pin);  // Output Level Low
#endif
}


/**
 * RS485 switch to send
 */
void RS485_TxEN(void)
{
#ifdef RS485_RE_GPIO_Port
    GPIO_PIN_SETBIT(RS485_RE_GPIO_Port, RS485_RE_Pin);  // Output Level High
#endif
}

void RS485_2_DirInit(void)
{
    /**
     * Note: Initialize on MX_GPIO_Init()
     */

#ifdef RS485_RE_GPIO_Port
    GPIO_PIN_CLRBIT(GPIOG, GPIO_PIN_15);  // Output Level Low
    GPIO_PIN_INIT  (GPIOG, GPIO_PIN_15, GPIO_M_OUT | GPIO_S_MEDIUM);
    GPIO_PIN_CLRBIT(GPIOG, GPIO_PIN_15);  // Output Level Low
#endif
}


/**
 * RS485 switch to receive
 */
void RS485_2_RxEN(void)
{
#ifdef RS485_RE_GPIO_Port
    GPIO_PIN_CLRBIT(GPIOG, GPIO_PIN_15);  // Output Level Low
#endif
}


/**
 * RS485 switch to send
 */
void RS485_2_TxEN(void)
{
#ifdef RS485_RE_GPIO_Port
    GPIO_PIN_SETBIT(GPIOG, GPIO_PIN_15);  // Output Level High
#endif
}

/**
 * Get RS485 address
 */
uint8_t RS485_GetAddr(void)
{   //lint --e{572} --e{734} --e{835} --e{845}
    return( 0 );
}

void InputInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_ClockCtrl(GPIOC, 1);
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_13 | GPIO_PIN_9 |GPIO_PIN_14 |GPIO_PIN_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/******************  fsmc 模块  *******************/
#if  1

void Fsmc_init(void)
{
    SET_BIT(RCC->AHB3ENR,  RCC_AHB3ENR_FSMCEN);

    GPIO_PIN_INIT(FSMC_A0_Port   , FSMC_A0_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A1_Port   , FSMC_A1_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A2_Port   , FSMC_A2_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A3_Port   , FSMC_A3_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A4_Port   , FSMC_A4_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A5_Port   , FSMC_A5_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A6_Port   , FSMC_A6_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A7_Port   , FSMC_A7_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A8_Port   , FSMC_A8_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A9_Port   , FSMC_A9_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A10_Port  , FSMC_A10_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A11_Port  , FSMC_A11_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A12_Port  , FSMC_A12_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A13_Port  , FSMC_A13_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A14_Port  , FSMC_A14_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A15_Port  , FSMC_A15_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A16_Port  , FSMC_A16_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A17_Port  , FSMC_A17_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_A18_Port  , FSMC_A18_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D0_Port   , FSMC_D0_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D1_Port   , FSMC_D1_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D2_Port   , FSMC_D2_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D3_Port   , FSMC_D3_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D4_Port   , FSMC_D4_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D5_Port   , FSMC_D5_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D6_Port   , FSMC_D6_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D7_Port   , FSMC_D7_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D8_Port   , FSMC_D8_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D9_Port   , FSMC_D9_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D10_Port  , FSMC_D10_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D11_Port  , FSMC_D11_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D12_Port  , FSMC_D12_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D13_Port  , FSMC_D13_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D14_Port  , FSMC_D14_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_D15_Port  , FSMC_D15_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_NOE_Port  , FSMC_NOE_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
    GPIO_PIN_INIT(FSMC_NWE_Port  , FSMC_NWE_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
//  GPIO_PIN_INIT(FSMC_NE_Port   , FSMC_NE_Pin   , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);

#ifdef            FSMC_CLK_Port
//  GPIO_PIN_INIT(FSMC_CLK_Port  , FSMC_CLK_Pin  , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
#endif
#ifdef            FSMC_NWAIT_Port
//  GPIO_PIN_INIT(FSMC_NWAIT_Port, FSMC_NWAIT_Pin, GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
#endif
#ifdef            FSMC_NBL0_Port
    GPIO_PIN_INIT(FSMC_NBL0_Port , FSMC_NBL0_Pin , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
#endif
#ifdef            FSMC_NBL1_Port
    GPIO_PIN_INIT(FSMC_NBL1_Port , FSMC_NBL1_Pin , GPIO_M_AF | GPIO_AFn(0xC) | GPIO_P_PU | GPIO_S_HIGH);
#endif

    MODIFY_REG(FSMC_Bank1->BTCR[0+0],  FSMC_BCR1_CBURSTRW
                                  /* | FSMC_BCR1_CPSIZE */
                                     | FSMC_BCR1_ASYNCWAIT
                                     | FSMC_BCR1_EXTMOD
                                     | FSMC_BCR1_WAITEN
                                     | FSMC_BCR1_WREN
                                     | FSMC_BCR1_WAITCFG
                                     | FSMC_BCR1_WRAPMOD
                                     | FSMC_BCR1_WAITPOL
                                     | FSMC_BCR1_BURSTEN
                                     | FSMC_BCR1_FACCEN
                                     | FSMC_BCR1_MWID
                                     | FSMC_BCR1_MTYP
                                     | FSMC_BCR1_MUXEN
                                     | FSMC_BCR1_MBKEN
                                    ,
                                       FSMC_BCR1_EXTMOD         // 读写使用不同时序
                                     | FSMC_BCR1_WREN           // 允许写操作
                                     |(1u << 4)                 // 数据总线宽 16bit
                                     |(0u << 2)                 // 存储器类型 SRAM 
                                     | 0  // FSMC_BCR1_MUXEN    // 地址/数据非复用
              );
    MODIFY_REG(FSMC_Bank1->BTCR[0+1],  FSMC_BTR1_ACCMOD
                                     | FSMC_BTR1_DATLAT
                                     | FSMC_BTR1_CLKDIV
                                     | FSMC_BTR1_BUSTURN
                                     | FSMC_BTR1_DATAST
                                     | FSMC_BTR1_ADDHLD
                                     | FSMC_BTR1_ADDSET
                                    ,
                                      (0u  << 28)               // 访问模式 A                  (Max 3u)
                                     |(0u  << 24)               // 同步突发 NOR Flash 数据延迟 (Max 15u)
                                     |(1u  << 20)               // CLK 信号的时钟分频比        (Max 15u)
                                     |(1u  << 16)               // 总线周转阶段的持续时间      (Max 15u)
                                     |(40u <<  8)               // 数据保持时间                (Max 255u)
                                     |(1u  <<  4)               // 地址保持时间                (Max 15u)
                                     |(12u <<  0)               // 地址设置时间                (Max 15u)
              );
    MODIFY_REG(FSMC_Bank1E->BWTR[0] ,  FSMC_BWTR1_ACCMOD
                                  /* | FSMC_BWTR1_DATLAT */
                                  /* | FSMC_BWTR1_CLKDIV */
                                     | FSMC_BWTR1_BUSTURN
                                     | FSMC_BWTR1_DATAST
                                     | FSMC_BWTR1_ADDHLD
                                     | FSMC_BWTR1_ADDSET
                                    ,
                                      (0u  << 28)               // 访问模式 A                  (Max 3u)
                                  /* |(0u  << 24) */            // 同步突发 NOR Flash 数据延迟 (Max 15u)
                                  /* |(1u  << 20) */            // CLK 信号的时钟分频比        (Max 15u)
                                     |(1u  << 16)               // 总线周转阶段的持续时间      (Max 15u)
                                     |(40u <<  8)               // 数据保持时间                (Max 255u)
                                     |(1u  <<  4)               // 地址保持时间                (Max 15u)
                                     |(12u <<  0)               // 地址设置时间                (Max 15u)
              );
    SET_BIT   (FSMC_Bank1->BTCR[0+0],  FSMC_BCR1_MBKEN);        // 存储区域使能
    Board_DelayUS(50 * 1000);                                   // 延时等待FPGA配置ok

#if 0
    {   volatile uint32_t *ptr = (uint32_t *)0x60008000;
        uint32_t           m   = 0x55AAAA55;
        uint32_t           n   = 0xAA5555AA;
        uint32_t           m1, n1;
        uint32_t           prev;
        static uint32_t    cnt = 0;
        static uint32_t    sum = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        for(;; m++, n++)
        {
            prev = DWT->CYCCNT;
            ptr[0] = m;
            ptr[1] = n;
            m1 = ptr[0];
            n1 = ptr[1];
            sum += DWT->CYCCNT - prev;
            cnt += 1;
            
            if((m1 != m) || (n1 != n)) {
                while(1);
            }
        }
    }
#endif
}
#endif


/**
 * Ethernet PHY chip Reset
 */
void ETH_PHY_Reset(void)
{
#if defined RMII_CLK_50M_GPIO_Port
    GPIO_PIN_INIT(RMII_CLK_50M_GPIO_Port, RMII_CLK_50M_Pin, GPIO_M_AF | GPIO_S_FAST);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2 | RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2_0 | 0x00);

    CLEAR_BIT(RCC->CR, RCC_CR_PLLI2SON);        // Disable the PLLI2S
    while((RCC->CR & RCC_CR_PLLI2SRDY) != 0) ;  // Wait till PLLI2S is disabled
    RCC->PLLI2SCFGR = (200 << 6) | (4 << 28);
    SET_BIT(RCC->CR, RCC_CR_PLLI2SON);          // Enable the PLLI2S
    while((RCC->CR & RCC_CR_PLLI2SRDY) == 0) ;  // Wait till PLLI2S is ready
    Board_DelayUS(50 * 1000);
#elif defined RMII_CLK_25M_GPIO_Port
    GPIO_PIN_INIT(RMII_CLK_25M_GPIO_Port, RMII_CLK_25M_Pin, GPIO_M_AF | GPIO_S_FAST);
    if (RMII_CLK_25M_GPIO_Port == GPIOA) {
        MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO1 | RCC_CFGR_MCO1PRE, RCC_CFGR_MCO1_1 | 0x00);
    } else {
        MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2 | RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2_1 | 0x00);
    }
    Board_DelayUS(50 * 1000);
#endif

#ifdef RMII_RESET_GPIO_Port
    GPIO_PIN_INIT(RMII_RESET_GPIO_Port, RMII_RESET_Pin, GPIO_M_OUT | GPIO_S_LOW);

    GPIO_PIN_CLRBIT(RMII_RESET_GPIO_Port, RMII_RESET_Pin);
    Board_DelayUS(2 * 1000);
    GPIO_PIN_SETBIT(RMII_RESET_GPIO_Port, RMII_RESET_Pin);
    Board_DelayUS(1 * 1000);
#endif
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

