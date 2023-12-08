/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */
/* Private defines -----------------------------------------------------------*/
/******************  rs485  ************************************/
#if 1
#define RS485_TX_Pin            GPIO_PIN_10
#define RS485_TX_GPIO_Port      GPIOC

#define RS485_RX_Pin            GPIO_PIN_11
#define RS485_RX_GPIO_Port      GPIOC

#define RS485_RE_Pin            GPIO_PIN_12
#define RS485_RE_GPIO_Port      GPIOC
#endif

/******************  FSMC  **************************************/
#if 1
#define FSMC_A0_Pin             GPIO_PIN_0
#define FSMC_A0_Port            GPIOF
#define FSMC_A1_Pin             GPIO_PIN_1
#define FSMC_A1_Port            GPIOF
#define FSMC_A2_Pin             GPIO_PIN_2
#define FSMC_A2_Port            GPIOF
#define FSMC_A3_Pin             GPIO_PIN_3
#define FSMC_A3_Port            GPIOF
#define FSMC_A4_Pin             GPIO_PIN_4
#define FSMC_A4_Port            GPIOF
#define FSMC_A5_Pin             GPIO_PIN_5
#define FSMC_A5_Port            GPIOF
#define FSMC_A6_Pin             GPIO_PIN_12
#define FSMC_A6_Port            GPIOF
#define FSMC_A7_Pin             GPIO_PIN_13
#define FSMC_A7_Port            GPIOF
#define FSMC_A8_Pin             GPIO_PIN_14
#define FSMC_A8_Port            GPIOF
#define FSMC_A9_Pin             GPIO_PIN_15
#define FSMC_A9_Port            GPIOF
#define FSMC_A10_Pin            GPIO_PIN_0
#define FSMC_A10_Port           GPIOG
#define FSMC_A11_Pin            GPIO_PIN_1
#define FSMC_A11_Port           GPIOG
#define FSMC_A12_Pin            GPIO_PIN_2
#define FSMC_A12_Port           GPIOG
#define FSMC_A13_Pin            GPIO_PIN_3
#define FSMC_A13_Port           GPIOG
#define FSMC_A14_Pin            GPIO_PIN_4
#define FSMC_A14_Port           GPIOG
#define FSMC_A15_Pin            GPIO_PIN_5
#define FSMC_A15_Port           GPIOG
#define FSMC_A16_Pin            GPIO_PIN_11
#define FSMC_A16_Port           GPIOD
#define FSMC_A17_Pin            GPIO_PIN_12
#define FSMC_A17_Port           GPIOD
#define FSMC_A18_Pin            GPIO_PIN_13
#define FSMC_A18_Port           GPIOD

#define FSMC_D0_Pin             GPIO_PIN_14
#define FSMC_D0_Port            GPIOD
#define FSMC_D1_Pin             GPIO_PIN_15
#define FSMC_D1_Port            GPIOD
#define FSMC_D2_Pin             GPIO_PIN_0
#define FSMC_D2_Port            GPIOD
#define FSMC_D3_Pin             GPIO_PIN_1
#define FSMC_D3_Port            GPIOD
#define FSMC_D4_Pin             GPIO_PIN_7
#define FSMC_D4_Port            GPIOE
#define FSMC_D5_Pin             GPIO_PIN_8
#define FSMC_D5_Port            GPIOE
#define FSMC_D6_Pin             GPIO_PIN_9
#define FSMC_D6_Port            GPIOE
#define FSMC_D7_Pin             GPIO_PIN_10
#define FSMC_D7_Port            GPIOE
#define FSMC_D8_Pin             GPIO_PIN_11
#define FSMC_D8_Port            GPIOE
#define FSMC_D9_Pin             GPIO_PIN_12
#define FSMC_D9_Port            GPIOE
#define FSMC_D10_Pin            GPIO_PIN_13
#define FSMC_D10_Port           GPIOE
#define FSMC_D11_Pin            GPIO_PIN_14
#define FSMC_D11_Port           GPIOE
#define FSMC_D12_Pin            GPIO_PIN_15
#define FSMC_D12_Port           GPIOE
#define FSMC_D13_Pin            GPIO_PIN_8
#define FSMC_D13_Port           GPIOD
#define FSMC_D14_Pin            GPIO_PIN_9
#define FSMC_D14_Port           GPIOD
#define FSMC_D15_Pin            GPIO_PIN_10
#define FSMC_D15_Port           GPIOD

#define FSMC_NOE_Pin            GPIO_PIN_4
#define FSMC_NOE_Port           GPIOD
#define FSMC_NWE_Pin            GPIO_PIN_5
#define FSMC_NWE_Port           GPIOD
#define FSMC_NE_Pin             GPIO_PIN_7
#define FSMC_NE_Port            GPIOD
#define FSMC_CLK_Pin            GPIO_PIN_3
#define FSMC_CLK_Port           GPIOD
#define FSMC_NWAIT_Pin          GPIO_PIN_6
#define FSMC_NWAIT_Port         GPIOD

#define FSMC_NBL0_Pin          GPIO_PIN_0
#define FSMC_NBL0_Port         GPIOE
#define FSMC_NBL1_Pin          GPIO_PIN_1
#define FSMC_NBL1_Port         GPIOE

#define FSMC_INT1_Pin           GPIO_PIN_0
#define FSMC_INT1_Port          GPIOA
#define FSMC_INT1_SOURCE_Port   0
#define FSMC_INT1_IRQ_Num       EXTI0_IRQn
#define FSMC_INT1_IRQ_Fun       EXTI0_IRQHandler
#define FSMC_INT1_IRQ_Prio      12u

#define FSMC_INT2_Pin           GPIO_PIN_1
#define FSMC_INT2_Port          GPIOA
#define FSMC_INT2_SOURCE_Port   0
#define FSMC_INT2_IRQ_Num       EXTI1_IRQn
#define FSMC_INT2_IRQ_Fun       EXTI1_IRQHandler
#define FSMC_INT2_IRQ_Prio      12u

#define FSMC_INT3_Pin           GPIO_PIN_2
#define FSMC_INT3_Port          GPIOA
#define FSMC_INT3_SOURCE_Port   0
#define FSMC_INT3_IRQ_Num       EXTI2_IRQn
#define FSMC_INT3_IRQ_Fun       EXTI2_IRQHandler
#define FSMC_INT3_IRQ_Prio      12u

#define FSMC_INT4_Pin           GPIO_PIN_3
#define FSMC_INT4_Port          GPIOA
#define FSMC_INT4_SOURCE_Port   0
#define FSMC_INT4_IRQ_Num       EXTI3_IRQn
#define FSMC_INT4_IRQ_Fun       EXTI3_IRQHandler
#define FSMC_INT4_IRQ_Prio      12u
#endif

#if 1
/****************** iic2 º”√‹+SPI ***********************************/
#define IIC_SCL_Pin                   GPIO_PIN_10
#define IIC_SCL_Port                  GPIOB

#define IIC_SDA_Pin                   GPIO_PIN_11
#define IIC_SDA_Port                  GPIOB

#define SPI2_CS_Pin                   GPIO_PIN_12
#define SPI2_CS_Port                  GPIOB

#define SPI2_CLK_Pin                  GPIO_PIN_13
#define SPI2_CLK_Port                 GPIOB

#define SPI2_MISO_Pin                 GPIO_PIN_14
#define SPI2_MISO_Port                GPIOB

#define SPI2_MOSI_Pin                 GPIO_PIN_15
#define SPI2_MOSI_Port                GPIOB

#endif

#ifdef MCU_SENS  /* øÿ÷∆∞Â */
/******************  read ID 4IO *************************/
#define READ_BRDID_BIT0_Pin      GPIO_PIN_2
#define READ_BRDID_BIT0_Port     GPIOE
                                 
#define READ_BRDID_BIT1_Pin      GPIO_PIN_3
#define READ_BRDID_BIT1_Port     GPIOE
                                 
#define READ_BRDID_BIT2_Pin      GPIO_PIN_4
#define READ_BRDID_BIT2_Port     GPIOE
                                 
#define READ_BRDID_BIT3_Pin      GPIO_PIN_5
#define READ_BRDID_BIT3_Port     GPIOE

#define SENS_LED_Pin             GPIO_PIN_12
#define SENS_LED_Port            GPIOG
/******************  rise time ctrl 4IO *************************/
#define RISE_TIME_CTRL1          GPIO_PIN_6
#define RISE_TIME_CTRL1_Port     GPIOG

#define RISE_TIME_CTRL2          GPIO_PIN_7
#define RISE_TIME_CTRL2_Port     GPIOG

#define RISE_TIME_CTRL3          GPIO_PIN_8
#define RISE_TIME_CTRL3_Port     GPIOG

#define RISE_TIME_CTRL4          GPIO_PIN_10
#define RISE_TIME_CTRL4_Port     GPIOG

/******************  15V+- ***************************************/
#define Power_15V_P_Pin        GPIO_PIN_6
#define Power_15V_P_Port       GPIOA

#define Power_15V_N_Pin        GPIO_PIN_7
#define Power_15V_N_Port       GPIOA

/******************  slot check **********************************/
#define McuSlotStatus_Pin      GPIO_PIN_0
#define McuSlotStatus_Port     GPIOB

/******************* protect volt check **************************/
#define McuVoVin_Pin           GPIO_PIN_4
#define McuVoVin_Port          GPIOB
#define McuVoVout_Pin          GPIO_PIN_5
#define McuVoVout_Port         GPIOB
#define McuVoIin_Pin           GPIO_PIN_6
#define McuVoIin_Port          GPIOB
#define McuVoIout_Pin          GPIO_PIN_7
#define McuVoIout_Port         GPIOB

/******************  led status **********************************/
#define RUN_LED_Pin             GPIO_PIN_0
#define RUN_LED_GPIO_Port       GPIOC

#define ALARM_LED_Pin           GPIO_PIN_1
#define ALARM_LED_GPIO_Port     GPIOC

#define POWER_LED_Pin           GPIO_PIN_2
#define POWER_LED_GPIO_Port     GPIOC

/******************  DC/ReadBack 20+60+600 ***********************/
#define R_ReadBack20_pin         GPIO_PIN_4
#define R_ReadBack20_Port        GPIOC
#define I_ReadBack20_pin         GPIO_PIN_7
#define I_ReadBack20_Port        GPIOC

#define R_ReadBack60_pin         GPIO_PIN_5
#define R_ReadBack60_Port        GPIOC
#define I_ReadBack60_pin         GPIO_PIN_8
#define I_ReadBack60_Port        GPIOC

#define R_ReadBack600_pin        GPIO_PIN_6
#define R_ReadBack600_Port       GPIOC
#define I_ReadBack600_pin        GPIO_PIN_9
#define I_ReadBack600_Port       GPIOC

#define I_ReadBack3_pin          GPIO_PIN_14
#define I_ReadBack3_Port         GPIOC
#define I_ReadBack30_pin         GPIO_PIN_15
#define I_ReadBack30_Port        GPIOC

#endif

#ifdef MCU_SOUR

#define SENS_LED_Pin             GPIO_PIN_10
#define SENS_LED_Port       GPIOG

/******************  read ID 4IO *************************/
#define READ_BRDID_BIT0_Pin      GPIO_PIN_9
#define READ_BRDID_BIT0_Port     GPIOC

#define READ_BRDID_BIT1_Pin      GPIO_PIN_13
#define READ_BRDID_BIT1_Port     GPIOC

#define READ_BRDID_BIT2_Pin      GPIO_PIN_14
#define READ_BRDID_BIT2_Port     GPIOC

#define READ_BRDID_BIT3_Pin      GPIO_PIN_15
#define READ_BRDID_BIT3_Port     GPIOC
/*********************  PT100  **********************************/
/*  PT1 - 16 */
#define MCU_PT100A_pin         GPIO_PIN_6
#define MCU_PT100A_port        GPIOG
#define MCU_PT100A1_pin        GPIO_PIN_7
#define MCU_PT100A1_port       GPIOG
#define MCU_PT100B_pin         GPIO_PIN_8
#define MCU_PT100B_port        GPIOG
#define MCU_PT100B1_pin        GPIO_PIN_9
#define MCU_PT100B1_port       GPIOG
#define MCU_PT100C_pin         GPIO_PIN_4
#define MCU_PT100C_port        GPIOA
#define MCU_PT100C1_pin        GPIO_PIN_5
#define MCU_PT100C1_port       GPIOA
#define MCU_PT100D_pin         GPIO_PIN_6
#define MCU_PT100D_port        GPIOA
#define MCU_PT100D1_pin        GPIO_PIN_7
#define MCU_PT100D1_port       GPIOA
/*  PT2 - 16 */
#define MCU_PT100E_pin         GPIO_PIN_3
#define MCU_PT100E_port        GPIOB
#define MCU_PT100E1_pin        GPIO_PIN_4
#define MCU_PT100E1_port       GPIOB
#define MCU_PT100F_pin         GPIO_PIN_5
#define MCU_PT100F_port        GPIOB
#define MCU_PT100F1_pin        GPIO_PIN_6
#define MCU_PT100F1_port       GPIOB
#define MCU_PT100G_pin         GPIO_PIN_7
#define MCU_PT100G_port        GPIOB
#define MCU_PT100G1_pin        GPIO_PIN_8
#define MCU_PT100G1_port       GPIOB
#define MCU_PT100H_pin         GPIO_PIN_14
#define MCU_PT100H_port        GPIOG
#define MCU_PT100H1_pin        GPIO_PIN_15
#define MCU_PT100H1_port       GPIOG
/*  PT3 - 16 */
#define MCU_PT100I_pin         GPIO_PIN_0
#define MCU_PT100I_port        GPIOB
#define MCU_PT100I1_pin        GPIO_PIN_1
#define MCU_PT100I1_port       GPIOB
#define MCU_PT100J_pin         GPIO_PIN_11
#define MCU_PT100J_port        GPIOA
#define MCU_PT100J1_pin        GPIO_PIN_12
#define MCU_PT100J1_port       GPIOA
#define MCU_PT100K_pin         GPIO_PIN_7
#define MCU_PT100K_port        GPIOF
#define MCU_PT100K1_pin        GPIO_PIN_8
#define MCU_PT100K1_port       GPIOF
#define MCU_PT100L_pin         GPIO_PIN_9
#define MCU_PT100L_port        GPIOF
#define MCU_PT100L1_pin        GPIO_PIN_10
#define MCU_PT100L1_port       GPIOF

/*******************  water flow **********************************/
#define MCU_L_A_pin            GPIO_PIN_2
#define MCU_L_A_port           GPIOE
#define MCU_L_B_pin            GPIO_PIN_3
#define MCU_L_B_port           GPIOE
#define MCU_L_C_pin            GPIO_PIN_4
#define MCU_L_C_port           GPIOE
#define MCU_L_D_pin            GPIO_PIN_5
#define MCU_L_D_port           GPIOE
#define MCU_L_E_pin            GPIO_PIN_6
#define MCU_L_E_port           GPIOE
#define MCU_L_F_pin            GPIO_PIN_11
#define MCU_L_F_port           GPIOG
#define MCU_L_G_pin            GPIO_PIN_12
#define MCU_L_G_port           GPIOG
#define MCU_L_H_pin            GPIO_PIN_13
#define MCU_L_H_port           GPIOG

/*******************  PD Switch **********************************/
#define MCU_PD_A_pin           GPIO_PIN_0
#define MCU_PD_A_port          GPIOC
#define MCU_PD_A1_pin          GPIO_PIN_1
#define MCU_PD_A1_port         GPIOC
#define MCU_PD_B_pin           GPIO_PIN_2
#define MCU_PD_B_port          GPIOC
#define MCU_PD_B1_pin          GPIO_PIN_3
#define MCU_PD_B1_port         GPIOC
#define MCU_PD_C_pin           GPIO_PIN_4
#define MCU_PD_C_port          GPIOC
#define MCU_PD_C1_pin          GPIO_PIN_5
#define MCU_PD_C1_port         GPIOC
#define MCU_PD_D_pin           GPIO_PIN_6
#define MCU_PD_D_port          GPIOC
#define MCU_PD_D1_pin          GPIO_PIN_7
#define MCU_PD_D1_port         GPIOC
#endif


/********************************************/
#if 0
#define RMII_CLK_50M_Pin        GPIO_PIN_9
#define RMII_CLK_50M_GPIO_Port  GPIOC
#endif
#if 0
#define RMII_CLK_25M_Pin        GPIO_PIN_9
#define RMII_CLK_25M_GPIO_Port  GPIOC
#endif
#if 0
#define RMII_RESET_Pin          GPIO_PIN_12
#define RMII_RESET_GPIO_Port    GPIOG
#endif
#if 0
#define GPIB_ADDR1_Pin          GPIO_PIN_6
#define GPIB_ADDR1_GPIO_Port    GPIOG
#define GPIB_ADDR2_Pin          GPIO_PIN_7
#define GPIB_ADDR2_GPIO_Port    GPIOG
#define GPIB_ADDR3_Pin          GPIO_PIN_4
#define GPIB_ADDR3_GPIO_Port    GPIOA
#define GPIB_ADDR4_Pin          GPIO_PIN_5
#define GPIB_ADDR4_GPIO_Port    GPIOA
#define GPIB_ADDR5_Pin          GPIO_PIN_9
#define GPIB_ADDR5_GPIO_Port    GPIOG
#endif

#if 0
#define TNT4882_INTR_Pin            GPIO_PIN_4
#define TNT4882_INTR_GPIO_Port      GPIOG
#define TNT4882_INTR_SOURCE_Port    6u
#define TNT4882_INTR_IRQ_Num        EXTI4_IRQn
#define TNT4882_IntrFunction        EXTI4_IRQHandler

#define TNT4882_DRQ_Pin             GPIO_PIN_0
#define TNT4882_DRQ_GPIO_Port       GPIOG
#define TNT4882_RDY1_Pin            GPIO_PIN_1
#define TNT4882_RDY1_GPIO_Port      GPIOG
#define TNT4882_RDN_Pin             GPIO_PIN_2
#define TNT4882_RDN_GPIO_Port       GPIOG
#define TNT4882_WRN_Pin             GPIO_PIN_3
#define TNT4882_WRN_GPIO_Port       GPIOG
#define TNT4882_RESETN_Pin          GPIO_PIN_5
#define TNT4882_RESETN_GPIO_Port    GPIOG
#define TNT4882_BBUSN_Pin           GPIO_PIN_6
#define TNT4882_BBUSN_GPIO_Port     GPIOG
#define TNT4882_ABUSN_Pin           GPIO_PIN_7
#define TNT4882_ABUSN_GPIO_Port     GPIOG
#define TNT4882_DACKN_Pin           GPIO_PIN_8
#define TNT4882_DACKN_GPIO_Port     GPIOG
#define TNT4882_CSN_Pin             GPIO_PIN_9
#define TNT4882_CSN_GPIO_Port       GPIOG
#define TNT4882_ADDR1_Pin           GPIO_PIN_2
#define TNT4882_ADDR1_GPIO_Port     GPIOE
#define TNT4882_ADDR2_Pin           GPIO_PIN_3
#define TNT4882_ADDR2_GPIO_Port     GPIOE
#define TNT4882_ADDR3_Pin           GPIO_PIN_4
#define TNT4882_ADDR3_GPIO_Port     GPIOE
#define TNT4882_ADDR4_Pin           GPIO_PIN_5
#define TNT4882_ADDR4_GPIO_Port     GPIOE
#define TNT4882_ADDR5_Pin           GPIO_PIN_6
#define TNT4882_ADDR5_GPIO_Port     GPIOE
#define TNT4882_DATA_PORT           GPIOF
#endif
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
