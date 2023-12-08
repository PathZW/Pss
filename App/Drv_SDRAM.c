/**********************************************************************************************************/
/** @file     Drv_SDRAM.c
*** @author   Linghu
*** @version  V1.0.0
*** @date     2018/12/18
*** @brief    Driver for SDRAM
***********************************************************************************************************
*** @par Last Commit:
***      \$Author$ \n
***      \$Date$ \n
***      \$Rev$ \n
***      \$URL$ \n
***
*** @par Change Logs:
***      2018/12/18 -- Linghu -- the first version
***********************************************************************************************************/
//lint +libclass(all)
//lint ++d__RESTRICT=
//lint -e537                    "Repeated include file 'xxxx'"
//lint -e838                    " Previously assigned value to variable 'tmp' has not been used "
//lint -e845                    " The right argument to operator '|' is certain to be 0 "
//lint -e835                    " A zero has been given as left argument to operator '<<' "
//lint -e716                    " while(1) ... "

#define  ONLY_USED_REGS
#include "board.h"              // Device's defines

#include <stdint.h>
#include "Drv_SDRAM.h"


#if _lint
#undef  __NOP
#define __NOP()
#endif
/**********************************************************************************************************/
/** @addtogroup Drv_SDRAM
*** @{
*** @addtogroup                     Private_Macros
*** @{
***********************************************************************************************************/

#ifndef SDRAM_ADDR
#define SDRAM_ADDR      ( 0xC0000000 )
#endif
#ifndef SDRAM_SIZE
#define SDRAM_SIZE      ( 32 * 1024 * 1024 )
#endif

#ifndef ARRY_SIZE
#define ARRY_SIZE(arr)      ( sizeof(arr) / sizeof((arr)[0]) )
#endif

#define NBR_CONVERT(n)      ( ((uint32_t)(n) << 24) & 0xFF000000 | ((uint32_t)(n) & 0x00FFFFFF) )

#define SDRAM_R(addr, m)    ( m = *((volatile uint32_t*)addr) )

#define SDRAM_W(addr, m)    ( *((volatile uint32_t*)addr) = (uint32_t)m )


/**********************************************************************************************************/
/** @}
*** @addtogroup                     Private_Functions
*** @{
***********************************************************************************************************/
/** @brief  Initialize SDRAM controller.
***
*** @note   Called in startup_xx.s before jump to main, SDRAM will
***         be used as program data memory (including heap and stack).
***********************************************************************************************************/

void SDRAM_Init( void )
{
    uint32_t   tmp;
    uint32_t   timeout;

    __NOP();  __NOP();  __NOP();  __NOP();
    SET_BIT(RCC->AHB1ENR,   RCC_AHB1ENR_GPIOCEN           /* Enable clock: GPIOC                */
                          | RCC_AHB1ENR_GPIODEN           /*               GPIOD                */
                          | RCC_AHB1ENR_GPIOEEN           /*               GPIOE                */
                          | RCC_AHB1ENR_GPIOFEN           /*               GPIOF                */
                          | RCC_AHB1ENR_GPIOGEN           /*               GPIOG                */
                       // | RCC_AHB1ENR_GPIOHEN           /*               GPIOH                */
           );
    __NOP();  __NOP();  __NOP();  __NOP();                /* Delay after a RCC clock enabling   */

    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);          /* Enable clock: SYSCFG               */
    __NOP();  __NOP();  __NOP();  __NOP();                /* Delay after a RCC clock enabling   */
    SET_BIT(SYSCFG->CMPCR, SYSCFG_CMPCR_CMP_PD);          /* Enable Compensation Cell           */
    __NOP();  __NOP();  __NOP();  __NOP();

    MODIFY_REG(GPIOC->AFR[0],   0x0000FF0F,  0x0000CC0C); /* Configure PC: 0 2 3                */
    MODIFY_REG(GPIOC->AFR[1],   0x00000000,  0x00000000); /* --(4Bit:C)FMC Alternate function   */
    MODIFY_REG(GPIOC->MODER,    0x000000F3,  0x000000A2); /* --(2Bit:2)Alternate function mode  */
    MODIFY_REG(GPIOC->OSPEEDR,  0x000000F3,  0x000000F3); /* --(2Bit:3)Very high speed          */
//  MODIFY_REG(GPIOC->PUPDR,    0x000000F3,  0x00000051); /* --(2Bit:1)Pull-up                  */
    MODIFY_REG(GPIOC->PUPDR,    0x000000F3,  0x00000000); /* --(2Bit:0)No Pull-up/Pull-down     */
    MODIFY_REG(GPIOC->OTYPER,   0x0000000D,  0x00000000); /* --(1Bit:0)Output type push-pull    */
    tmp = READ_BIT(GPIOC->LCKR, 0x0000FFFF) |0x0000000D;  /* --Port configuration locked        */
    WRITE_REG(GPIOC->LCKR, tmp | 0x00010000);
    WRITE_REG(GPIOC->LCKR, tmp);
    WRITE_REG(GPIOC->LCKR, tmp | 0x00010000);
    tmp = READ_REG(GPIOC->LCKR);

    MODIFY_REG(GPIOD->AFR[0],   0x000000FF,  0x000000CC); /* Configure PD: 0 1 8 9 10 14 15     */
    MODIFY_REG(GPIOD->AFR[1],   0xFF000FFF,  0xCC000CCC); /* --(4Bit:C)FMC Alternate function   */
    MODIFY_REG(GPIOD->MODER,    0xF03F000F,  0xA02A000A); /* --(2Bit:2)Alternate function mode  */
    MODIFY_REG(GPIOD->OSPEEDR,  0xF03F000F,  0xF03F000F); /* --(2Bit:3)Very high speed          */
//  MODIFY_REG(GPIOD->PUPDR,    0xF03F000F,  0x50150005); /* --(2Bit:1)Pull-up                  */
    MODIFY_REG(GPIOD->PUPDR,    0xF03F000F,  0x00000000); /* --(2Bit:0)No Pull-up/Pull-down     */
    MODIFY_REG(GPIOD->OTYPER,   0x0000C703,  0x00000000); /* --(1Bit:0)Output type push-pull    */
    tmp = READ_BIT(GPIOD->LCKR, 0x0000FFFF) |0x0000C703;  /* --Port configuration locked        */
    WRITE_REG(GPIOD->LCKR, tmp | 0x00010000);
    WRITE_REG(GPIOD->LCKR, tmp);
    WRITE_REG(GPIOD->LCKR, tmp | 0x00010000);
    tmp = READ_REG(GPIOD->LCKR);

    MODIFY_REG(GPIOE->AFR[0],   0xF00000FF,  0xC00000CC); /* Configure PE: 0 1 7~15             */
    MODIFY_REG(GPIOE->AFR[1],   0xFFFFFFFF,  0xCCCCCCCC); /* --(4Bit:C)FMC Alternate function   */
    MODIFY_REG(GPIOE->MODER,    0xFFFFC00F,  0xAAAA800A); /* --(2Bit:2)Alternate function mode  */
    MODIFY_REG(GPIOE->OSPEEDR,  0xFFFFC00F,  0xFFFFC00F); /* --(2Bit:3)Very high speed          */
//  MODIFY_REG(GPIOE->PUPDR,    0xFFFFC00F,  0x55554005); /* --(2Bit:1)Pull-up                  */
    MODIFY_REG(GPIOE->PUPDR,    0xFFFFC00F,  0x00000000); /* --(2Bit:0)No Pull-up/Pull-down     */
    MODIFY_REG(GPIOE->OTYPER,   0x0000FF83,  0x00000000); /* --(1Bit:0)Output type push-pull    */
    tmp = READ_BIT(GPIOE->LCKR, 0x0000FFFF) |0x0000FF83;  /* --Port configuration locked        */
    WRITE_REG(GPIOE->LCKR, tmp | 0x00010000);
    WRITE_REG(GPIOE->LCKR, tmp);
    WRITE_REG(GPIOE->LCKR, tmp | 0x00010000);
    tmp = READ_REG(GPIOE->LCKR);

    MODIFY_REG(GPIOF->AFR[0],   0x00FFFFFF,  0x00CCCCCC); /* Configure PF: 0~5 11~15            */
    MODIFY_REG(GPIOF->AFR[1],   0xFFFFF000,  0xCCCCC000); /* --(4Bit:C)FMC Alternate function   */
    MODIFY_REG(GPIOF->MODER,    0xFFC00FFF,  0xAA800AAA); /* --(2Bit:2)Alternate function mode  */
    MODIFY_REG(GPIOF->OSPEEDR,  0xFFC00FFF,  0xFFC00FFF); /* --(2Bit:3)Very high speed          */
//  MODIFY_REG(GPIOF->PUPDR,    0xFFC00FFF,  0x55400555); /* --(2Bit:1)Pull-up                  */
    MODIFY_REG(GPIOF->PUPDR,    0xFFC00FFF,  0x00000000); /* --(2Bit:0)No Pull-up/Pull-down     */
    MODIFY_REG(GPIOF->OTYPER,   0x0000F83F,  0x00000000); /* --(1Bit:0)Output type push-pull    */
    tmp = READ_BIT(GPIOF->LCKR, 0x0000FFFF) |0x0000F83F;  /* --Port configuration locked        */
    WRITE_REG(GPIOF->LCKR, tmp | 0x00010000);
    WRITE_REG(GPIOF->LCKR, tmp);
    WRITE_REG(GPIOF->LCKR, tmp | 0x00010000);
    tmp = READ_REG(GPIOF->LCKR);

    MODIFY_REG(GPIOG->AFR[0],   0x00FF0FFF,  0x00CC0CCC); /* Configure PG: 0 1 2 4 5 8 15       */
    MODIFY_REG(GPIOG->AFR[1],   0xF000000F,  0xC000000C); /* --(4Bit:C)FMC Alternate function   */
    MODIFY_REG(GPIOG->MODER,    0xC0030F3F,  0x80020A2A); /* --(2Bit:2)Alternate function mode  */
    MODIFY_REG(GPIOG->OSPEEDR,  0xC0030F3F,  0xC0030F3F); /* --(2Bit:3)Very high speed          */
//  MODIFY_REG(GPIOG->PUPDR,    0xC0030F3F,  0x40010515); /* --(2Bit:1)Pull-up                  */
    MODIFY_REG(GPIOG->PUPDR,    0xC0030F3F,  0x00000000); /* --(2Bit:0)No Pull-up/Pull-down     */
    MODIFY_REG(GPIOG->OTYPER,   0x00008137,  0x00000000); /* --(1Bit:0)Output type push-pull    */
    tmp = READ_BIT(GPIOG->LCKR, 0x0000FFFF) |0x00008137;  /* --Port configuration locked        */
    WRITE_REG(GPIOG->LCKR, tmp | 0x00010000);
    WRITE_REG(GPIOG->LCKR, tmp);
    WRITE_REG(GPIOG->LCKR, tmp | 0x00010000);
    tmp = READ_REG(GPIOG->LCKR);

    /*-- FMC Configuration ---------------------------------------------------------------------*/
    __NOP();  __NOP();  __NOP();  __NOP();
    SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_FMCEN);           /* Enable the FMC interface clock       */
    __NOP();  __NOP();  __NOP();  __NOP();              /* Delay after a RCC clock enabling     */

    MODIFY_REG(   FMC_Bank5_6->SDCR[0]
                ,   FMC_SDCR1_SDCLK
                  | FMC_SDCR1_RBURST
                  | FMC_SDCR1_RPIPE
                ,   FMC_SDCR1_SDCLK_1                   /* (HCLK / 2) for SDRAM clock           */
                  | FMC_SDCR1_RBURST                    /* Enables burst read                   */
                  | FMC_SDCR1_RPIPE_0                   /* 1 HCLK clock cycle for Read delay    */
              );
    __NOP();  __NOP();  __NOP();  __NOP();
    MODIFY_REG(   FMC_Bank5_6->SDCR[0]
                ,   FMC_SDCR1_NC
                  | FMC_SDCR1_NR
                  | FMC_SDCR1_MWID
                  | FMC_SDCR1_NB
                  | FMC_SDCR1_CAS
                  | FMC_SDCR1_WP
                ,   FMC_SDCR1_NC_0                      /* 9 bits column address                */
                  | FMC_SDCR1_NR_1                      /* 13 bits row address                  */
                  | FMC_SDCR1_MWID_0                    /* 16 bits data bus width               */
                  | FMC_SDCR1_NB                        /* 4 internal Banks                     */
                  | FMC_SDCR1_CAS                       /* 3 cycles CAS Latency                 */
                  | 0                                   /* Write accesses allowed               */
              );
    __NOP();  __NOP();  __NOP();  __NOP();

    MODIFY_REG(   FMC_Bank5_6->SDTR[0]
                ,   FMC_SDTR1_TRC
                  | FMC_SDTR1_TRP
                ,   FMC_SDTR1_TRC_2 | FMC_SDTR1_TRC_0   /* 6 cycle for Row cycle delay          */
                  | FMC_SDTR1_TRP_0                     /* 2 cycle for Row precharge delay      */
              );
    __NOP();  __NOP();  __NOP();  __NOP();
    MODIFY_REG(   FMC_Bank5_6->SDTR[0]
                ,   FMC_SDTR1_TMRD
                  | FMC_SDTR1_TXSR
                  | FMC_SDTR1_TRAS
                  | FMC_SDTR1_TWR                       /* ## must be programmed to the slowest */
                  | FMC_SDTR1_TRCD
                ,   FMC_SDTR1_TMRD_0                    /* 2 cycle for Load Mode REG to Active  */
                  | FMC_SDTR1_TXSR_2 | FMC_SDTR1_TXSR_1 /* 8 cycle for Exit Self-refresh delay  */
                                     | FMC_SDTR1_TXSR_0
                  | FMC_SDTR1_TRAS_2 | FMC_SDTR1_TRAS_0 /* 6 cycle for Self-refresh time        */
                  | FMC_SDTR1_TWR_1 | FMC_SDTR1_TWR_0   /* 4 cycle for Recovery delay           */
                  | FMC_SDTR1_TRCD_0                    /* 2 cycle for Row to column delay      */
              );
    __NOP();  __NOP();  __NOP();  __NOP();

    /*-- SDRAM initialization sequence ---------------------------------------------------------*/
    FMC_Bank5_6->SDCMR =   FMC_SDCMR_CTB1           /* Clock enable command                     */
                         | FMC_SDCMR_MODE_0;
    __NOP();  __NOP();  __NOP();  __NOP();
    for(timeout = 0xFFFFF;  timeout > 0;  timeout--) {
        if( !(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) )  break;
    }
    for(timeout = 0xFFFFF;  timeout > 0;  timeout--) ; // 20000 /* Delay 200us                  */

    FMC_Bank5_6->SDCMR =   FMC_SDCMR_CTB1           /* PALL command                             */
                         | FMC_SDCMR_MODE_1;
    __NOP();  __NOP();  __NOP();  __NOP();
    for(timeout = 0xFFFF;  timeout > 0;  timeout--) {
        if( !(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) )  break;
    }
    __NOP();  __NOP();  __NOP();  __NOP();

    FMC_Bank5_6->SDCMR =   FMC_SDCMR_CTB1           /* Auto refresh command                     */
                         | FMC_SDCMR_MODE_1 | FMC_SDCMR_MODE_0
                         | FMC_SDCMR_NRFS_2 | FMC_SDCMR_NRFS_1 | FMC_SDCMR_NRFS_0 ;
    __NOP();  __NOP();  __NOP();  __NOP();
    for(timeout = 0xFFFFF;  timeout > 0;  timeout--) {
        if( !(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) )  break;
    }
    __NOP();  __NOP();  __NOP();  __NOP();

    FMC_Bank5_6->SDCMR =   FMC_SDCMR_CTB1           /* Load Mode Register command               */
                         | FMC_SDCMR_MODE_2
                         | (1 << (0 + 9))               /* --Burst Length: 8 (1/2/4/8)              */
                         | (0 << (3 + 9))           /* --Addressing Mode: Sequential            */
                         | (3 << (4 + 9))           /* --CAS Latency: 3 (2/3)                   */
                         | (0 << (7 + 9))           /* --Operating Mode: Normal                 */
                         | (1 << (9 + 9)) ;         /* --Write Mode: Burst read and Single write*/
    __NOP();  __NOP();  __NOP();  __NOP();
    for(timeout = 0xFFFFF;  timeout > 0;  timeout--) {
        if( !(FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) )  break;
    }
    __NOP();  __NOP();  __NOP();  __NOP();

    FMC_Bank5_6->SDRTR = 560 << 1;                  /* Set refresh count: 64*1000/8192*84-20    */
    __NOP();  __NOP();  __NOP();  __NOP();
    CLEAR_BIT(FMC_Bank5_6->SDCR[0], FMC_SDCR1_WP);  /* Disable write protection                 */
    __NOP();  __NOP();  __NOP();  __NOP();

    (void)(tmp);
}


/**********************************************************************************************************/
/** @brief  SDRAM Check.
***********************************************************************************************************/

void SDRAM_Check( void )
{
    static const uint32_t   TestMask[] = {  0x924CA635, 0x9CA24635, 0x9CA63245, 0x29CA6354
                                          , 0x942CA635, 0x9CA42635, 0x9CA63425, 0x49CA6352 };
    uint32_t    timeout;
    uint32_t    i, j, tmp;
    uint32_t   *p;


    while (1)
    {

    // Write test data to SDRAM
    for(p = (void*)SDRAM_ADDR, i = 0;  i < SDRAM_SIZE / sizeof(TestMask);  i++)
    {
        for(j = 0;  j < ARRY_SIZE(TestMask);  p++, j++)
        {
            SDRAM_W(p, TestMask[j] ^ NBR_CONVERT(p));
        }
    }

    // Delay 70ms
    for(timeout = 0x3FFFFF;  timeout > 0;  timeout--) ;

    // Read and Check test data from SDRAM
    for(p = (void*)SDRAM_ADDR, i = 0;  i < SDRAM_SIZE / sizeof(TestMask);  i++)
    {
        for(j = 0;  j < ARRY_SIZE(TestMask);  p++, j++)
        {
            SDRAM_R(p, tmp);
            if( tmp != (TestMask[j] ^ NBR_CONVERT(p)) ) {
                //continue;
                while(1) ;
            }
        }
    }

    }
}


/**********************************************************************************************************/
#ifdef RTE_CMSIS_RTOS2
#include "cmsis_os2.h"                  // osKernelGetSysTimerCount() osKernelGetSysTimerFreq()

/**********************************************************************************************************/
/** @brief      SDRAM Read Performance Analysis.
***
*** @return     Millisecond of the read all SDRAM.
***********************************************************************************************************/

uint32_t SDRAM_PerfRead( void )
{
    uint32_t    tmp;
    uint32_t   *ptr;
    uint32_t    SysTmrCnt = osKernelGetSysTimerCount();
    
    for(ptr = (void*)SDRAM_ADDR;  ((uint32_t)ptr) < (SDRAM_ADDR + SDRAM_SIZE);)
    {
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
        SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;  SDRAM_R(ptr, tmp); ptr++;
    }
    (void)tmp;  //lint !e771

    SysTmrCnt = osKernelGetSysTimerCount() - SysTmrCnt;
    return( (uint32_t)(((uint64_t)SysTmrCnt) * 1000 / osKernelGetSysTimerFreq()) );
}


/**********************************************************************************************************/
/** @brief      SDRAM Write Performance Analysis.
***
*** @return     Millisecond of the write all SDRAM.
***
*** @note       Will destroy all data in SDRAM.
***********************************************************************************************************/

uint32_t SDRAM_PerfWrite( void )
{
    uint32_t   *ptr;
    uint32_t    SysTmrCnt = osKernelGetSysTimerCount();

    for(ptr = (void*)SDRAM_ADDR;  ((uint32_t)ptr) < (SDRAM_ADDR + SDRAM_SIZE);)
    {
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
        SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;  SDRAM_W(ptr, ptr); ptr++;
    }

    SysTmrCnt = (osKernelGetSysTimerCount() - SysTmrCnt) - (SDRAM_SIZE / sizeof(uint32_t));
    return( (uint32_t)(((uint64_t)SysTmrCnt) * 1000 / osKernelGetSysTimerFreq()) );
}
#endif


/*****************************  END OF FILE  **************************************************************/
/** @}
*** @}
***********************************************************************************************************/

