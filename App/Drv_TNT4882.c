/*******************************************************************************
 * @file    Drv_TNT4882.c
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/9/11
 * @brief   Driver for GPIB interface Chip of TNT4882.
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/9/11 -- Linghu -- the first version
 ******************************************************************************/
//lint +libclass(all)
//lint -$
//lint -e537                    // Repeated include file 'xxxx'
//lint -e750                    // local macro 'xxxx' not referenced
//lint -e752                    // local declarator 'xxxx' not referenced
//lint ++dassert(e)=
//lint ++dassert_msg(e, msg)=

#include "Drv_TNT4882.h"
#ifdef    APP_CFG
#include  APP_CFG               // Apps configuration
#endif
#define   LOG_TAG   "TNT4882"
#include "emb_log.h"
#include "emb_delay.h"

#define   ONLY_USED_REGS
#include "board.h"              // Device's defines

#include "stdint.h"             // Used: uint8_t, uint16_t, uint32_t
#include "ctype.h"              // Used: isprint()
#include "defines.h"            // Used: __WEAK


/*******************************************************************************
 * @addtogroup Drv_TNT4882
 * @{
 * @addtogroup              Private_Macros
 * @{
 ******************************************************************************/
//lint -emacro(506, GPIO_PIN_CFG)
/**
 * 设置 GPIO Pin 输出
 */
#define GPIO_PIN_CFG(GPIOx, PinMsk, stat)   ((stat) ? GPIO_PIN_SETBIT((GPIOx), (PinMsk)) \
                                                    : GPIO_PIN_CLRBIT((GPIOx), (PinMsk)) \
                                            )

//lint ++dTNT4882_DELAY_50NS()=
/**
 * 延时 50nS (纳秒) -- CMDR AUXMR AUXCR
 */
#ifndef TNT4882_DELAY_50NS
#  if   defined(STM32F1)
#define TNT4882_DELAY_50NS()    (__NOP(), __NOP(), __NOP(), __NOP())
#elif   defined(STM32F2)
#define TNT4882_DELAY_50NS()    (__NOP(), __NOP(), __NOP(), __NOP(), __NOP(), __NOP())
#else //defined(STM32F4)
#define TNT4882_DELAY_50NS()    (__NOP(), __NOP(), __NOP(), __NOP(), __NOP(), __NOP(), __NOP(), __NOP())
#endif
#endif

/**
 * 传输超时计数(以 50nS 为单位)
 */
#ifndef TNT4882_DELAY_CNT         // 100mS
#define TNT4882_DELAY_CNT           (100 * ((1000 * 1000) / 50))
#endif

/**
 * Pend 超时, 单位为(mS)
 */
#ifndef TNT4882_PEND_TIME         // 2000mS
#define TNT4882_PEND_TIME           (2000)
#endif

#ifndef TNT4882_GPIO_SPEED
#define TNT4882_GPIO_SPEED       GPIO_S_MEDIUM  // GPIO output speed
#endif

#ifndef TNT4882_INTR_NVIC_PRIO
#define TNT4882_INTR_NVIC_PRIO        12u       // EXTI NVIC Priority
#endif


/*-------------------- TNT4882 Pin Config ------------------------------------*/
#ifndef TNT4882_F1_PIN_SWJ_CFG      // Remap JTAG or SWD, 0: FULL , 1: No NJTRST
#define TNT4882_F1_PIN_SWJ_CFG      2   //                2: SW-DP, 4: Disable
#endif

#ifndef TNT4882_INTR_Pin            // INTR
#define TNT4882_INTR_Pin            GPIO_PIN_4
#define TNT4882_INTR_GPIO_Port      GPIOG
#define TNT4882_INTR_SOURCE_Port    6u
#define TNT4882_INTR_IRQ_Num        EXTI4_IRQn
#define TNT4882_IntrFunction        EXTI4_IRQHandler
#endif

#ifndef TNT4882_DRQ_Pin             // DRQ
#define TNT4882_DRQ_Pin             GPIO_PIN_0
#define TNT4882_DRQ_GPIO_Port       GPIOG
#endif
#define TNT4882_DRQ_INIT()          GPIO_PIN_INIT(TNT4882_DRQ_GPIO_Port, TNT4882_DRQ_Pin, GPIO_M_IN | GPIO_P_PD)
#define TNT4882_DRQ_GET()           GPIO_PIN_READ(TNT4882_DRQ_GPIO_Port, TNT4882_DRQ_Pin)

#ifndef TNT4882_RDY1_Pin            // RDY1
#define TNT4882_RDY1_Pin            GPIO_PIN_1
#define TNT4882_RDY1_GPIO_Port      GPIOG
#endif
#define TNT4882_RDY1_INIT()         GPIO_PIN_INIT(TNT4882_RDY1_GPIO_Port, TNT4882_RDY1_Pin, GPIO_M_IN | GPIO_P_PD)
#define TNT4882_RDY1_GET()          GPIO_PIN_READ(TNT4882_RDY1_GPIO_Port, TNT4882_RDY1_Pin)

#ifndef TNT4882_RDN_Pin             // RDN
#define TNT4882_RDN_Pin             GPIO_PIN_2
#define TNT4882_RDN_GPIO_Port       GPIOG
#endif
#define TNT4882_RDN_INIT()          GPIO_PIN_INIT(TNT4882_RDN_GPIO_Port, TNT4882_RDN_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED)
#define TNT4882_RDN_SET(stat)       GPIO_PIN_CFG( TNT4882_RDN_GPIO_Port, TNT4882_RDN_Pin, (stat))

#ifndef TNT4882_WRN_Pin             // WRN
#define TNT4882_WRN_Pin             GPIO_PIN_3
#define TNT4882_WRN_GPIO_Port       GPIOG
#endif
#define TNT4882_WRN_INIT()          GPIO_PIN_INIT(TNT4882_WRN_GPIO_Port, TNT4882_WRN_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED)
#define TNT4882_WRN_SET(stat)       GPIO_PIN_CFG( TNT4882_WRN_GPIO_Port, TNT4882_WRN_Pin, (stat))

#ifndef TNT4882_RESETN_Pin          // RESETN
#define TNT4882_RESETN_Pin          GPIO_PIN_5
#define TNT4882_RESETN_GPIO_Port    GPIOG
#endif
#define TNT4882_RESETN_INIT()       GPIO_PIN_INIT(TNT4882_RESETN_GPIO_Port, TNT4882_RESETN_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED)
#define TNT4882_RESETN_SET(stat)    GPIO_PIN_CFG( TNT4882_RESETN_GPIO_Port, TNT4882_RESETN_Pin, (stat))

#ifndef TNT4882_BBUSN_Pin           // BBUSN
#define TNT4882_BBUSN_Pin           GPIO_PIN_6
#define TNT4882_BBUSN_GPIO_Port     GPIOG
#endif
#define TNT4882_BBUSN_INIT()        GPIO_PIN_INIT(TNT4882_BBUSN_GPIO_Port, TNT4882_BBUSN_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED)
#define TNT4882_BBUSN_SET(stat)     GPIO_PIN_CFG( TNT4882_BBUSN_GPIO_Port, TNT4882_BBUSN_Pin, (stat))

#ifndef TNT4882_ABUSN_Pin           // ABUSN
#define TNT4882_ABUSN_Pin           GPIO_PIN_7
#define TNT4882_ABUSN_GPIO_Port     GPIOG
#endif
#define TNT4882_ABUSN_INIT()        GPIO_PIN_INIT(TNT4882_ABUSN_GPIO_Port, TNT4882_ABUSN_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED)
#define TNT4882_ABUSN_SET(stat)     GPIO_PIN_CFG( TNT4882_ABUSN_GPIO_Port, TNT4882_ABUSN_Pin, (stat))

#ifndef TNT4882_DACKN_Pin           // DACKN
#define TNT4882_DACKN_Pin           GPIO_PIN_8
#define TNT4882_DACKN_GPIO_Port     GPIOG
#endif
#define TNT4882_DACKN_INIT()        GPIO_PIN_INIT(TNT4882_DACKN_GPIO_Port, TNT4882_DACKN_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED)
#define TNT4882_DACKN_SET(stat)     GPIO_PIN_CFG( TNT4882_DACKN_GPIO_Port, TNT4882_DACKN_Pin, (stat))

#ifndef TNT4882_CSN_Pin             // CSN
#define TNT4882_CSN_Pin             GPIO_PIN_9
#define TNT4882_CSN_GPIO_Port       GPIOG
#endif
#define TNT4882_CSN_INIT()          GPIO_PIN_INIT(TNT4882_CSN_GPIO_Port, TNT4882_CSN_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED)
#define TNT4882_CSN_SET(stat)       GPIO_PIN_CFG( TNT4882_CSN_GPIO_Port, TNT4882_CSN_Pin, (stat))

#ifndef TNT4882_ADDR1_Pin           // ADDR1
#define TNT4882_ADDR1_Pin           GPIO_PIN_2
#define TNT4882_ADDR1_GPIO_Port     GPIOE
#endif
#ifndef TNT4882_ADDR2_Pin           // ADDR2
#define TNT4882_ADDR2_Pin           GPIO_PIN_3
#define TNT4882_ADDR2_GPIO_Port     GPIOE
#endif
#ifndef TNT4882_ADDR3_Pin           // ADDR3
#define TNT4882_ADDR3_Pin           GPIO_PIN_4
#define TNT4882_ADDR3_GPIO_Port     GPIOE
#endif
#ifndef TNT4882_ADDR4_Pin           // ADDR4
#define TNT4882_ADDR4_Pin           GPIO_PIN_5
#define TNT4882_ADDR4_GPIO_Port     GPIOE
#endif
#ifndef TNT4882_ADDR5_Pin           // ADDR5
#define TNT4882_ADDR5_Pin           GPIO_PIN_6
#define TNT4882_ADDR5_GPIO_Port     GPIOE
#endif
#define TNT4882_ADDR_INIT()       ( GPIO_PIN_INIT(TNT4882_ADDR1_GPIO_Port, TNT4882_ADDR1_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED), \
                                    GPIO_PIN_INIT(TNT4882_ADDR2_GPIO_Port, TNT4882_ADDR2_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED), \
                                    GPIO_PIN_INIT(TNT4882_ADDR3_GPIO_Port, TNT4882_ADDR3_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED), \
                                    GPIO_PIN_INIT(TNT4882_ADDR4_GPIO_Port, TNT4882_ADDR4_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED), \
                                    GPIO_PIN_INIT(TNT4882_ADDR5_GPIO_Port, TNT4882_ADDR5_Pin, GPIO_M_OUT | TNT4882_GPIO_SPEED)  \
                                  )
#define TNT4882_ADDR_SET(addr)    ( GPIO_PIN_CFG( TNT4882_ADDR1_GPIO_Port, TNT4882_ADDR1_Pin, (addr) & 0x01), \
                                    GPIO_PIN_CFG( TNT4882_ADDR2_GPIO_Port, TNT4882_ADDR2_Pin, (addr) & 0x02), \
                                    GPIO_PIN_CFG( TNT4882_ADDR3_GPIO_Port, TNT4882_ADDR3_Pin, (addr) & 0x04), \
                                    GPIO_PIN_CFG( TNT4882_ADDR4_GPIO_Port, TNT4882_ADDR4_Pin, (addr) & 0x08), \
                                    GPIO_PIN_CFG( TNT4882_ADDR5_GPIO_Port, TNT4882_ADDR5_Pin, (addr) & 0x10)  \
                                  )

#ifndef TNT4882_DATA_PORT           // D[0:16]
#define TNT4882_DATA_PORT           GPIOF
#endif
#define TNT4882_DATA_INIT()         GPIO_PortInit(  TNT4882_DATA_PORT, GPIO_M_OUT | GPIO_O_OD | GPIO_P_PU | TNT4882_GPIO_SPEED)
#define TNT4882_DATA_DIR_I()        GPIO_PORT_WRITE(TNT4882_DATA_PORT, 0xFFFF)
#define TNT4882_DATA_DIR_O()        GPIO_PORT_WRITE(TNT4882_DATA_PORT, 0xFFFF)
#define TNT4882_DATA_IN()           GPIO_PORT_READ( TNT4882_DATA_PORT)
#define TNT4882_DATA_OUT(data)      GPIO_PORT_WRITE(TNT4882_DATA_PORT, (data))


/*----------------------- GPIB address Pin Config ----------------------------*/
#ifndef GPIB_ADDR1_Pin                          /*
#define GPIB_ADDR1_Pin              GPIO_PIN_5
#define GPIB_ADDR1_GPIO_Port        GPIOB       */
#endif
#ifndef GPIB_ADDR2_Pin                          /*
#define GPIB_ADDR2_Pin              GPIO_PIN_2
#define GPIB_ADDR2_GPIO_Port        GPIOC       */
#endif
#ifndef GPIB_ADDR3_Pin                          /*
#define GPIB_ADDR3_Pin              GPIO_PIN_0
#define GPIB_ADDR3_GPIO_Port        GPIOC       */
#endif
#ifndef GPIB_ADDR4_Pin                          /*
#define GPIB_ADDR4_Pin              GPIO_PIN_14
#define GPIB_ADDR4_GPIO_Port        GPIOC       */
#endif
#ifndef GPIB_ADDR5_Pin                          /*
#define GPIB_ADDR5_Pin              GPIO_PIN_15
#define GPIB_ADDR5_GPIO_Port        GPIOC       */
#endif

#ifndef GPIB_ADDR_PIN_INIT
#define GPIB_ADDR_PIN_INIT(P, Pin)  GPIO_PIN_INIT((P), (Pin), GPIO_M_IN)
#endif
#ifndef GPIB_ADDR_PIN_READ
#define GPIB_ADDR_PIN_READ(P, Pin)  ( !GPIO_PIN_READ((P), (Pin)) )
#endif


/*----------------------- TNT4882 Regster ADD ONE MODE -----------------------*/
//        REGISTER     OFFSET    TYPE    BIT7    BIT6        BIT5      BIT4       BIT3     BIT2        BIT1        BIT0
#define TNT4882_ISR0    0x1D  //   R     nba     STBO        NL        EOS        IFCI     ATNI        TO          SYNC
#define TNT4882_IMR0    0x1D  //   W     1       STBO IE     NL EN     BTO        IFCI IE  ATNI IE     TO IE       SYNC IE
#define TNT4882_ISR1    0x02  //   R     CPT     APT         DET       END RX     DEC      ERR         DO          DI
#define TNT4882_IMR1    0x02  //   W     CPT IE  APT IE      DET IE    END RX IE  DEC IE   ERR IE      DO IE       DI IE
#define TNT4882_ISR2    0x04  //   R     INT     X           LOK       REM        X        LOKC        REMC        ADSC
#define TNT4882_IMR2    0x04  //   W     0       0           DMAO      DMAI       0        LOKC IE     REMC IE     ADSC IE
#define TNT4882_ISR3    0x1A  //   R     INT     INTSRC 2    X         STOP       NFF      NEF         TLC INT     DONE
#define TNT4882_IMR3    0x12  //  R/W    0       INTSRC2 IE  0         STOP IE    NFF IE   NEF IE      TLC INT IE  DONE IE
#define TNT4882_ADSR    0x08  //   R     X       ATN         SPMS      LPAS       TPAS     LA          TA          MJMN
#define TNT4882_STS1    0x10  //   R     DONE    0           IN        DRQ        STOP     DAV         HALT        GSYNC
#define TNT4882_STS2    0x1C  //   R     1       16/8N       0         1          AFFN     AEFN        BFFN        BEFN

#define TNT4882_ADR     0x0C  //   W     ARS     DT          DL        AD5        AD4      AD3         AD2         AD1
#define TNT4882_ADR0    0x0C  //   R     X       DT0         DL0       AD5-0      AD4-0    AD3-0       AD2-0       AD1-0
#define TNT4882_ADR1    0x0E  //   R     EOI     DT1         DL1       AD5-1      AD4-1    AD3-1       AD2-1       AD1-1
#define TNT4882_ADMR    0x08  //   W     ton     lon         1         1          0        0           ADM1        ADM0
#define TNT4882_AUXMR   0x0A  //   W     AUX7    AUX6        AUX5      AUX4       AUX3     AUX2        AUX1        AUX0
#define TNT4882_BSR     0x1F  //   R     ATN     DAV         NDAC      NRFD       EOI      SRQ         IFC         REN
#define TNT4882_BCR     0x1F  //   W     ATN     DAV         NDAC      NRFD       EOI      SRQ         IFC         REN
#define TNT4882_SASR    0x1B  //   R     nba     AEHS        ANHS1     ANHS2      ADHS     ACRDY       SH1A        SH1B
#define TNT4882_DCR     0x1B  //   W     DIO8    DIO7        DIO6      DIO5       DIO4     DIO3        DIO2        DIO1
#define TNT4882_DSR     0x11  //   R     DIO8    DIO7        DIO6      DIO5       DIO4     DIO3        DIO2        DIO1
#define TNT4882_SPSR    0x06  //   R     S8      PEND        S6        S5         S4       S3          S2          S1
#define TNT4882_SPMR    0x06  //   W     S8      rsv/RQS     S6        S5         S4       S3          S2          S1
#define TNT4882_CNT0    0x14  //  R/W    CNT7    CNT6        CNT5      CNT4       CNT3     CNT2        CNT1        CNT0
#define TNT4882_CNT1    0x16  //  R/W    CNT15   CNT14       CNT13     CNT12      CNT11    CNT10       CNT9        CNT8
#define TNT4882_CNT2    0x09  //  R/W    CNT23   CNT22       CNT21     CNT20      CNT19    CNT18       CNT17       CNT16
#define TNT4882_CNT3    0x0B  //  R/W    CNT31   CNT30       CNT29     CNT28      CNT27    CNT26       CNT25       CNT24
#define TNT4882_HSSEL   0x0D  //   W     0       0           GO2 SIDS  NO DMA     0        0           0           ONEC
#define TNT4882_EOSR    0x0E  //   W     EOS7    EOS6        EOS5      EOS4       EOS3     EOS2        EOS1        EOS0
#define TNT4882_SH_CNT  0x11  //   W     CNT2    CNT1        CNT0      TD4        TD3      TD2         TD1         TD0
#define TNT4882_HIER    0x13  //   W     DGA     DGB         0         NO_TSETUP  0        0           0           PMT_W_EOS
#define TNT4882_MISC    0x15  //   W     0       0           0         HSE        SLOW     WRAP        NOAS        NOTS
#define TNT4882_FIFOB   0x18  //  R/W    FB7     FB6         FB5       FB4        FB3      FB2         FB1         FB0
#define TNT4882_FIFOA   0x19  //  R/W    FA15    FA14        FA13      FA12       FA11     FA10        FA9         FA8
#define TNT4882_CFG     0x10  //   W     0       TLC HLTE    IN        A/BN       CCEN     TMOE        TIM/BYTN    16/8N
#define TNT4882_CMDR    0x1C  //   W     CMD7    CMD6        CMD5      CMD4       CMD3     CMD2        CMD1        CMD0
#define TNT4882_TIMER   0x1E  //  R/W    TMR7    TMR6        TMR5      TMR4       TMR3     TMR2        TMR1        TMR0
#define TNT4882_KEYREG  0x17  //   W     0       SWAP        0         0          KEY CLK  KEY DAT EN  KEY DATA    KEY RST
#define TNT4882_CSR     0x17  //   R     V3      V2          V1        V0         KEY DQ   MODE        0           0
#define TNT4882_CCR     0x1A  //   W    D7      D6          D5        D4         D3       D2          D1          D0
#define TNT4882_ACCWR   0x05  //   W    0       0           0         0          0        0           0           DMAEN
#define TNT4882_CPTR    0x0A  //   R    CPT7    CPT6        CPT5      CPT4       CPT3     CPT2        CPT1        CPT0
#define TNT4882_CDOR    0x00  //   W    DIO8    DIO7        DIO6      DIO5       DIO4     DIO3        DIO2        DIO1
#define TNT4882_DIR     0x00  //   R    DIO8    DIO7        DIO6      DIO5       DIO4     DIO3        DIO2        DIO1
#define TNT4882_INTR    0x07  //   W    0       0           0         0          0        0           0           INTEN

// TNT4882 INTERRUPT ENABLE  IMR0
#define TNT4882_IMR0_BASE           0x80
#define TNT4882_IMR0_STBO_IE       (0x40 + TNT4882_IMR0_BASE)
#define TNT4882_IMR0_NL_EN         (0x20 + TNT4882_IMR0_BASE)
#define TNT4882_IMR0_BTO_EN        (0x10 + TNT4882_IMR0_BASE)
#define TNT4882_IMR0_IFCI_IE       (0x08 + TNT4882_IMR0_BASE)
#define TNT4882_IMR0_ATNI_IE       (0x04 + TNT4882_IMR0_BASE)
#define TNT4882_IMR0_TO_IE         (0x02 + TNT4882_IMR0_BASE)
#define TNT4882_IMR0_SYNC_IE       (0x01 + TNT4882_IMR0_BASE)

//lint -emacro(835, TNT4882_ISR0_*)
// TNT4882 INTERRUPT ISR0
#define TNT4882_ISR0_BASE          0x00
#define TNT4882_ISR0_nba_RX       (0x80 + TNT4882_ISR0_BASE)
#define TNT4882_ISR0_STBO_RX      (0x40 + TNT4882_ISR0_BASE)
#define TNT4882_ISR0_NL_RX        (0x20 + TNT4882_ISR0_BASE)
#define TNT4882_ISR0_EOS_RX       (0x10 + TNT4882_ISR0_BASE)
#define TNT4882_ISR0_IFCI_RX      (0x08 + TNT4882_ISR0_BASE)
#define TNT4882_ISR0_ATNI_RX      (0x04 + TNT4882_ISR0_BASE)
#define TNT4882_ISR0_TO_RX        (0x02 + TNT4882_ISR0_BASE)
#define TNT4882_ISR0_SYNC_RX      (0x01 + TNT4882_ISR0_BASE)

//lint -emacro(835, TNT4882_IMR1_*)
// TNT4882 INTERRUPT ENABLE  IMR1
#define TNT4882_IMR1_BASE           0x00
#define TNT4882_IMR1_CPT_IE        (0x80 + TNT4882_IMR1_BASE)
#define TNT4882_IMR1_APT_IE        (0x40 + TNT4882_IMR1_BASE)
#define TNT4882_IMR1_DET_IE        (0x20 + TNT4882_IMR1_BASE)
#define TNT4882_IMR1_ENDRX_IE      (0x10 + TNT4882_IMR1_BASE)
#define TNT4882_IMR1_DEC_IE        (0x08 + TNT4882_IMR1_BASE)
#define TNT4882_IMR1_ERR_IE        (0x04 + TNT4882_IMR1_BASE)
#define TNT4882_IMR1_DO_IE         (0x02 + TNT4882_IMR1_BASE)
#define TNT4882_IMR1_DI_IE         (0x01 + TNT4882_IMR1_BASE)

//lint -emacro(835, TNT4882_ISR1_*)
// TNT4882 INTERRUPT ISR1
#define TNT4882_ISR1_BASE           0x00
#define TNT4882_ISR1_CPT_RX        (0x80 + TNT4882_ISR1_BASE)
#define TNT4882_ISR1_APT_RX        (0x40 + TNT4882_ISR1_BASE)
#define TNT4882_ISR1_DET_RX        (0x20 + TNT4882_ISR1_BASE)
#define TNT4882_ISR1_END_RX        (0x10 + TNT4882_ISR1_BASE)
#define TNT4882_ISR1_DEC_RX        (0x08 + TNT4882_ISR1_BASE)
#define TNT4882_ISR1_ERR_RX        (0x04 + TNT4882_ISR1_BASE)
#define TNT4882_ISR1_DO_RX         (0x02 + TNT4882_ISR1_BASE)
#define TNT4882_ISR1_DI_RX         (0x01 + TNT4882_ISR1_BASE)

//lint -emacro(835, TNT4882_IMR2_*)
// TNT4882 INTERRUPT ENABLE  IMR2
#define TNT4882_IMR2_BASE           0x00
#define TNT4882_IMR2_DMAO_EN       (0x20 + TNT4882_IMR2_BASE)
#define TNT4882_IMR2_DMAI_EN       (0x10 + TNT4882_IMR2_BASE)
#define TNT4882_IMR2_LOCK_IE       (0x04 + TNT4882_IMR2_BASE)
#define TNT4882_IMR2_REMC_IE       (0x02 + TNT4882_IMR2_BASE)
#define TNT4882_IMR2_ADSC_IE       (0x01 + TNT4882_IMR2_BASE)

//lint -emacro(835, TNT4882_ISR2_*)
// TNT4882 INTERRUPT ISR2
#define TNT4882_ISR2_BASE           0x00
#define TNT4882_ISR2_INT_RX        (0x80 + TNT4882_ISR2_BASE)
#define TNT4882_ISR2_LOK_RX        (0x20 + TNT4882_ISR2_BASE)
#define TNT4882_ISR2_REM_RX        (0x10 + TNT4882_ISR2_BASE)
#define TNT4882_ISR2_LOKC_RX       (0x04 + TNT4882_ISR2_BASE)
#define TNT4882_ISR2_REMC_RX       (0x02 + TNT4882_ISR2_BASE)
#define TNT4882_ISR2_ADSC_RX       (0x01 + TNT4882_ISR2_BASE)

//lint -emacro(835, TNT4882_IMR3_*)
// TNT4882 INTERRUPT ENABLE  IMR3
#define TNT4882_IMR3_BASE           0x00
#define TNT4882_IMR3_INTSRC2_EN1   (0x40 + TNT4882_IMR3_BASE)
#define TNT4882_IMR3_STOP_EN1      (0x10 + TNT4882_IMR3_BASE)
#define TNT4882_IMR3_NFF_IE1       (0x08 + TNT4882_IMR3_BASE)
#define TNT4882_IMR3_NEF_IE1       (0x04 + TNT4882_IMR3_BASE)
#define TNT4882_IMR3_TLC_IE1       (0x02 + TNT4882_IMR3_BASE)
#define TNT4882_IMR3_DONE_IE1      (0x01 + TNT4882_IMR3_BASE)

//lint -emacro(835, TNT4882_ISR3_*)
// TNT4882 INTERRUPT ISR3
#define TNT4882_ISR3_BASE           0x00
#define TNT4882_ISR3_INT3_RX       (0x80 + TNT4882_ISR3_BASE)
#define TNT4882_ISR3_INTSRC2_RX    (0x20 + TNT4882_ISR3_BASE)
#define TNT4882_ISR3_STOP_RX       (0x10 + TNT4882_ISR3_BASE)
#define TNT4882_ISR3_NFF_RX        (0x08 + TNT4882_ISR3_BASE)
#define TNT4882_ISR3_NEF_RX        (0x04 + TNT4882_ISR3_BASE)
#define TNT4882_ISR3_TLC_RX        (0x02 + TNT4882_ISR3_BASE)
#define TNT4882_ISR3_DONE_RX       (0x01 + TNT4882_ISR3_BASE)

//TNT4882 AUXMR COMMAND
#define TNT4882_AUXMR_POWER_ON                 0x00 // pon
#define TNT4882_AUXMR_CLR_PARALLEL_POLL_FLAG   0x01 // ~ist
#define TNT4882_AUXMR_CHIP_RESET               0x02 // ch rst
#define TNT4882_AUXMR_FINISH_HANDSHAKE         0x03 // rhdf
#define TNT4882_AUXMR_TRIGGER                  0x04 // trig
#define TNT4882_AUXMR_CLR_PULSE_2_LOCAL        0x05 // rtl
#define TNT4882_AUXMR_SEND_EOI                 0x06 // seoi
#define TNT4882_AUXMR_NOVALID_2_COM_ADDR       0x07 // nonvalid
#define TNT4882_AUXMR_SET_PARALLEL_POLL_FLAG   0x09 // ist
#define TNT4882_AUXMR_UNTALK_COM               0x0B // lut
#define TNT4882_AUXMR_UNLISTEN_COM             0x0C // lul
#define TNT4882_AUXMR_SET_2_LOCAL              0x0D //
#define TNT4882_AUXMR_NBA_FALSE                0x0E // nbaf
#define TNT4882_AUXMR_VALID_2_COM_ADDR         0x0F // valid
#define TNT4882_AUXMR_SWITCH_2_9914            0x15 //
#define TNT4882_AUXMR_RSV_TRUE                 0x18 // reqt
#define TNT4882_AUXMR_RSV_FALSE                0x19 // reqf
#define TNT4882_AUXMR_PAGEIN_ADDI_REG          0x50 // valid
#define TNT4882_AUXMR_HOLDOFF_HANDSHAKE        0x51 // hldi
#define TNT4882_AUXMR_CLEAR_DET                0x54 // ISR1[5]r
#define TNT4882_AUXMR_CLEAR_END                0x55 // ISR1[4]r
#define TNT4882_AUXMR_CLEAR_DEC                0x56 // ISR1[3]r
#define TNT4882_AUXMR_CLEAR_ERR                0x57 // ISR1[2]r
#define TNT4882_AUXMR_CLEAR_LOKC               0x59 // ISR2[2]r
#define TNT4882_AUXMR_CLEAR_REMC               0x5A // ISR2[1]r
#define TNT4882_AUXMR_CLEAR_ADSC               0x5B // ISR2[0]r
#define TNT4882_AUXMR_CLEAR_IFCI               0x5C // ISR0[3]r
#define TNT4882_AUXMR_CLEAR_ATNI               0x5D // ISR0[2]r
#define TNT4882_AUXMR_CLEAR_SYNC               0x5E // ISR0[0]r
#define TNT4882_AUXMR_SET_SYNC                 0x5F // ISR0[0]r

#define TNT4882_AUXMR_NORMAL_HANDSHAKE_MODE    0x80 //
#define TNT4882_AUXMR_ONALLDATA_HANDSHAKE_MODE 0x81 //
#define TNT4882_AUXMR_ONENDDATA_HANDSHAKE_MODE 0x82 //
#define TNT4882_AUXMR_NO_HANDSHAKE_MODE        0x83 //

//TNT4882 CMDR COMMAND
#define TNT4882_CMDR_GO                        0x04 // clear HALT signal
#define TNT4882_CMDR_STOP                      0x08 // set HALT signal
#define TNT4882_CMDR_RESET_FIFO                0x10 // reset FIFO
#define TNT4882_CMDR_SOFT_RESET                0x22 // chip reset


/*------------------------ TNT4882 Status ------------------------------------*/
// #define TNT4882_STATUS_NULL         0
// #define TNT4882_STATUS_LISTEN       1
// #define TNT4882_STATUS_TALK_EN      2
// #define TNT4882_STATUS_TALKING      3

// TNT4882 STATUS
#define TNT4882_LINK_NULL       0
#define TNT4882_LINK_LISTEN     1
#define TNT4882_LINK_TALK       2

// GET TNT4882 MESSAGE
#define TNT4882_NO_MESSAGE      0
#define TNT4882_REC_MESSAGE     1
#define TNT4882_TX_MESSAGE      2


/*******************************************************************************
 * @}
 * @addtogroup              Private_Variables
 * @{
 ******************************************************************************/

static volatile uint8_t* _TNT4882_txBuf = NULL;             // 发送缓存指针
static volatile uint16_t _TNT4882_txLen = 0;                // 发送缓存长度(字节)
static volatile uint16_t _TNT4882_txPos = 0;                // 发送缓存位置

static volatile uint8_t* _TNT4882_rxBuf = NULL;             // 接收缓存指针
static volatile uint16_t _TNT4882_rxLen = 0;                // 接收缓存长度(字节)
static volatile uint16_t _TNT4882_rxPos = 0;                // 接收缓存位置
static volatile uint16_t _TNT4882_rxPrv = 0;                // 接收数据长度(发送接收完成事件时)

static          void   (*_TNT4882_ListenEnd )(void) = NULL; // 事件回调函数: listen 结束
static          void   (*_TNT4882_TalkEnable)(void) = NULL; // 事件回调函数: talk   允许

static volatile uint8_t  _TNT4882_TalkStatus = TNT4882_NO_MESSAGE;
static volatile uint8_t  _TNT4882_TalkInit   = TNT4882_NO_MESSAGE;
static volatile uint8_t  _TNT4882_LinkStatus = TNT4882_LINK_NULL;


/*******************************************************************************
 * @}
 * @addtogroup              Private_Prototypes
 * @{
 ******************************************************************************/
//lint -ecall(835, TNT4882_RegWrite, TNT4882_RegRead)

static void TNT4882_PinInit(void);
static void TNT4882_HardRst(void);

static void    TNT4882_RegWrite(uint8_t addr, uint8_t data);
static uint8_t TNT4882_RegRead (uint8_t addr);

static void TNT4882_ToTalker(uint16_t len);
static void TNT4882_ToListen(void);

static void TNT4882_IntrInit   (void);
static void TNT4882_IntrHandler(void);


/*******************************************************************************
 * @}
 * @addtogroup              Private_Functions
 * @{
 ******************************************************************************/
/**
 * TNT4882 Pins Init
 */
static void TNT4882_PinInit(void)
{
    TNT4882_DRQ_INIT();         // DRQ
    TNT4882_DRQ_GET();

    TNT4882_RDY1_INIT();        // RDY1
    TNT4882_RDY1_GET();

    TNT4882_RDN_INIT();         // RDN
    TNT4882_RDN_SET(1);

    TNT4882_WRN_INIT();         // WRN
    TNT4882_WRN_SET(1);

    TNT4882_RESETN_INIT();      // RESETN
    TNT4882_RESETN_SET(1);

    TNT4882_BBUSN_INIT();       // BBUSN
    TNT4882_BBUSN_SET(0);

    TNT4882_ABUSN_INIT();       // ABUSN
    TNT4882_ABUSN_SET(1);

    TNT4882_DACKN_INIT();       // DACKN
    TNT4882_DACKN_SET(1);

    TNT4882_CSN_INIT();         // CSN
    TNT4882_CSN_SET(1);

    TNT4882_ADDR_INIT();        // ADDR BUS
    TNT4882_ADDR_SET(0x1F);

    TNT4882_DATA_INIT();        // DATA BUS
    TNT4882_DATA_OUT(0xFFFF);

    TNT4882_DELAY_50NS();
    TNT4882_DELAY_50NS();
}


/**
 * TNT4882 hardware Reset
 */
static void TNT4882_HardRst(void)
{
    TNT4882_RESETN_SET(0);
    DelayMS(50);
    TNT4882_RESETN_SET(1);
    DelayMS(50);
}


/**
 * TNT4882 Register Write
 *
 * @param[in]  addr  Register Address
 * @param[in]  data  Data to Write
 */
static void TNT4882_RegWrite(uint8_t addr, uint8_t data)
{
    uint32_t    timeout;

    for(timeout = TNT4882_DELAY_CNT;  timeout != 0;  timeout--) {
        if(TNT4882_RDY1_GET() == 0) { break; }
        TNT4882_DELAY_50NS();       // Waitting RDY1 to 0
    }

    TNT4882_DATA_DIR_O();           // Data Bus direction
    TNT4882_ADDR_SET(addr);         // ADDR[]
    TNT4882_CSN_SET(0);             // CSN
    TNT4882_WRN_SET(0);             // WRN
    TNT4882_DATA_OUT(0xFF00 | data);// DATA[]
    TNT4882_DELAY_50NS();  TNT4882_DELAY_50NS();
    TNT4882_DELAY_50NS();  TNT4882_DELAY_50NS();
    TNT4882_DELAY_50NS();  TNT4882_DELAY_50NS();
  //TNT4882_DELAY_50NS();  TNT4882_DELAY_50NS();

    for(timeout = TNT4882_DELAY_CNT;  timeout != 0;  timeout--) {
        if(TNT4882_RDY1_GET() != 0) { break; }
        TNT4882_DELAY_50NS();       // Waitting RDY1 to 1
    }
    TNT4882_WRN_SET(1);             // WRN
    TNT4882_CSN_SET(1);             // CSN
}


/**
 * TNT4882 Register Read
 *
 * @param[in]  addr  Register Address
 *
 * @return  Data of Read
 */
static uint8_t TNT4882_RegRead(uint8_t addr)
{
    uint32_t    timeout;
    uint8_t     data;

    for(timeout = TNT4882_DELAY_CNT;  timeout != 0;  timeout--) {
        if(TNT4882_RDY1_GET() == 0) { break; }
        TNT4882_DELAY_50NS();       // Waitting RDY1 to 0
    }
    TNT4882_DATA_DIR_I();           // Data Bus direction
    TNT4882_ADDR_SET(addr);         // ADDR[]
    TNT4882_CSN_SET(0);             // CSN
    TNT4882_RDN_SET(0);             // RDN

    for(timeout = TNT4882_DELAY_CNT;  timeout != 0;  timeout--) {
        if(TNT4882_RDY1_GET() != 0) { break; }
        TNT4882_DELAY_50NS();       // Waitting RDY1 to 1
    }
    data = TNT4882_DATA_IN();       //lint !e734  // DATA[]
    TNT4882_RDN_SET(1);             // RDN
    TNT4882_CSN_SET(1);             // CSN

    return( data );
}


/**
 * TNT4882 Switch to Talker
 *
 * @param[in]  len  Length of talker
 */
static void TNT4882_ToTalker(uint16_t len)
{
 // TNT4882_RegWrite(TNT4882_IMR3, TNT4882_IMR3_BASE);

    // 1. Wait for the GPIB Controller to complete the necessary GPIB addressing

    // 2. Reset the FIFO
    TNT4882_RegWrite(TNT4882_CMDR, TNT4882_CMDR_RESET_FIFO);
    TNT4882_DELAY_50NS();

    // 3. Write the proper value to the Configuration Register (CFG) to establish the condition for the transfer
    TNT4882_RegWrite(TNT4882_CFG, 0x48);

    // 4. Load the two's complement of the GPIB transfer count into the Count Registers (CNTs)
    TNT4882_RegWrite(TNT4882_CNT0, (~len + 1)     ); //lint !e734
    TNT4882_RegWrite(TNT4882_CNT1, (~len + 1) >> 8); //lint !e734 !e702

    // 5. ISA pin configuration only

    // 6. Enable the desired interrupt bits in IMR0, IMR1, and IMR2
    TNT4882_RegWrite(TNT4882_IMR2, TNT4882_IMR2_ADSC_IE); // Add Change
    TNT4882_RegWrite(TNT4882_IMR1, TNT4882_IMR1_ERR_IE);  // Trans ERR
    TNT4882_RegWrite(TNT4882_IMR0, TNT4882_IMR0_TO_IE);   // byte TimeOut
    TNT4882_RegWrite(TNT4882_AUXMR, 0xFB);                // Set TimeOut Value 1s
    TNT4882_DELAY_50NS();

    // 7. Send the GO command
    TNT4882_RegWrite(TNT4882_CMDR, TNT4882_CMDR_GO);
    TNT4882_DELAY_50NS();

    // 8. Enable the appropriate interrupts in IMR3
 // TNT4882_RegWrite(TNT4882_IMR3, TNT4882_IMR3_TLC_IE1 | TNT4882_IMR3_NFF_IE1 | TNT4882_IMR3_DONE_IE1);
    TNT4882_RegWrite(TNT4882_IMR3, TNT4882_IMR3_TLC_IE1);

    // 9. ISA pin configuration only
}


/**
 * TNT4882 Switch to Listener
 */
static void TNT4882_ToListen(void)
{
 // TNT4882_RegWrite(TNT4882_IMR3, TNT4882_IMR3_BASE);

    // 1. Wait for the GPIB Controller to complete the necessary GPIB addressing

    // 2. Reset the FIFO
    TNT4882_RegWrite(TNT4882_CMDR, TNT4882_CMDR_RESET_FIFO);
    TNT4882_DELAY_50NS();

    // 3. Write the proper value to the Configuration Register (CFG) to establish the condition for the transfer
    TNT4882_RegWrite(TNT4882_CFG, 0x60);

    // 4. Load the two's complement of the GPIB transfer count into the Count Registers (CNTs)
    TNT4882_RegWrite(TNT4882_CNT0, 0x00);
    TNT4882_RegWrite(TNT4882_CNT1, 0x00);

    // 5. ISA pin configuration only

    // 6. Enable the desired interrupt bits in IMR0, IMR1, and IMR2
    TNT4882_RegWrite(TNT4882_IMR2, TNT4882_IMR2_ADSC_IE); // Add Change
    TNT4882_RegWrite(TNT4882_IMR1, TNT4882_IMR1_ENDRX_IE | TNT4882_IMR1_DEC_IE);
    TNT4882_RegWrite(TNT4882_IMR0, TNT4882_IMR0_NL_EN);   // new line
    TNT4882_RegWrite(TNT4882_AUXMR, 0xF0);                // Set TimeOut Value 0
    TNT4882_DELAY_50NS();

    // HANDSHAKE MODE
    TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_ONALLDATA_HANDSHAKE_MODE);
    TNT4882_DELAY_50NS();

    // 7. Send the GO command
    TNT4882_RegWrite(TNT4882_CMDR, TNT4882_CMDR_GO);
    TNT4882_DELAY_50NS();

    // 8. Enable the appropriate interrupts in IMR3
 // TNT4882_RegWrite(TNT4882_IMR3,TNT4882_IMR3_TLC_IE1 | TNT4882_IMR3_NEF_IE1 | TNT4882_IMR3_DONE_IE1);
    TNT4882_RegWrite(TNT4882_IMR3,TNT4882_IMR3_TLC_IE1 | TNT4882_IMR3_NEF_IE1);

    // 9. ISA pin configuration only
}


/**
 * TNT4882 Interrupt Init
 */
static void TNT4882_IntrInit(void)
{
    unsigned  pin = POSITION_VAL(TNT4882_INTR_Pin);

    // Initialize GPIO Pin for EXIT
    GPIO_PIN_INIT(TNT4882_INTR_GPIO_Port, TNT4882_INTR_Pin, GPIO_M_IN | GPIO_P_PU);

    // Config the source input GPIO Port for EXTI
  #if defined(STM32F1)
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
    #if(TNT4882_F1_PIN_SWJ_CFG != 0)
    MODIFY_REG(AFIO->MAPR, 0x07 << 24, TNT4882_F1_PIN_SWJ_CFG << 24);
    #endif
    MODIFY_REG(AFIO->EXTICR[pin / 4],   0x0Fu << ((pin % 4) * 4)
               ,     TNT4882_INTR_SOURCE_Port << ((pin % 4) * 4)
              );    //lint !e835 !e845
  #elif defined(STM32F2) || defined(STM32F4)
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
    MODIFY_REG(SYSCFG->EXTICR[pin / 4], 0x0Fu << ((pin % 4) * 4)
               ,     TNT4882_INTR_SOURCE_Port << ((pin % 4) * 4)
              );    //lint !e415 !e845
  #else   // STM32F1
  #error "Unsupported stm32 family."
  #endif  // STM32F1

    // Config the type for EXTI
    SET_BIT  (EXTI->IMR,  TNT4882_INTR_Pin);    // Enable  Interrupt mask
    CLEAR_BIT(EXTI->EMR,  TNT4882_INTR_Pin);    // Disable Event mask
    SET_BIT  (EXTI->RTSR, TNT4882_INTR_Pin);    // Enable  Rising  trigger
    CLEAR_BIT(EXTI->FTSR, TNT4882_INTR_Pin);    // Disable Falling trigger
    WRITE_REG(EXTI->PR,   TNT4882_INTR_Pin);    // Clear Interrupt Pending Bit

    // Config NVIC
    NVIC_SetPriority(TNT4882_INTR_IRQ_Num, TNT4882_INTR_NVIC_PRIO);
    NVIC_EnableIRQ  (TNT4882_INTR_IRQ_Num);
}


/**
 * This function handles interrupt request for EXTI.
 */
void TNT4882_IntrFunction(void)
{
    if (READ_BIT (EXTI->PR, TNT4882_INTR_Pin) != 0) { // Exist Interrupt Pending Bit
        WRITE_REG(EXTI->PR, TNT4882_INTR_Pin);        // Clear Interrupt Pending Bit
        TNT4882_IntrHandler();                        // Call Interrupt Handler Function
    }
}


/**
 * TNT4882 Interrupt Handler Function
 */
static void TNT4882_IntrHandler(void)
{
    uint16_t  cnt;
    uint8_t   tmp;
    uint8_t   ISR0 = TNT4882_RegRead(TNT4882_ISR0);
    uint8_t   ISR1 = TNT4882_RegRead(TNT4882_ISR1);
    uint8_t   ISR2 = TNT4882_RegRead(TNT4882_ISR2);
    uint8_t   ISR3 = TNT4882_RegRead(TNT4882_ISR3);
    uint8_t   ADSR = TNT4882_RegRead(TNT4882_ADSR);
    uint8_t   STS1 = TNT4882_RegRead(TNT4882_STS1);
    uint8_t   STS2 = TNT4882_RegRead(TNT4882_STS2);
    ((void)ISR2);  ((void)ISR3);

    LOG_DEBUG("-------- Enter Interrupt --------");
    LOG_DEBUG( "ISR0: 0x%02X, ISR1: 0x%02X, ISR2: 0x%02X, ISR3: 0x%02X, ADSR: 0x%02X, STS1: 0x%02X, STS2: 0x%02X"
              , ISR0,         ISR1,         ISR2,         ISR3,         ADSR,         STS1,         STS2
             );
    TNT4882_RegWrite(TNT4882_IMR3, TNT4882_IMR3_BASE);  // Disable Interrupt

    if (0x8B == (0x8B & STS1))
    {   // DONE STOP HALT
        if ((0x20 & ISR0) && !(0x20 & STS1))
        {   // DO NL
            _TNT4882_LinkStatus = TNT4882_LINK_TALK;
            _TNT4882_TalkInit   = TNT4882_NO_MESSAGE;
            _TNT4882_TalkStatus = TNT4882_NO_MESSAGE;
            TNT4882_ToListen();     // to Listener

            LOG_DEBUG("::DONE STOP HALT, DO, NL, to Listener");
            LOG_DEBUG("-------- Exit Interrupt ---------");
            return;
        }
    }

    if (0x14 & ADSR)
    {   // LA or LPAS
        cnt = (256 * TNT4882_RegRead(TNT4882_CNT1)
                   + TNT4882_RegRead(TNT4882_CNT0)
              );
        if (0x04 & ADSR)
        {   // LA
            LOG_DEBUG("::LA. CNT: %d | %d, FIFO: %s"
                      , (int)((int16_t)cnt)
                      , (int)_TNT4882_rxPos
                      , ((STS2 & 0x01) ? "Not Empty" : "Empty")
                     );
            if ((0 == cnt) && !(ISR1 & TNT4882_ISR1_END_RX))
            {  // FIFO Empty
                TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_FINISH_HANDSHAKE);
                TNT4882_DELAY_50NS();   // rhdf
                LOG_DEBUG("  no listen data, rhdf");
            }
            _TNT4882_LinkStatus = TNT4882_LINK_LISTEN;
            _TNT4882_TalkInit   = TNT4882_NO_MESSAGE;
            _TNT4882_TalkStatus = TNT4882_NO_MESSAGE;
            // 不能直接rhdf,会导致无法接收完整数据
        }
        else// if (0x10 & ADSR)
        {   // LPAS
            LOG_DEBUG("::LPAS. CNT: %d | %d, FIFO: %s"
                      , (int)((int16_t)cnt)
                      , (int)_TNT4882_rxPos
                      , ((STS2 & 0x01) ? "Not Empty" : "Empty")
                     );
            if ((0 == cnt) && !(ISR1 & TNT4882_ISR1_END_RX))
            {  // FIFO Empty
                TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_FINISH_HANDSHAKE);
                TNT4882_DELAY_50NS();   // rhdf
                LOG_DEBUG("  no listen data, rhdf");
            }
        }
    }

    if (STS2 & 0x01)
    {   // FIFO Not Empty
        TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_HOLDOFF_HANDSHAKE);
        TNT4882_DELAY_50NS();           // hldi

        cnt = (256 * TNT4882_RegRead(TNT4882_CNT1)
                   + TNT4882_RegRead(TNT4882_CNT0)
              );
        if ((cnt > 0) && (TNT4882_LINK_LISTEN == _TNT4882_LinkStatus) && (_TNT4882_rxBuf != NULL))
        {
            while(((_TNT4882_rxPos - _TNT4882_rxPrv) < cnt) && (_TNT4882_rxPos < _TNT4882_rxLen))
            {
                tmp = TNT4882_RegRead(TNT4882_FIFOB);
                LOG_DEBUG("  Rx data: %c <0x%02X>, CNT: %d <- %d"
                          , (char)(isprint(tmp) ? tmp : ' '), (unsigned)tmp
                          , (int)((int16_t)cnt), (unsigned)_TNT4882_rxPos
                         );
                _TNT4882_rxBuf[_TNT4882_rxPos++] = tmp;
            }
            if (!(ISR1 & TNT4882_ISR1_END_RX)) {
                TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_FINISH_HANDSHAKE);
                TNT4882_DELAY_50NS();   // Rlease Holdoff
                LOG_DEBUG("  rhdf");
            }
        }
        else
        {
            LOG_DEBUG("  Rx data, but discarded");
            TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_FINISH_HANDSHAKE);
            TNT4882_DELAY_50NS();       // Rlease Holdoff
            TNT4882_RegWrite(TNT4882_CMDR, TNT4882_CMDR_GO);
            TNT4882_DELAY_50NS();       // Send GO Command
            LOG_DEBUG("  rhdf, GO");
        }
    }

    if (ISR1 & TNT4882_ISR1_END_RX)
    {   // END RX
        if(!(STS2 & 0x01)) {
            TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_HOLDOFF_HANDSHAKE);
            TNT4882_DELAY_50NS();       // hldi
        }
        LOG_DEBUG("::END RX");

        // 防止数据还未读完就检测到EOS信号
        cnt = (256 * TNT4882_RegRead(TNT4882_CNT1)
                   + TNT4882_RegRead(TNT4882_CNT0)
              );
     // if ((_TNT4882_rxBuf != NULL) && (_TNT4882_rxPos > 1) && ('\n' != _TNT4882_rxBuf[_TNT4882_rxPos - 1]))
        if ((cnt > 0) && (TNT4882_LINK_LISTEN == _TNT4882_LinkStatus) && (_TNT4882_rxBuf != NULL))
        {
            while(((_TNT4882_rxPos - _TNT4882_rxPrv) < cnt) && (_TNT4882_rxPos < _TNT4882_rxLen))
            {
                tmp = TNT4882_RegRead(TNT4882_FIFOB);
                LOG_DEBUG("  Rx data: %c <0x%02X>, CNT: %d <- %d"
                          , (char)(isprint(tmp) ? tmp : ' '), (unsigned)tmp
                          , (int)((int16_t)cnt), (unsigned)_TNT4882_rxPos
                         );
                _TNT4882_rxBuf[_TNT4882_rxPos++] = tmp;
            }
        }

        LOG_DEBUG("  Event Callback: ListenEnd -- %d", (int)_TNT4882_rxPos);
        if(_TNT4882_ListenEnd != NULL)  _TNT4882_ListenEnd();
        _TNT4882_rxPrv = _TNT4882_rxPos;    // 保存发送事件时接收数据长度
      //_TNT4882_rxPos = 0;                 // --- 用于未能即时处理接收数据时可继续接收
      //_TNT4882_rxBuf = NULL;              // --- 修复上位机两个 write() 操作间无延时 BUG

        // Clear FIFO and CNT
        TNT4882_RegWrite(TNT4882_CMDR, TNT4882_CMDR_RESET_FIFO);
        TNT4882_DELAY_50NS();
        TNT4882_RegWrite(TNT4882_CNT0, 0x00);
        TNT4882_RegWrite(TNT4882_CNT1, 0x00);

        if (!(0x0A & ADSR)) {
            TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_FINISH_HANDSHAKE);
            TNT4882_DELAY_50NS();       // Rlease Holdoff
            LOG_DEBUG("  rhdf");
        }
    }

    if (0x0A & ADSR)
    {
        if((0x02 & ADSR) && !(0x20 & ADSR)) // 2021.4.17: No BIT SPMS -- Talker Extended (TE)
        {   // TA
            _TNT4882_LinkStatus = TNT4882_LINK_TALK;
            LOG_DEBUG("::TA");

            if(TNT4882_NO_MESSAGE == _TNT4882_TalkStatus)
            {
                LOG_DEBUG("  Event Callback: TalkEnable");
                _TNT4882_TalkStatus = TNT4882_TX_MESSAGE;
                if(_TNT4882_TalkEnable != NULL)  _TNT4882_TalkEnable();
            }
            TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_FINISH_HANDSHAKE);
            TNT4882_DELAY_50NS();       // Rlease Holdoff
            LOG_DEBUG("  rhdf");
        }
        else// if (0x08 & ADSR)
        {   // TPAS
            TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_FINISH_HANDSHAKE);
            TNT4882_DELAY_50NS();       // Rlease Holdoff
            LOG_DEBUG("::TPAS, rhdf");
        }
    }

    if (ISR1 & TNT4882_ISR1_DEC_RX)
    {   // DEC RX
        TNT4882_RegWrite(TNT4882_CMDR, TNT4882_CMDR_GO);
        TNT4882_DELAY_50NS();           // Send GO Command
        LOG_DEBUG("::DEC RX, GO");
    }

    if (ISR1 & TNT4882_ISR1_ERR_RX)
    {   // Talk ERR
        _TNT4882_LinkStatus = TNT4882_LINK_TALK;
        _TNT4882_TalkInit   = TNT4882_NO_MESSAGE;
        _TNT4882_TalkStatus = TNT4882_NO_MESSAGE;
        TNT4882_ToListen();             // to listener

        LOG_DEBUG("::Talk ERR, to Listener");
        LOG_DEBUG("-------- Exit Interrupt ---------");
        return;
    }

    if (ISR0 & TNT4882_ISR0_TO_RX)
    {   // Talk 1s Time Out
        TNT4882_RegWrite(TNT4882_TIMER, 0x00);  // Clear Time Out
        _TNT4882_LinkStatus = TNT4882_LINK_TALK;
        _TNT4882_TalkInit   = TNT4882_NO_MESSAGE;
        _TNT4882_TalkStatus = TNT4882_NO_MESSAGE;
        TNT4882_ToListen();         // to Listener

        LOG_DEBUG("::Talk 1s Time Out, to Listener");
        LOG_DEBUG("-------- Exit Interrupt ---------");
        return;
    }

    // Enable, FIFO NotEmpty
    if(/*(TNT4882_LINK_LISTEN == _TNT4882_LinkStatus) && */(_TNT4882_rxBuf != NULL) && (_TNT4882_rxPos < _TNT4882_rxLen)) {
        TNT4882_RegWrite(TNT4882_IMR3, TNT4882_IMR3_TLC_IE1 | TNT4882_IMR3_NEF_IE1);
    } else {
        TNT4882_RegWrite(TNT4882_IMR3, TNT4882_IMR3_TLC_IE1);
    }
    LOG_DEBUG("-------- Exit Interrupt ---------");
    return;
}


/**
 * TNT4882 Begin Send Data
 *
 * @param[in]  data  Pointer of data buffer
 * @param[in]  len   Length  of data buffer
 *
 * @retval  (>= 0) Number of data to be transmitted
 * @retval  ( < 0) Failed
 */
int TNT4882_SendBegin(uint8_t *data, int len)
{
    uint32_t    timeout;
    uint16_t    cnt1, cnt2;
    uint8_t     tmp;

    if ((data == NULL) || (len <= 0))
    {   // 结束 talker
        if (TNT4882_TX_MESSAGE == _TNT4882_TalkStatus)
        {
            NVIC_DisableIRQ(TNT4882_INTR_IRQ_Num);
            {
                _TNT4882_TalkInit   =  TNT4882_NO_MESSAGE;
                _TNT4882_TalkStatus =  TNT4882_NO_MESSAGE;
                _TNT4882_txBuf      =  NULL;
                _TNT4882_txLen      =  0;
                _TNT4882_txPos      =  0;
                TNT4882_ToListen();     // to listener
            }
            NVIC_EnableIRQ(TNT4882_INTR_IRQ_Num);
            LOG_DEBUG("::Is Talker, But No Tx Data, to Listener");
        }
        return( 0 );
    }

    for(timeout = TNT4882_PEND_TIME;  TNT4882_TX_MESSAGE != _TNT4882_TalkStatus;)
    {   // Waitting to talker
        if (--timeout == 0) {
            LOG_DEBUG("::Timeout of waitting for entry Talker");
            return( -1 );               // time out
        }
        DelayMS(1);
    }

    if (TNT4882_NO_MESSAGE == _TNT4882_TalkInit)
    {
        NVIC_DisableIRQ(TNT4882_INTR_IRQ_Num);
        {
            TNT4882_ToTalker(len);      //lint !e734  // to talker
            _TNT4882_TalkInit = TNT4882_TX_MESSAGE;
            _TNT4882_txBuf    = data;
            _TNT4882_txLen    = len;    //lint !e734
            _TNT4882_txPos    = 0;
        }
        NVIC_EnableIRQ(TNT4882_INTR_IRQ_Num);
        LOG_DEBUG("::TxLength Total: %d", (unsigned)len);
    }

    for(;  _TNT4882_txPos < _TNT4882_txLen;  _TNT4882_txPos++)
    {   // Send data
        if (TNT4882_TX_MESSAGE != _TNT4882_TalkStatus) {
            break;
        }
        NVIC_DisableIRQ(TNT4882_INTR_IRQ_Num);
        {
            cnt1 = (256 * TNT4882_RegRead(TNT4882_CNT1)
                        + TNT4882_RegRead(TNT4882_CNT0)
                   );
            tmp  = _TNT4882_txBuf[_TNT4882_txPos];
            TNT4882_RegWrite(TNT4882_FIFOB, tmp);

            for(timeout = TNT4882_PEND_TIME;  timeout != 0;  timeout--)
            {   // 2s等待本次数据发送完成
                cnt2 = (256 * TNT4882_RegRead(TNT4882_CNT1)
                            + TNT4882_RegRead(TNT4882_CNT0)
                       );
                if(cnt1 != cnt2)  break;

                NVIC_EnableIRQ(TNT4882_INTR_IRQ_Num);
                DelayMS(1);
                NVIC_DisableIRQ(TNT4882_INTR_IRQ_Num);
            }
        }
        NVIC_EnableIRQ(TNT4882_INTR_IRQ_Num);

        LOG_DEBUG("  Tx data: %c <0x%02X>, CNT: %d -> %d"
                  , (char)(isprint(tmp) ? tmp : ' '), (unsigned)tmp
                  , (int)((int16_t)cnt1),  (int)((int16_t)cnt2)
                 );
    }

    NVIC_DisableIRQ(TNT4882_INTR_IRQ_Num);
    {
        _TNT4882_TalkInit   = TNT4882_NO_MESSAGE;
        _TNT4882_TalkStatus = TNT4882_NO_MESSAGE;
        _TNT4882_txBuf      = NULL;
    }
    NVIC_EnableIRQ(TNT4882_INTR_IRQ_Num);
    return( len );
}


/**
 * TNT4882 Waitted for Send Data to completed
 *
 * @retval  (>= 0) Number of data transmitted
 * @retval  ( < 0) Failed
 */
int TNT4882_SendWait(void)
{
    uint32_t    timeout;

    for(timeout = TNT4882_PEND_TIME;  _TNT4882_txBuf != NULL;)
    {   // Waitting for END TX
        if (--timeout == 0) {
            LOG_DEBUG("::Timeout of Waitting for Send Data to completed");
            return( -1 );   // time out
        }
        DelayMS(1);
    }
    return( _TNT4882_txPos );
}


/**
 * TNT4882 Begin Receive Data
 *
 * @param[in]  data  Pointer of data buffer
 * @param[in]  len   Length  of data buffer
 *
 * @retval  (>= 0) Number of data to be received
 * @retval  ( < 0) Failed
 */
int TNT4882_RecvBegin(uint8_t *data, int len)
{
    uint32_t    timeout;

    if ((data == NULL) || (len <= 0)) {
        return( -1 );
    }
    for(timeout = TNT4882_PEND_TIME;  _TNT4882_rxBuf != NULL;)
    {   // Waitting for END RX
        if (--timeout == 0) {
            LOG_DEBUG("::Timeout of Begin Receive Data");
            return( -1 );   // time out
        }
        DelayMS(1);
    }
    if (timeout > (TNT4882_PEND_TIME - 20)) {
        DelayMS(20);        // 修复上位机两个 write() 操作间无延时 BUG
    }
    LOG_DEBUG("::Begin Receive Data, Max Length: %d", len);

    NVIC_DisableIRQ(TNT4882_INTR_IRQ_Num);
    {
        _TNT4882_rxBuf = data;
        _TNT4882_rxLen = len;   //lint !e734
        _TNT4882_rxPos = 0;
        _TNT4882_rxPrv = 0;
        TNT4882_RegWrite(TNT4882_IMR3, TNT4882_RegRead(TNT4882_IMR3) | TNT4882_IMR3_NEF_IE1);
    }
    NVIC_EnableIRQ(TNT4882_INTR_IRQ_Num);
    return( len );
}


/**
 * TNT4882 Waitted for Receive Data to completed
 *
 * @retval  (>= 0) Number of data received
 * @retval  ( < 0) Failed
 */
int TNT4882_RecvWait(void)
{
    int         rxLen;
    uint32_t    timeout;

    for(timeout = TNT4882_PEND_TIME;  _TNT4882_rxPos == 0;)
    {   // Waitting for END RX
        if (--timeout == 0) {
            LOG_DEBUG("::Timeout of Waitting for Receive Data to completed");
            return( -1 );   // time out
        }
        DelayMS(1);
    }
    if (timeout > (TNT4882_PEND_TIME - 20)) {
        DelayMS(20);        // 修复上位机两个 write() 操作间无延时 BUG
    }
    NVIC_DisableIRQ(TNT4882_INTR_IRQ_Num);
    rxLen           = (int)_TNT4882_rxPos;
    _TNT4882_rxPos  = 0;
    _TNT4882_rxPrv  = 0;
    _TNT4882_rxBuf  = NULL;
    NVIC_EnableIRQ(TNT4882_INTR_IRQ_Num);
    LOG_DEBUG("::Received Data, Length: %d", rxLen);

    return( rxLen );
}


/**
 * TNT4882 Init
 *
 * @param[in]  GpibAddr  GPIB address
 *
 * @retval  (== 0) Success
 * @retval  ( < 0) Failed
 */
int TNT4882_Init(uint8_t GpibAddr)
{
    TNT4882_IntrInit();     // Interrupt Init
    TNT4882_PinInit();      // Pins Init
    TNT4882_HardRst();      // hardware Reset
    NVIC_DisableIRQ(TNT4882_INTR_IRQ_Num);

    // 1. Reset the Turbo488 Circuitry of the TNT4882
    TNT4882_RegWrite(TNT4882_CMDR, TNT4882_CMDR_SOFT_RESET);
    TNT4882_DELAY_50NS();

    TNT4882_RegWrite(TNT4882_IMR3, 0x55);
    TNT4882_DELAY_50NS();
    TNT4882_DELAY_50NS();
    TNT4882_RegRead(TNT4882_IMR3);  //lint !e534
    TNT4882_DELAY_50NS();
    TNT4882_DELAY_50NS();
    if (TNT4882_RegRead(TNT4882_IMR3) != 0x55) {
        return( -1 );
    }
    TNT4882_RegWrite(TNT4882_IMR3, TNT4882_IMR3_BASE);

    // 2. Place the TNT4882 in Turbo+7210 Mode
    TNT4882_RegWrite(TNT4882_SPMR , 0x80);
    TNT4882_RegWrite(TNT4882_AUXMR, 0x80);
    TNT4882_DELAY_50NS();
    TNT4882_RegWrite(TNT4882_SPMR , 0x99);
    TNT4882_RegWrite(TNT4882_AUXMR, 0x99);
    TNT4882_DELAY_50NS();

    // 3. Configure the TNT4882 for One-Chip Mode
    TNT4882_RegWrite(TNT4882_HSSEL, 0x01);

    // 4. Make Sure that the Local Power-On Message is Asserted
    TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_CHIP_RESET);
    TNT4882_DELAY_50NS();

    // Enable T1 Delay
    TNT4882_RegWrite(TNT4882_KEYREG, 0x20);
    TNT4882_RegWrite(TNT4882_AUXMR, 0xE0);
    TNT4882_DELAY_50NS();
    TNT4882_RegWrite(TNT4882_AUXMR, 0xA0);
    TNT4882_DELAY_50NS();

    // 5A. Set the GPIB Address (normal double)
    TNT4882_RegWrite(TNT4882_ADMR, 0x31);
    TNT4882_RegWrite(TNT4882_ADR, GpibAddr); // Primarry addr
    TNT4882_RegWrite(TNT4882_ADR, 0xE0);     // NoSecond addr

    // 5-B. Write the Initial Serial Poll Response
 // TNT4882_RegWrite(TNT4882_SPMR, 0x0);

    // 5-C. Configure the Initial Parallel Response
 // TNT4882_RegWrite(TNT4882_AUXMR, 0x70);
 // TNT4882_DELAY_50NS();

    // 5-D. Enable Interrupts (clear IE)
    TNT4882_RegWrite(TNT4882_IMR0, TNT4882_IMR0_BASE);
    TNT4882_RegWrite(TNT4882_IMR1, TNT4882_IMR1_BASE);
    TNT4882_RegWrite(TNT4882_IMR2, TNT4882_IMR2_BASE);
    TNT4882_RegWrite(TNT4882_IMR3, TNT4882_IMR3_BASE);

    // 5-E. Set the GPIB Handshake Parameters (deglitching circuits toslowest value)
    TNT4882_RegWrite(TNT4882_HIER, 0xC0);
    TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_HOLDOFF_HANDSHAKE);
    TNT4882_DELAY_50NS();       // Clear HOLDOFF

    // 6. Clear the Local Power-On Message to Begin GPIB Operation
    TNT4882_RegWrite(TNT4882_AUXMR, TNT4882_AUXMR_POWER_ON);
    TNT4882_DELAY_50NS();

    TNT4882_ToListen();         // to listener
    NVIC_EnableIRQ(TNT4882_INTR_IRQ_Num);
    return( 0 );
}


/**
 * TNT4882 Register Event Callback Function
 *
 * @param[in]  Fun_ListenEnd   Callback Function for Event of Listen End
 * @param[in]  Fun_TalkEnable  Callback Function for Event of Talk Enable
 */
void TNT4882_EventFunReg( void (*Fun_ListenEnd)(void), void (*Fun_TalkEnable)(void) )
{
    NVIC_DisableIRQ(TNT4882_INTR_IRQ_Num);
    _TNT4882_ListenEnd  = Fun_ListenEnd;
    _TNT4882_TalkEnable = Fun_TalkEnable;
    NVIC_EnableIRQ(TNT4882_INTR_IRQ_Num);
}


/******************************************************************************/
/**
 * GPIB Address Pin initialize
 */
__WEAK void GPIB_AddrInit(void)
{
#ifdef GPIB_ADDR1_Pin
    GPIB_ADDR_PIN_INIT(GPIB_ADDR1_GPIO_Port, GPIB_ADDR1_Pin);
    GPIB_ADDR_PIN_INIT(GPIB_ADDR2_GPIO_Port, GPIB_ADDR2_Pin);
    GPIB_ADDR_PIN_INIT(GPIB_ADDR3_GPIO_Port, GPIB_ADDR3_Pin);
    GPIB_ADDR_PIN_INIT(GPIB_ADDR4_GPIO_Port, GPIB_ADDR4_Pin);
    GPIB_ADDR_PIN_INIT(GPIB_ADDR5_GPIO_Port, GPIB_ADDR5_Pin);
#endif
}


/**
 * Get the GPIB Address
 *
 * @return  GPIB Address
 */
__WEAK uint8_t GPIB_AddrGet(void)
{
#ifdef GPIB_ADDR1_Pin
    uint8_t  addr = 0;

    if(GPIB_ADDR_PIN_READ(GPIB_ADDR1_GPIO_Port, GPIB_ADDR1_Pin)){ addr |= 0x01; }
    if(GPIB_ADDR_PIN_READ(GPIB_ADDR2_GPIO_Port, GPIB_ADDR2_Pin)){ addr |= 0x02; }
    if(GPIB_ADDR_PIN_READ(GPIB_ADDR3_GPIO_Port, GPIB_ADDR3_Pin)){ addr |= 0x04; }
    if(GPIB_ADDR_PIN_READ(GPIB_ADDR4_GPIO_Port, GPIB_ADDR4_Pin)){ addr |= 0x08; }
    if(GPIB_ADDR_PIN_READ(GPIB_ADDR5_GPIO_Port, GPIB_ADDR5_Pin)){ addr |= 0x10; }

    return( addr );
#else
    return( 0xFF );
#endif
}


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 *****************************  END OF FILE  **********************************/

