/*******************************************************************************
 * @file    Board_Device.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2018/11/30
 * @brief   Defines of board level device
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2018/11/30 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __BOARD_DEVICE_H__
#define __BOARD_DEVICE_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/

#ifdef   APP_CFG
#include APP_CFG                // Apps configuration
#endif
#include <stdint.h>             // uint32_t

#ifdef   ONLY_USED_REGS         // Only used register defines
# undef  USE_HAL_DRIVER
# undef  USE_STDPERIPH_DRIVER
#endif// ONLY_USED_REGS

#ifdef   _RTE_                  // CMSIS_device_header
#include "RTE_Components.h"     // Component selection
#endif// _RTE_
#ifdef   CMSIS_device_header
#include CMSIS_device_header    // CMSIS Device header and Libraries header files
#else                           // eg: "stm32f4xx.h" and "stm32f4xx_hal.h"
#error "CMSIS_device_header: must defined to device header file name, eg: stm32f4xx.h"
#endif

#if !defined(STM32F1) && defined(__STM32F10X_STDPERIPH_VERSION)
#    define  STM32F1            // STM32 Family
#endif
#if !defined(STM32F2) && defined(__STM32F2XX_STDPERIPH_VERSION)
#    define  STM32F2
#endif
#if !defined(STM32F4) && defined(__STM32F4XX_STDPERIPH_VERSION)
#    define  STM32F4
#endif

#ifndef  ONLY_USED_REGS
#include "main.h"               // Defined of GPIO PIN
#endif


/*******************************************************************************
 * @addtogroup Board
 * @{
 * @addtogroup              Exported_Macros
 * @{
 ******************************************************************************/

#if defined(STM32F1)
#   ifndef ROM_MAIN_ADDR
#   define ROM_MAIN_ADDR                    0x08000000
#   endif
#   ifndef ROM_MAIN_SIZE
#   define ROM_MAIN_SIZE  (((const uint16_t *)0x1FFFF7E0)[0] * (uint32_t)1024)
#   endif
#   ifndef RAM_MAIN_ADDR
#   define RAM_MAIN_ADDR                    0x20000000
#   endif
//# ifndef RAM_MAIN_SIZE
//# define RAM_MAIN_SIZE                         0
//# endif

#elif defined(STM32F2)
#   ifndef ROM_MAIN_ADDR
#   define ROM_MAIN_ADDR                    0x08000000
#   endif
#   ifndef ROM_MAIN_SIZE
#   define ROM_MAIN_SIZE  (((const uint16_t *)0x1FFF7A22)[0] * (uint32_t)1024)
#   endif
#   ifndef RAM_MAIN_ADDR
#   define RAM_MAIN_ADDR                    0x20000000
#   endif
//# ifndef RAM_MAIN_SIZE
//# define RAM_MAIN_SIZE                         0
//# endif

#elif defined(STM32F4)
#   ifndef ROM_MAIN_ADDR
#   define ROM_MAIN_ADDR                    0x08000000
#   endif
#   ifndef ROM_MAIN_SIZE
#   define ROM_MAIN_SIZE  (((const uint16_t *)0x1FFF7A22)[0] * (uint32_t)1024)
#   endif
#   ifndef RAM_MAIN_ADDR
#   define RAM_MAIN_ADDR                    0x20000000
#   endif
//# ifndef RAM_MAIN_SIZE
//# define RAM_MAIN_SIZE                         0
//# endif

#else
#error "Unsupported stm32 family."
#endif


/*----------------------------------------------------------------------------*/
//lint -$
//lint -emacro(10, LOAD_REGION_START)
//lint -emacro(10, LOAD_REGION_LIMIT)
//lint -emacro(40, LOAD_REGION_START)
//lint -emacro(40, LOAD_REGION_LIMIT)
#ifndef _lint
#if defined(__CC_ARM) || defined(__ARMCC_VERSION) //--- ARM Compiler -----------

#   define ROM_LINK_START       ((uintptr_t)&IROM_REGION_START(IROM_REGION_NAME))
#   define ROM_LINK_LIMIT       ((uintptr_t)&IROM_REGION_LIMIT(IROM_REGION_NAME))

#   define RAM_LINK_START       ((uintptr_t)&IRAM_REGION_START(IRAM_REGION_NAME))
#   define RAM_LINK_LIMIT       ((uintptr_t)&IRAM_REGION_LIMIT(IRAM_REGION_NAME))

#   ifndef IROM_REGION_NAME
#   define IROM_REGION_NAME   LR_IROM1  // IROM region name, default: LR_IROM1
#   endif
#   ifndef IRAM_REGION_NAME
#   define IRAM_REGION_NAME   RW_IRAM1  // IROM region name, default: RW_IRAM1
#   endif
#   define  LOAD_REGION_START(_name_)   Load$$LR$$##_name_##$$Base
#   define  LOAD_REGION_LIMIT(_name_)   Load$$LR$$##_name_##$$Limit
#   define IMAGE_REGION_START(_name_)   Image$$##_name_##$$RW$$Base
#   define IMAGE_REGION_LIMIT(_name_)   Image$$##_name_##$$ZI$$Limit
#   define  IROM_REGION_START(_name_)    LOAD_REGION_START(_name_)
#   define  IROM_REGION_LIMIT(_name_)    LOAD_REGION_LIMIT(_name_)
#   define  IRAM_REGION_START(_name_)   IMAGE_REGION_START(_name_)
#   define  IRAM_REGION_LIMIT(_name_)   IMAGE_REGION_LIMIT(_name_)
    extern const int            IROM_REGION_START(IROM_REGION_NAME);
    extern const int            IROM_REGION_LIMIT(IROM_REGION_NAME);
    extern const int            IRAM_REGION_START(IRAM_REGION_NAME);
    extern const int            IRAM_REGION_LIMIT(IRAM_REGION_NAME);

#elif defined(__ICCARM__)   //------------------------- IAR Compiler -----------
#error "not supported compiler"

#   define ROM_LINK_START      ((uint32_t)__section_begin(IROM_REGION_NAME))
#   define ROM_LINK_LIMIT      ((uint32_t)__section_end(  IROM_REGION_NAME))

#   ifndef IROM_REGION_NAME
#   define IROM_REGION_NAME   ".text"   // IROM region name, default: ".text"
#   endif
#   pragma section=IROM_REGION_NAME

#elif defined(__GNUC__)     //------------------------- GNU Compiler -----------
#error "not supported compiler"

#   define ROM_LINK_START      ((uint32_t)&IROM_REGION_START)
#   define ROM_LINK_LIMIT      ((uint32_t)&IROM_REGION_LIMIT)

#   ifndef IROM_REGION_START
#   define IROM_REGION_START   _stext   // IROM region start addr, default: _stext
#   endif
#   ifndef IROM_REGION_LIMIT
#   define IROM_REGION_LIMIT   _etext   // IROM region limit addr, default: _etext
#   endif
    extern const int           IROM_REGION_START;
    extern const int           IROM_REGION_LIMIT;

#else                       //------------------------- Unknow Compiler --------
#error "not supported compiler"
#endif
#endif//_lint


/*******************************************************************************
 * @}
 * @}
 * @addtogroup Board_Device
 * @{
 ******************************************************************************/

#ifndef GPIO_PIN_0
#define GPIO_PIN_0      ((uint32_t)0x00000001)  //!< Pin 0  selected
#define GPIO_PIN_1      ((uint32_t)0x00000002)  //!< Pin 1  selected
#define GPIO_PIN_2      ((uint32_t)0x00000004)  //!< Pin 2  selected
#define GPIO_PIN_3      ((uint32_t)0x00000008)  //!< Pin 3  selected
#define GPIO_PIN_4      ((uint32_t)0x00000010)  //!< Pin 4  selected
#define GPIO_PIN_5      ((uint32_t)0x00000020)  //!< Pin 5  selected
#define GPIO_PIN_6      ((uint32_t)0x00000040)  //!< Pin 6  selected
#define GPIO_PIN_7      ((uint32_t)0x00000080)  //!< Pin 7  selected
#define GPIO_PIN_8      ((uint32_t)0x00000100)  //!< Pin 8  selected
#define GPIO_PIN_9      ((uint32_t)0x00000200)  //!< Pin 9  selected
#define GPIO_PIN_10     ((uint32_t)0x00000400)  //!< Pin 10 selected
#define GPIO_PIN_11     ((uint32_t)0x00000800)  //!< Pin 11 selected
#define GPIO_PIN_12     ((uint32_t)0x00001000)  //!< Pin 12 selected
#define GPIO_PIN_13     ((uint32_t)0x00002000)  //!< Pin 13 selected
#define GPIO_PIN_14     ((uint32_t)0x00004000)  //!< Pin 14 selected
#define GPIO_PIN_15     ((uint32_t)0x00008000)  //!< Pin 15 selected
#define GPIO_PIN_All    ((uint32_t)0x0000FFFF)  //!< All pins selected

#define EXTI_LINE_16    ((uint32_t)0x00010000)  //!< Output of PVD
#define EXTI_LINE_17    ((uint32_t)0x00020000)  //!< Event  of RTC Alarm
#define EXTI_LINE_18    ((uint32_t)0x00040000)  //!< Event  of USB FS Wakeup
#define EXTI_LINE_19    ((uint32_t)0x00080000)  //!< Event  of Ethernet Wakeup
#define EXTI_LINE_20    ((uint32_t)0x00100000)  //!< Event  of USB HS Wakeup
#define EXTI_LINE_21    ((uint32_t)0x00200000)  //!< Event  of RTC Tamper and TimeStamp
#define EXTI_LINE_22    ((uint32_t)0x00400000)  //!< Event  of RTC Wakeup
#define EXTI_LINE_23    ((uint32_t)0x00400000)  //!< Event  of LPTIM Wakeup
#endif

#ifdef _lint
#undef  POSITION_VAL
#define POSITION_VAL(VAL)   (((VAL) + 1) % 32)
#endif
#ifndef POSITION_VAL
#define POSITION_VAL(VAL)   (__CLZ(__RBIT(VAL)))
#endif

//lint -emacro(835, SET_BIT, CLEAR_BIT, READ_BIT, CLEAR_REG)
//lint -emacro(835, WRITE_REG, READ_REG, MODIFY_REG)
//lint -emacro(845, SET_BIT, CLEAR_BIT, READ_BIT, CLEAR_REG)
//lint -emacro(845, WRITE_REG, READ_REG, MODIFY_REG)
//lint -emacro(778, POSITION_VAL)


/*----------------------------------------------------------------------------*/
//lint -ecall(835, GPIO_PinInit,   GPIO_PortInit, GPIO_ClockCtrl, GPIO_ClockStat)
//lint -ecall(845, GPIO_PinInit,   GPIO_PortInit, GPIO_ClockCtrl, GPIO_ClockStat)
//lint -emacro(835, GPIO_PIN_CLRBIT, GPIO_PIN_SETBIT, GPIO_PIN_WRITE, GPIO_PIN_READ)
//lint -emacro(845, GPIO_PIN_CLRBIT, GPIO_PIN_SETBIT, GPIO_PIN_WRITE, GPIO_PIN_READ)
//lint -emacro(835, GPIO_PORT_WRITE, GPIO_PORT_READ)
//lint -emacro(845, GPIO_PORT_WRITE, GPIO_PORT_READ)
//lint -emacro(522, GPIO_PIN_READ, GPIO_PORT_READ)
/**
 * Configuration value for Port Pin
 */
#define GPIO_MODE_IT_MSK           0x8000u  //---------- NO USED ----------
#define GPIO_MODE_EV_MSK           0x4000u  //---------- NO USED ----------
#define GPIO_MODE_RT_MSK           0x2000u  //---------- NO USED ----------
#define GPIO_MODE_FT_MSK           0x1000u  //---------- NO USED ----------
#define GPIO_MODE_MSK              0x0003u  //---------- NO USED ----------
#define GPIO_M_IN                  0x0000u      //!< 通用输入
#define GPIO_M_OUT                 0x0001u      //!< 通用输出
#define GPIO_M_AF                  0x0002u      //!< 复用功能
#define GPIO_M_AN                  0x0003u      //!< 模拟功能
#define GPIO_M_IT_RT               0xA000u      //!< 外部中断 -- 上升沿
#define GPIO_M_IT_FT               0x9000u      //!< 外部中断 -- 下降沿
#define GPIO_M_IT_RFT              0xB000u      //!< 外部中断 -- 上升与下降沿
#define GPIO_M_EV_RT               0x6000u      //!< 外部事件 -- 上升沿
#define GPIO_M_EV_FT               0x5000u      //!< 外部事件 -- 下降沿
#define GPIO_M_EV_RFT              0x7000u      //!< 外部事件 -- 上升与下降沿
#define GPIO_PULL_MSK              0x000Cu  //---------- NO USED ----------
#define GPIO_P_PU                  0x0004u      //!< 上拉
#define GPIO_P_PD                  0x0008u      //!< 下拉
#define GPIO_SPEED_MSK             0x0030u  //---------- NO USED ----------
#define GPIO_S_LOW                 0x0000u      //!< 输出速度: 低 (2MHz)
#define GPIO_S_MEDIUM              0x0010u      //!< 输出速度: 中 (10MHz)
#define GPIO_S_FAST                0x0020u      //!< 输出速度: 快 (50MHz)
#define GPIO_S_HIGH                0x0030u      //!< 输出速度: 高 (100MHz)
#define GPIO_O_OD                  0x0040u      //!< 开漏输出
#define GPIO_AFn_MSK               0x0F00u  //---------- NO USED ----------
#define GPIO_AFn(x)    (((x)<<8) & 0x0F00u)     //!< 复用功能选择


/**
 * Set the callback function for EXTI interrupt handle, and Enable or Disable Interrupt.
 *
 * @param[in]  LineNum    EXTI line number.
 * @param[in]  Callback   Callback function, (!= NULL) Enable Interrupt, (== NULL) Disable.
 */
void EXTI_LineCtrl(unsigned LineNum, void(*Callback)(uint32_t LineStatus));

/**
 * Set the Interrupt Priority for EXTI Line.
 *
 * @param[in]  LineNum   EXTI line number.
 * @param[in]  Prio      Interrupt Priority.
 */
void EXTI_LinePrio(unsigned LineNum, unsigned Prio);

/**
 * Initialize one selected Port Pin
 *
 * @param[in]  GPIOx    Where x can be (A..K) to select the GPIO peripheral
 * @param[in]  PinNum   Specifies the Pin where can be (0..15)
 * @param[in]  Conf     Configuration value for Port Pin
 */
void GPIO_PinInit(void *GPIOx, unsigned PinNum, unsigned Conf);

/**
 * Initialize one selected Port Pin
 *
 * @param[in]  GPIOx    Where x can be (A..K) to select the GPIO peripheral
 * @param[in]  PinMsk   Port Pin, can be GPIO_PIN_x where x can be (0..15)
 * @param[in]  Conf     Configuration value for Port Pin
 */
#define GPIO_PIN_INIT(GPIOx, PinMsk, Conf)  GPIO_PinInit((GPIOx), POSITION_VAL(PinMsk), (Conf))

/**
 * Set the callback function for GPIO EXTI interrupt, and Enable or Disable Interrupt.
 *
 * @param[in]  GPIOx      Where x can be (A..K) to select the GPIO peripheral.
 * @param[in]  PinMsk     Port Pin, can be GPIO_PIN_x where x can be (0..15).
 * @param[in]  Callback   Callback function, (!= NULL) Enable Interrupt, (== NULL) Disable.
 */
#define GPIO_PIN_INTR_CTRL(GPIOx, PinMsk, Callback)  EXTI_LineCtrl(POSITION_VAL(PinMsk), (Callback))

/**
 * Set the Interrupt Priority for GPIO EXTI interrupt.
 *
 * @param[in]  GPIOx      Where x can be (A..K) to select the GPIO peripheral.
 * @param[in]  PinMsk     Port Pin, can be GPIO_PIN_x where x can be (0..15).
 * @param[in]  Prio       Interrupt Priority.
 */
#define GPIO_PIN_INTR_PRIO(GPIOx, PinMsk, Prio)      EXTI_LinePrio(POSITION_VAL(PinMsk), (Prio))

/**
 * @def        GPIO_PIN_CLRBIT(GPIOx, PinMsk)
 * @brief      Set the selected port output Pins
 * @param[in]  GPIOx   Where x can be (A..K) to select the GPIO peripheral
 * @param[in]  PinMsk  Port Pins, can be any combination of GPIO_PIN_x where x can be (0..15)
 */
/**
 * @def        GPIO_PIN_SETBIT(GPIOx, PinMsk)
 * @brief      Clear the selected port output Pins
 * @param[in]  GPIOx   Where x can be (A..K) to select the GPIO peripheral
 * @param[in]  PinMsk  Port Pins, can be any combination of GPIO_PIN_x where x can be (0..15)
 */
#if defined(__STM32F2XX_STDPERIPH_VERSION) || defined(__STM32F4XX_STDPERIPH_VERSION)
#define GPIO_PIN_CLRBIT(GPIOx, PinMsk)  ((GPIOx)->BSRRH =  ((uint32_t)(PinMsk))        )
#define GPIO_PIN_SETBIT(GPIOx, PinMsk)  ((GPIOx)->BSRRL =  ((uint32_t)(PinMsk))        )
#else
#define GPIO_PIN_CLRBIT(GPIOx, PinMsk)  ((GPIOx)->BSRR  = (((uint32_t)(PinMsk)) << 16u))
#define GPIO_PIN_SETBIT(GPIOx, PinMsk)  ((GPIOx)->BSRR  =  ((uint32_t)(PinMsk))        )
#endif

/**
 * @brief      Write the selected port output Pins
 * @param[in]  GPIOx   Where x can be (A..K) to select the GPIO peripheral
 * @param[in]  PinMsk  Port Pins, can be any combination of GPIO_PIN_x where x can be (0..15)
 * @param[in]  val     Value of Port Pins to Set
 */
#define GPIO_PIN_WRITE(GPIOx, PinMsk, val)  (  GPIO_PIN_CLRBIT(GPIOx, (PinMsk) & ~(val)) \
                                             , GPIO_PIN_SETBIT(GPIOx, (PinMsk) &  (val)) \
                                            )

/**
 * @brief      Read the selected port input Pins
 * @param[in]  GPIOx   Where x can be (A..K) to select the GPIO peripheral
 * @param[in]  PinMsk  Port Pins, can be any combination of GPIO_PIN_x where x can be (0..15)
 * @return     The input port Pins value.
 */
#define GPIO_PIN_READ(GPIOx, PinMsk)    ((GPIOx)->IDR & (uint32_t)(PinMsk))

/**
 * Initialize specified Port
 *
 * @param[in]  GPIOx    Where x can be (A..K) to select the GPIO peripheral
 * @param[in]  Conf     Configuration value for Port
 */
void GPIO_PortInit(void *GPIOx, unsigned Conf);
#define GPIO_PORT_INIT(GPIOx, Conf)  GPIO_PortInit((GPIOx), (Conf))

/**
 * @brief      Write the specified port output data
 * @param[in]  GPIOx   Where x can be (A..K) to select the GPIO peripheral
 * @param[in]  val     The write data
 */
#define GPIO_PORT_WRITE(GPIOx, val)  ((GPIOx)->ODR = (uint16_t)(val))

/**
 * @brief      Read the specified port input data
 * @param[in]  GPIOx   Where x can be (A..K) to select the GPIO peripheral
 * @return     The read data
 */
#define GPIO_PORT_READ(GPIOx)   ((uint16_t)((GPIOx)->IDR))

/**
 * GPIO Port Clock Control
 * @param[in]  GPIOx   Where x can be (A..K) to select the GPIO peripheral
 * @param[in]  enable   Enable(1) or disable(0) clock
 */
void GPIO_ClockCtrl(const void *GPIOx, int enable);

/**
 * Get GPIO port clock state
 * @param[in]  GPIOx   Where x can be (A..K) to select the GPIO peripheral
 * @return  Enabled(1) or Disabled(0)
 */
int GPIO_ClockStat(const void *GPIOx);


/*****************************  END OF FILE  ***********************************
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __BOARD_DEVICE_H__
/*****************************  END OF FILE  **********************************/

