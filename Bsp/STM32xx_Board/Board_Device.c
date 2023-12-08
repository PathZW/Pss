/*******************************************************************************
 * @file    Board_Device.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/4/26
 * @brief   Defines of board level device
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/4/26 -- Linghu -- the first version
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
#include "board.h"              // Device's defines

#include <stdint.h>
#include "defines.h"            // Used: __WEAK
#include "emb_delay.h"

#ifndef  CMSIS_device_header
#warning "Undefined Macro: CMSIS_device_header"
#endif


#if _lint
#undef  __NOP
#define __NOP()
#endif
/*******************************************************************************
 * @addtogroup Board_Device
 * @{
 * @addtogroup              Private_Prototypes
 * @{
 ******************************************************************************/
//lint -esym(522, SystemClock_Config, SystemExMem_Config)
//lint -esym(522, SystemBoard_Config, SystemStart_Config)

__WEAK void SystemClock_Config(void){}
__WEAK void SystemExMem_Config(void){}
__WEAK void SystemBoard_Config(void){}
__WEAK void SystemStart_Config(void){}


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/

void Board_BareInit(void)
{
    extern uint32_t  __VECTOR_TABLE;

    SystemCoreClockUpdate();
    Board_DelayInit();

    // Todo 1: CMSIS SystemInit()
    SystemInit();
    SCB->VTOR = (uint32_t)(&__VECTOR_TABLE);    // Config Vector Table
    SystemCoreClockUpdate();
    Board_DelayInit();

    // Todo 2: SystemClock_Config()
    SystemClock_Config();
    SystemCoreClockUpdate();
    Board_DelayInit();

    // Todo 3: SystemExMem_Config()
    SystemExMem_Config();
    SystemCoreClockUpdate();
    Board_DelayInit();
}


void Board_Init(void)
{
    LOG_INFO("Board Core Clock: %lu Hz", (uint32_t)SystemCoreClock);
    LOG_INFO("       HSE Clock: %lu Hz", (uint32_t)HSE_VALUE);  //lint !e40
    LOG_INFO("       HSI Closk: %lu Hz", (uint32_t)HSI_VALUE);  //lint !e40
    DelayUS(2);

    // Todo 4: SystemBoard_Config()
    LOG_INFO("Loading SystemBoard_Config()");
    DelayUS(2);
    SystemBoard_Config();
}


void Board_Start(void)
{
    // Todo 5: SystemStart_Config
    LOG_INFO("Loading SystemStart_Config()");
    DelayMS(2);
    SystemStart_Config();
}


void Board_Reset(void)
{
    __set_FAULTMASK(1); // Disable all fault and interrupt
    NVIC_SystemReset(); // System Reset
}


/*----------------------------------------------------------------------------*/
//lint -esym(522, Board_DelayInit)

void Board_DelayInit(void)
{
}


uint32_t Board_GetMsec(void)
{
    static uint32_t _CountMS = 0U;

    Board_DelayUS(1000);
    return( ++_CountMS );
}


void Board_DelayUS(uint16_t uSec)
{
    uint32_t  cnt;
    uint32_t  clock = SystemCoreClock;

    if (uSec < 0x400) {
        cnt  = 1 + (((clock >>  8) * (uSec >> 0)) >> 16); //lint !e835
    } else {
        cnt  = 1 + (((clock >> 12) * (uSec >> 2)) >> 10);
    }
    for(; cnt != 1;)
    {
        __NOP();     __NOP();
        __NOP();     __NOP();
        __NOP();     __NOP();
        cnt -= 1;    __NOP();
        __NOP();     __NOP();
        __NOP();     __NOP();
        __NOP();     __NOP();
    }
}


#if 0
#pragma push
#pragma O0
__WEAK void Board_DelayUS1(uint16_t uSec)
{
    uint32_t  cnt;
    uint32_t  clock = SystemCoreClock;

    if (uSec < 0x400) {
        cnt  = 1 + (((clock >>  8) * (uSec >> 0)) >> 16);
    } else {
        cnt  = 1 + (((clock >> 12) * (uSec >> 2)) >> 10);
    }
    for(; cnt != 1;)
    {
        __NOP();     __NOP();
        __NOP();     __NOP();
        __NOP();     __NOP();
        cnt -= 1;    __NOP();
        __NOP();     __NOP();
        __NOP();     __NOP();
        __NOP();     __NOP();
    }
}
#pragma O2
__WEAK void Board_DelayUS2(uint16_t uSec)
{
    uint32_t  cnt;
    uint32_t  clock = SystemCoreClock;

    if (uSec < 0x400) {
        cnt  = 1 + (((clock >>  8) * (uSec >> 0)) >> 16);
    } else {
        cnt  = 1 + (((clock >> 12) * (uSec >> 2)) >> 10);
    }
    for(; cnt != 1;)
    {
        __NOP();     __NOP();
        __NOP();     __NOP();
        __NOP();     __NOP();
        cnt -= 1;    __NOP();
        __NOP();     __NOP();
        __NOP();     __NOP();
        __NOP();     __NOP();
    }
}
#pragma pop

__WEAK void Delay_Test(void)
{
    DWT->CYCCNT = 0;  Board_DelayUS1(1);
    DWT->CYCCNT = 0;  Board_DelayUS2(1);

    DWT->CYCCNT = 0;  Board_DelayUS1(2);
    DWT->CYCCNT = 0;  Board_DelayUS2(2);

    DWT->CYCCNT = 0;  Board_DelayUS1(4);
    DWT->CYCCNT = 0;  Board_DelayUS2(4);

    DWT->CYCCNT = 0;  Board_DelayUS1(8);
    DWT->CYCCNT = 0;  Board_DelayUS2(8);

    DWT->CYCCNT = 0;  Board_DelayUS1(16);
    DWT->CYCCNT = 0;  Board_DelayUS2(16);

    DWT->CYCCNT = 0;  Board_DelayUS1(32);
    DWT->CYCCNT = 0;  Board_DelayUS2(32);

    DWT->CYCCNT = 0;  Board_DelayUS1(64);
    DWT->CYCCNT = 0;  Board_DelayUS2(64);

    DWT->CYCCNT = 0;  Board_DelayUS1(100);
    DWT->CYCCNT = 0;  Board_DelayUS2(100);

    DWT->CYCCNT = 0;  Board_DelayUS1(1000);
    DWT->CYCCNT = 0;  Board_DelayUS2(1000);

    DWT->CYCCNT = 0;  Board_DelayUS1(1000 * 10);
    DWT->CYCCNT = 0;  Board_DelayUS2(1000 * 10);

    DWT->CYCCNT = 0;  Board_DelayUS1(1000 * 60);
    DWT->CYCCNT = 0;  Board_DelayUS2(1000 * 60);
}
#endif


/*----------------------------------------------------------------------------*/
#ifdef USE_HAL_DRIVER

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{   // 注意: 可能 时钟 与 C运行库 都未初始化
    ((void)(TickPriority));
    Board_DelayInit();
    return HAL_OK;
}

void HAL_IncTick(void)
{
    for(;;) ;
}

void HAL_SuspendTick(void)
{
    for(;;) ;
}

void HAL_ResumeTick(void)
{
    for(;;) ;
}

uint32_t HAL_GetTick(void)
{
    return( GetSysMsec() );
}

void HAL_Delay(uint32_t Delay)
{
    DelayMS(Delay);
}
#endif // USE_HAL_DRIVER


/*----------------------------------------------------------------------------*/
//lint -emacro((778), EXTI_LINE_READ)
//lint -emacro((845), EXTI_LINE_READ)
/**
 * Read the selected EXTI line input Pin (GPIO Pin)
 * @param[in]  LineNum  EXTI line can be 0 ~ 15
 * @return     The EXTI line input Pin (GPIO Pin) value
 */
#define EXTI_LINE_READ(LineNum)     GPIO_PIN_READ(     \
    _GPIO_Index[0xFu & (   SYSCFG->EXTICR[(LineNum)/4] \
                        >> (((LineNum)%4)*4)           \
                       )                               \
               ]                                       \
  , 1u << (LineNum)                                    \
)

/**
 * The callback function for EXTI interrupt handle
 * @param[in]  LineStatus    EXTI line status for Low or High
 */
static void(*_EXTI_cbFun[16])(uint32_t LineStatus) = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
  , NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
//, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

/**
 * EXTI Interrupt Number
 */
static const IRQn_Type _EXTI_IRQn[16] = {
    EXTI0_IRQn    , EXTI1_IRQn    , EXTI2_IRQn    , EXTI3_IRQn
  , EXTI4_IRQn    , EXTI9_5_IRQn  , EXTI9_5_IRQn  , EXTI9_5_IRQn
  , EXTI9_5_IRQn  , EXTI9_5_IRQn  , EXTI15_10_IRQn, EXTI15_10_IRQn
  , EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn
};

/**
 * Index of the GPIO Port
 */
static GPIO_TypeDef * const _GPIO_Index[] = {
            GPIOA   //lint !e835
    #ifdef  GPIOB
          , GPIOB
    #endif
    #ifdef  GPIOC
          , GPIOC
    #endif
    #ifdef  GPIOD
          , GPIOD
    #endif
    #ifdef  GPIOE
          , GPIOE
    #endif
    #ifdef  GPIOF
          , GPIOF
    #endif
    #ifdef  GPIOG
          , GPIOG
    #endif
    #ifdef  GPIOH
          , GPIOH
    #endif
    #ifdef  GPIOI
          , GPIOI
    #endif
    #ifdef  GPIOJ
          , GPIOJ
    #endif
    #ifdef  GPIOK
          , GPIOK
    #endif
};


#if 0
void PVD_IRQHandler(void)
{
    if (_EXTI_cbFun[16] != NULL) {
        _EXTI_cbFun[16]( 0 );
    }
    WRITE_REG(EXTI->PR, 1u << 16);
}

void RTC_Alarm_IRQHandler(void)
{
    if (_EXTI_cbFun[17] != NULL) {
        _EXTI_cbFun[17]( 0 );
    }
    WRITE_REG(EXTI->PR, 1u << 17);
}

void OTG_FS_WKUP_IRQHandler(void)
{
    if (_EXTI_cbFun[18] != NULL) {
        _EXTI_cbFun[18]( 0 );
    }
    WRITE_REG(EXTI->PR, 1u << 18);
}

void ETH_WKUP_IRQHandler(void)
{
    if (_EXTI_cbFun[19] != NULL) {
        _EXTI_cbFun[19]( 0 );
    }
    WRITE_REG(EXTI->PR, 1u << 19);
}

void OTG_HS_WKUP_IRQHandler(void)
{
    if (_EXTI_cbFun[20] != NULL) {
        _EXTI_cbFun[20]( 0 );
    }
    WRITE_REG(EXTI->PR, 1u << 20);
}

void TAMP_STAMP_IRQHandler(void)
{
    if (_EXTI_cbFun[21] != NULL) {
        _EXTI_cbFun[21]( 0 );
    }
    WRITE_REG(EXTI->PR, 1u << 21);
}

void RTC_WKUP_IRQHandler(void)
{
    if (_EXTI_cbFun[22] != NULL) {
        _EXTI_cbFun[22]( 0 );
    }
    WRITE_REG(EXTI->PR, 1u << 22);
}
#endif

#if defined(STM32F1) || defined(STM32F2) || defined(STM32F4)
void EXTI0_IRQHandler(void)
{
    if (_EXTI_cbFun[0] != NULL) {
        _EXTI_cbFun[0]( EXTI_LINE_READ(0) );
    }
    WRITE_REG(EXTI->PR, GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
    if (_EXTI_cbFun[1] != NULL) {
        _EXTI_cbFun[1]( EXTI_LINE_READ(1) );
    }
    WRITE_REG(EXTI->PR, GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
    if (_EXTI_cbFun[2] != NULL) {
        _EXTI_cbFun[2]( EXTI_LINE_READ(2) );
    }
    WRITE_REG(EXTI->PR, GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
    if (_EXTI_cbFun[3] != NULL) {
        _EXTI_cbFun[3]( EXTI_LINE_READ(3) );
    }
    WRITE_REG(EXTI->PR, GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
    if (_EXTI_cbFun[4] != NULL) {
        _EXTI_cbFun[4]( EXTI_LINE_READ(4) );
    }
    WRITE_REG(EXTI->PR, GPIO_PIN_4);
}

void EXTI9_5_IRQHandler(void)
{
    if (READ_BIT(EXTI->PR, 1u << 5))
    {
        if (_EXTI_cbFun[5] != NULL) {
            _EXTI_cbFun[5]( EXTI_LINE_READ(5) );
        }
    }
    if (READ_BIT(EXTI->PR, 1u << 6))
    {
        if (_EXTI_cbFun[6] != NULL) {
            _EXTI_cbFun[6]( EXTI_LINE_READ(6) );
        }
    }
    if (READ_BIT(EXTI->PR, 1u << 7))
    {
        if (_EXTI_cbFun[7] != NULL) {
            _EXTI_cbFun[7]( EXTI_LINE_READ(7) );
        }
    }
    if (READ_BIT(EXTI->PR, 1u << 8))
    {
        if (_EXTI_cbFun[8] != NULL) {
            _EXTI_cbFun[8]( EXTI_LINE_READ(8) );
        }
    }
    if (READ_BIT(EXTI->PR, 1u << 9))
    {
        if (_EXTI_cbFun[9] != NULL) {
            _EXTI_cbFun[9]( EXTI_LINE_READ(9) );
        }
    }
    WRITE_REG(EXTI->PR, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9);
}

void EXTI15_10_IRQHandler(void)
{
    if (READ_BIT(EXTI->PR, 1u << 10))
    {
        if (_EXTI_cbFun[10] != NULL) {
            _EXTI_cbFun[10]( EXTI_LINE_READ(10) );
        }
    }
    if (READ_BIT(EXTI->PR, 1u << 11))
    {
        if (_EXTI_cbFun[11] != NULL) {
            _EXTI_cbFun[11]( EXTI_LINE_READ(11) );
        }
    }
    if (READ_BIT(EXTI->PR, 1u << 12))
    {
        if (_EXTI_cbFun[12] != NULL) {
            _EXTI_cbFun[12]( EXTI_LINE_READ(12) );
        }
    }
    if (READ_BIT(EXTI->PR, 1u << 13))
    {
        if (_EXTI_cbFun[13] != NULL) {
            _EXTI_cbFun[13]( EXTI_LINE_READ(13) );
        }
    }
    if (READ_BIT(EXTI->PR, 1u << 14))
    {
        if (_EXTI_cbFun[14] != NULL) {
            _EXTI_cbFun[14]( EXTI_LINE_READ(14) );
        }
    }
    if (READ_BIT(EXTI->PR, 1u << 15))
    {
        if (_EXTI_cbFun[15] != NULL) {
            _EXTI_cbFun[15]( EXTI_LINE_READ(15) );
        }
    }
    WRITE_REG(EXTI->PR, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
}
#endif


/**
 * Set the callback function for EXTI interrupt handle, and Enable or Disable Interrupt
 */
void EXTI_LineCtrl(unsigned LineNum, void(*Callback)(uint32_t LineStatus))
{
    unsigned    cnt;
    unsigned    num;
    IRQn_Type   IRQn;

    if (LineNum >= ARRAY_SIZE(_EXTI_cbFun)) {
        return;
    }
    _EXTI_cbFun[LineNum] = Callback;

    for(IRQn = _EXTI_IRQn[LineNum], cnt = num = 0;  num < ARRAY_SIZE(_EXTI_IRQn);  num++)
    {   // 使用 LineNum 对应 IRQn 的中断回调函数, 非 NULL 数量
        if((IRQn == _EXTI_IRQn[num]) && (_EXTI_cbFun[num] != NULL)) {
            cnt++;
        }
    }
    if (cnt == 0) {     // 不存在有效中断回调函数, 禁止中断
        NVIC_DisableIRQ(IRQn);
    } else {            //   存在有效中断回调函数, 允许中断
        NVIC_EnableIRQ (IRQn);
    }
}


/**
 * Set the Interrupt Priority for EXTI Line
 */
void EXTI_LinePrio(unsigned LineNum, unsigned Prio)
{
    if (LineNum >= ARRAY_SIZE(_EXTI_IRQn)) {
        return;
    }
    NVIC_SetPriority(_EXTI_IRQn[LineNum], Prio);
}


/**
 * Initialize one selected Port Pin
 */
void GPIO_PinInit(void *GPIOx, unsigned PinNum, unsigned Conf)
{
    uint32_t       tmp;
    GPIO_TypeDef  *GPIOx1 = GPIOx;
    GPIO_ClockCtrl(GPIOx1, 1);  // Enable GPIO Peripheral Clock

#if defined(STM32F1)
#error "Unsupported stm32 family."

#elif defined(STM32F2) || defined(STM32F4)
    if( PinNum < 16 )
    {
        // GPIO 模式配置
        switch(Conf & GPIO_MODE_MSK) {
        default         :
        case GPIO_M_IN  :   tmp = 0x00;  break;
        case GPIO_M_OUT :   tmp = 0x01;  break;
        case GPIO_M_AN  :   tmp = 0x03;  break;
        case GPIO_M_AF  : //tmp = 0x02;  break;
            MODIFY_REG(GPIOx1->AFR[PinNum / 8], (0xFu              ) << ((PinNum % 8) * 4)
                                              , (0xFu & (Conf >> 8)) << ((PinNum % 8) * 4)
                      );
            /*-----------*/ tmp = 0x02;  break;
        }
        MODIFY_REG(GPIOx1->MODER, 3u << (PinNum * 2), tmp << (PinNum * 2));

        // GPIO 输出速度配置
        switch(Conf & GPIO_SPEED_MSK) {
        default            :
        case GPIO_S_LOW    :  tmp = 0x00;  break;
        case GPIO_S_MEDIUM :  tmp = 0x01;  break;
        case GPIO_S_FAST   :  tmp = 0x02;  break;
        case GPIO_S_HIGH   :  tmp = 0x03;  break;
        }
        MODIFY_REG(GPIOx1->OSPEEDR, 3u << (PinNum * 2), tmp << (PinNum * 2));

        // GPIO 输出类型配置
        tmp = ((Conf & GPIO_O_OD) ? 0x01u : 0x00u);
        MODIFY_REG(GPIOx1->OTYPER, 1u << (PinNum * 1), tmp << (PinNum * 1));

        // GPIO 上下拉配置
        switch(Conf & GPIO_PULL_MSK) {
        default        :  tmp = 0x00;  break;
        case GPIO_P_PU :  tmp = 0x01;  break;
        case GPIO_P_PD :  tmp = 0x02;  break;
        }
        MODIFY_REG(GPIOx1->PUPDR, 3u << (PinNum * 2), tmp << (PinNum * 2));

        // GPIO EXTI 配置
        if((Conf & (GPIO_MODE_IT_MSK | GPIO_MODE_EV_MSK)) != 0)
        {
            SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
//          tmp = GPIO_IndexGet(GPIOx1);
            for(tmp = 0;  tmp < ARRAY_SIZE(_GPIO_Index);  tmp++) {
                if(GPIOx1 == _GPIO_Index[tmp])  break;
            }
            MODIFY_REG(SYSCFG->EXTICR[PinNum / 4], 0xFu << ((PinNum % 4) * 4)
                                                 , tmp  << ((PinNum % 4) * 4)
                      );
        }
    }
    if( PinNum < 23 )
    {
        // GPIO EXTI 配置
        if((Conf & (GPIO_MODE_IT_MSK | GPIO_MODE_EV_MSK)) != 0)
        {
            MODIFY_REG(EXTI->IMR ,                              1u       << PinNum
                                 , ((Conf & GPIO_MODE_IT_MSK) ? 1u : 0u) << PinNum
                      );
            MODIFY_REG(EXTI->EMR ,                              1u       << PinNum
                                 , ((Conf & GPIO_MODE_EV_MSK) ? 1u : 0u) << PinNum
                      );
            MODIFY_REG(EXTI->RTSR,                              1u       << PinNum
                                 , ((Conf & GPIO_MODE_RT_MSK) ? 1u : 0u) << PinNum
                      );
            MODIFY_REG(EXTI->FTSR,                              1u       << PinNum
                                 , ((Conf & GPIO_MODE_FT_MSK) ? 1u : 0u) << PinNum
                      );
            CLEAR_BIT (EXTI->PR,                                1u       << PinNum);
        }
    }
#else   // STM32F1
#error "Unsupported stm32 family."
#endif  // STM32F1
}


/**
 * Initialize specified Port
 */
void GPIO_PortInit(void *GPIOx, unsigned Conf)
{
    uint32_t       tmp;
    GPIO_TypeDef  *GPIOx1 = GPIOx;
    GPIO_ClockCtrl(GPIOx1, 1);  // Enable GPIO Peripheral Clock

#if defined(STM32F1)
#error "Unsupported stm32 family."

#elif defined(STM32F2) || defined(STM32F4)
    switch(Conf & GPIO_MODE_MSK) {
    default         :
    case GPIO_M_IN  :   tmp = 0x00000000;  break;
    case GPIO_M_OUT :   tmp = 0x55555555;  break;
    case GPIO_M_AN  :   tmp = 0xFFFFFFFF;  break;
    case GPIO_M_AF  : //tmp = 0xAAAAAAAA;  break;
        tmp =   (0xFu & (Conf >> 8));
        tmp =   (tmp << (4 * 0)) | (tmp << (4 * 1))  //lint !e835
              | (tmp << (4 * 2)) | (tmp << (4 * 3))
              | (tmp << (4 * 4)) | (tmp << (4 * 5))
              | (tmp << (4 * 6)) | (tmp << (4 * 7)); //lint !e845
        WRITE_REG(GPIOx1->AFR[0], tmp);
        WRITE_REG(GPIOx1->AFR[1], tmp);
        /*-----------*/ tmp = 0xAAAAAAAA;  break;
    }
    WRITE_REG(GPIOx1->MODER, tmp);

    switch(Conf & GPIO_SPEED_MSK) {
    default            :
    case GPIO_S_LOW    :  tmp = 0x00000000;  break;
    case GPIO_S_MEDIUM :  tmp = 0x55555555;  break;
    case GPIO_S_FAST   :  tmp = 0xAAAAAAAA;  break;
    case GPIO_S_HIGH   :  tmp = 0xFFFFFFFF;  break;
    }
    WRITE_REG(GPIOx1->OSPEEDR, tmp);

    tmp = ((Conf & GPIO_O_OD) ? 0xFFFFu : 0x0000u);
    WRITE_REG(GPIOx1->OTYPER, tmp);

    switch(Conf & GPIO_PULL_MSK) {
    default        :  tmp = 0x00000000;  break;
    case GPIO_P_PU :  tmp = 0x55555555;  break;
    case GPIO_P_PD :  tmp = 0xAAAAAAAA;  break;
    }
    WRITE_REG(GPIOx1->PUPDR, tmp);
#else   // STM32F1
#error "Unsupported stm32 family."
#endif  // STM32F1
}


/**
 * GPIO Port Clock Control
 */
void GPIO_ClockCtrl(const void *GPIOx, int enable)
{   //lint --e{717} --e{835} --e{838}
    if (GPIOA == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOA_CLK_ENABLE();  }
        else       { __GPIOA_CLK_DISABLE(); }
    #elif defined(STM32F1)
        if(enable) { RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN; }
        else       { RCC->APB2ENR &= ~RCC_APB2ENR_IOPAEN; }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOAEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOAEN; }
    #endif
        return;
    }

#ifdef  GPIOB
    if (GPIOB == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOB_CLK_ENABLE();  }
        else       { __GPIOB_CLK_DISABLE(); }
    #elif defined(STM32F1)
        if(enable) { RCC->APB2ENR |=  RCC_APB2ENR_IOPBEN; }
        else       { RCC->APB2ENR &= ~RCC_APB2ENR_IOPBEN; }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOBEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOBEN; }
    #endif
        return;
    }
#endif

#ifdef  GPIOC
    if (GPIOC == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOC_CLK_ENABLE();  }
        else       { __GPIOC_CLK_DISABLE(); }
    #elif defined(STM32F1)
        if(enable) { RCC->APB2ENR |=  RCC_APB2ENR_IOPCEN; }
        else       { RCC->APB2ENR &= ~RCC_APB2ENR_IOPCEN; }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOCEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN; }
    #endif
        return;
    }
#endif

#ifdef  GPIOD
    if (GPIOD == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOD_CLK_ENABLE();  }
        else       { __GPIOD_CLK_DISABLE(); }
    #elif defined(STM32F1)
        if(enable) { RCC->APB2ENR |=  RCC_APB2ENR_IOPDEN; }
        else       { RCC->APB2ENR &= ~RCC_APB2ENR_IOPDEN; }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIODEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIODEN; }
    #endif
        return;
    }
#endif

#ifdef  GPIOE
    if (GPIOE == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOE_CLK_ENABLE();  }
        else       { __GPIOE_CLK_DISABLE(); }
    #elif defined(STM32F1)
        if(enable) { RCC->APB2ENR |=  RCC_APB2ENR_IOPEEN; }
        else       { RCC->APB2ENR &= ~RCC_APB2ENR_IOPEEN; }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOEEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOEEN; }
    #endif
        return;
    }
#endif

#ifdef  GPIOF
    if (GPIOF == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOF_CLK_ENABLE();  }
        else       { __GPIOF_CLK_DISABLE(); }
    #elif defined(STM32F1)
        if(enable) { RCC->APB2ENR |=  RCC_APB2ENR_IOPFEN; }
        else       { RCC->APB2ENR &= ~RCC_APB2ENR_IOPFEN; }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOFEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOFEN; }
    #endif
        return;
    }
#endif

#ifdef  GPIOG
    if (GPIOG == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOG_CLK_ENABLE();  }
        else       { __GPIOG_CLK_DISABLE(); }
    #elif defined(STM32F1)
        if(enable) { RCC->APB2ENR |=  RCC_APB2ENR_IOPGEN; }
        else       { RCC->APB2ENR &= ~RCC_APB2ENR_IOPGEN; }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOGEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOGEN; }
    #endif
        return;
    }
#endif

#ifdef  GPIOH
    if (GPIOH == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOH_CLK_ENABLE();  }
        else       { __GPIOH_CLK_DISABLE(); }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOHEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOHEN; }
    #endif
        return;
    }
#endif

#ifdef  GPIOI
    if (GPIOI == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOI_CLK_ENABLE();  }
        else       { __GPIOI_CLK_DISABLE(); }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOIEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOIEN; }
    #endif
        return;
    }
#endif

#ifdef  GPIOJ
    if (GPIOJ == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOJ_CLK_ENABLE();  }
        else       { __GPIOJ_CLK_DISABLE(); }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOJEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOJEN; }
    #endif
        return;
    }
#endif

#ifdef  GPIOK
    if (GPIOK == GPIOx) {
    #ifdef USE_HAL_DRIVER
        if(enable) { __GPIOK_CLK_ENABLE();  }
        else       { __GPIOK_CLK_DISABLE(); }
    #elif defined(STM32F2) || defined(STM32F4)
        if(enable) { RCC->AHB1ENR |=  RCC_AHB1ENR_GPIOKEN; }
        else       { RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOKEN; }
    #endif
        return;
    }
#endif
}


/**
 * Get GPIO Port clock state
 */
int GPIO_ClockStat(const void *GPIOx)
{   //lint --e{641} --e{835}
    if (GPIOA == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOA_IS_CLK_ENABLED() );
    #elif defined(STM32F1)
        return( (RCC->APB2ENR & RCC_APB2ENR_IOPAEN) != 0U );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN) != 0U );
    #else
        return( 0 );
    #endif
    }

#ifdef  GPIOB
    if (GPIOB == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOB_IS_CLK_ENABLED() );
    #elif defined(STM32F1)
        return( (RCC->APB2ENR & RCC_APB2ENR_IOPBEN) != 0U );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOBEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

#ifdef  GPIOC
    if (GPIOC == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOC_IS_CLK_ENABLED() );
    #elif defined(STM32F1)
        return( (RCC->APB2ENR & RCC_APB2ENR_IOPCEN) != 0U );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOCEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

#ifdef  GPIOD
    if (GPIOD == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOD_IS_CLK_ENABLED() );
    #elif defined(STM32F1)
        return( (RCC->APB2ENR & RCC_APB2ENR_IOPDEN) != 0U );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIODEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

#ifdef  GPIOE
    if (GPIOE == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOE_IS_CLK_ENABLED() );
    #elif defined(STM32F1)
        return( (RCC->APB2ENR & RCC_APB2ENR_IOPEEN) != 0U );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOEEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

#ifdef  GPIOF
    if (GPIOF == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOF_IS_CLK_ENABLED() );
    #elif defined(STM32F1)
        return( (RCC->APB2ENR & RCC_APB2ENR_IOPFEN) != 0U );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOFEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

#ifdef  GPIOG
    if (GPIOG == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOG_IS_CLK_ENABLED() );
    #elif defined(STM32F1)
        return( (RCC->APB2ENR & RCC_APB2ENR_IOPGEN) != 0U );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOGEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

#ifdef  GPIOH
    if (GPIOH == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOH_IS_CLK_ENABLED() );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOHEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

#ifdef  GPIOI
    if (GPIOI == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOI_IS_CLK_ENABLED() );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOIEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

#ifdef  GPIOJ
    if (GPIOJ == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOJ_IS_CLK_ENABLED() );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOJEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

#ifdef  GPIOK
    if (GPIOK == GPIOx) {
    #ifdef USE_HAL_DRIVER
        return( __GPIOK_IS_CLK_ENABLED() );
    #elif defined(STM32F2) || defined(STM32F4)
        return( (RCC->AHB1ENR & RCC_AHB1ENR_GPIOKEN) != 0U );
    #else
        return( 0 );
    #endif
    }
#endif

    return( 0 );
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

