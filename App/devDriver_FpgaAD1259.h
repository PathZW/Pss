/******************************************************************************
 *
 * 文件名  ： PssMcuDrvsFpgaAds1259.h
 * 负责人  ： myk
 * 创建日期： 20210918
 * 版本号  ： v1.0
 * 文件描述： PSS ADS1259驱动接口
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/

/*---------------------------------- 预处理区 ---------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _PSS_MCU_DRVS_ADS1259_H_
#define _PSS_MCU_DRVS_ADS1259_H_

/************************************ 头文件 ***********************************/
#include <stdint.h>
#include <stdarg.h>
#include <emb_log.h>

/************************************ 变量区 ***********************************/
static uint32_t u32_read_AD1259_OriginData = 0;
static float    f32_int2float_val= 0;
/* 2.5 / (2 ** 23 - 1) V */
#define _I32_TO_F32_STEP_                    (5.0f/8388607)

/************************************ 静态函数定义 ***********************************/
/************************************ 宏定义 ***********************************/

#define   MCU_DRVS_ADS1259_BASE1        (0x60004000)
#define   MCU_DRVS_ADS1259_BASE2        (0x60005000)
#define   MCU_DRVS_ADS1259_BASE3        (0x60006000)


/* ADS1259命令 */
#define _CMD_WAKEUP_                         (0x02)
#define _CMD_SLEEP_                          (0x04)
#define _CMD_RESET_                          (0x06)
#define _CMD_START_                          (0x08)
#define _CMD_STOP_                           (0x0A)
#define _CMD_RDATAC_                         (0x10)
#define _CMD_SDATAC_                         (0x11)
#define _CMD_RDATA_                          (0x12)
#define _CMD_RREG_0                          (0x20)
#define _CMD_RREG_1                          (0x21)
#define _CMD_RREG_2                          (0x22)
#define _CMD_WREG_0                          (0x40)
#define _CMD_WREG_1                          (0x41)
#define _CMD_WREG_2                          (0x42)
#define _CMD_OFSCAL_                         (0x18)
#define _CMD_GANCAL_                         (0x19)
#define _CMD_CFG0_OFFSET_                    (0)
#define _CMD_CFG_NUMS_                       (3)
#define _SPI_RECV_FLAG                       (1<<0)
#define _SPI_SEND_FLAG                       (1<<1)
#define _OPT2_NUM                            (1)
/* ADS1259寄存器 */                                  //  bit7   bi6      bit5      bit4    bit3    bit2     bit1    bit0
#define _REG_CFG0_                           (0x00)  //    1       0       ID1     ID0       0     RBIAS      0     SPI
#define _REG_CFG1_                           (0x01)  //  FLAG   CHKSUM      0      SINC2   EXTREF  DELAY2   DELAY1  DELAY0
#define _REG_CFG2_                           (0x02)  //  DRDY   EXTCLK   SYNCOUT   PULSE     0     DR2      DR1     DR0
#if 0 /* 校准未使用 */
#define _REG_OFC0_                           (0x03)  //  OFC7   OFC6     OFC5      OFC4    OFC3    OFC2     OFC1    OFC0
#define _REG_OFC1_                           (0x04)  //  OFC15  OFC14    OFC13     OFC12   OFC11   OFC10    OFC9    OFC8
#define _REG_OFC2_                           (0x05)  //  OFC23  OFC22    OFC21     OFC20   OFC19   OFC18    OFC17   OFC16
#define _REG_FSC0_                           (0x06)  //  FSC7   FSC6     FSC5      FSC4    FSC3    FSC2     FSC1    FSC0
#define _REG_FSC1_                           (0x07)  //  FSC15  FSC14    FSC13     FSC12   FSC11   FSC10    FSC9    FSC8
#define _REG_FSC2_                           (0x08)  //  FSC23  FSC22    FSC21     FSC20   FSC19   FSC18    FSC17   FSC16
#endif

#define PSS_MCU_DRVS_ADS1259_V_CHANNEL                          (0)
#define PSS_MCU_DRVS_ADS1259_I_CHANNEL                          (1)
/* ADS1259 只读bits */
#define _REG_CFG0_DRDY_OFFSET_               (7)
#define _REG_CFG0_DRDY_YES_                  (0x00)
#define _REG_CFG0_DRDY_NO_                   (0x01)
#define _REG_CFG0_EXTCLK_OFFSET_             (6)
#define _REG_CFG0_EXTCLK_INOSC_              (0x00)
#define _REG_CFG0_EXTCLK_EXTCLK_             (0x01)
/* ADS1259 可读写bits */
#define _REG_CFG0_RBIAS_OFFSET_              (2)
#define _REG_CFG0_RBIAS_MASK_                (0x01)
#define _REG_CFG0_RBIAS_ENABLE_              (0x01)
#define _REG_CFG0_RBIAS_DISABLE_             (0x00)
#define _REG_CFG0_SPI_TO_OFFSET_             (0)
#define _REG_CFG0_SPI_TO_MASK_               (0x01)
#define _REG_CFG0_SPI_TO_ENABLE_             (0x01)
#define _REG_CFG0_SPI_TO_DISABLE_            (0x00)
#define _REG_CFG1_FLAG_OFFSET_               (7)
#define _REG_CFG1_FLAG_MASK_                 (0x01)
#define _REG_CFG1_FLAG_ENABLE_               (0x01)
#define _REG_CFG1_FLAG_DISABLE_              (0x00)
#define _REG_CFG1_CHKSUM_OFFSET_             (6)
#define _REG_CFG1_CHKSUM_MASK_               (0x01)
#define _REG_CFG1_CHKSUM_ENABLE_             (0x01)
#define _REG_CFG1_CHKSUM_DISABLE_            (0x00)
#define _REG_CFG1_SINC2_OFFSET_              (4)
#define _REG_CFG1_SINC2_MASK_                (0x01)
#define _REG_CFG1_SINC2_ENABLE_              (0x01)
#define _REG_CFG1_SINC2_DISABLE_             (0x00)
#define _REG_CFG1_EXTREF_OFFSET_             (3)
#define _REG_CFG1_EXTREF_MASK_               (0x01)
#define _REG_CFG1_EXTREF_EXTERNAL_           (0x01)
#define _REG_CFG1_EXTREF_INTERNAL_           (0x00)
#define _REG_CFG1_DELAY_OFFSET_              (0)
#define _REG_CFG1_DELAY_MASK_                (0x07)
#define _REG_CFG1_DELAY_NO_DELAY_            (0x00)
#define _REG_CFG1_DELAY_64_TCLK_             (0x01)
#define _REG_CFG1_DELAY_128_TCLK_            (0x02)
#define _REG_CFG1_DELAY_256_TCLK_            (0x03)
#define _REG_CFG1_DELAY_512_TCLK_            (0x04)
#define _REG_CFG1_DELAY_1024_TCLK_           (0x05)
#define _REG_CFG1_DELAY_2048_TCLK_           (0x06)
#define _REG_CFG1_DELAY_4096_TCLK_           (0x07)
#define _REG_CFG2_SYNCOUT_OFFSET_            (5)
#define _REG_CFG2_SYNCOUT_MASK_              (0x01)
#define _REG_CFG2_SYNCOUT_ENABLE_            (0x01)
#define _REG_CFG2_SYNCOUT_DISABLE_           (0x00)
#define _REG_CFG2_MODE_OFFSET_               (4)
#define _REG_CFG2_MODE_MASK_                 (0x01)
#define _REG_CFG2_MODE_PULSE_                (0x01)
#define _REG_CFG2_MODE_GATE_                 (0x00)
#define _REG_CFG2_DR_OFFSET_                 (0)
#define _REG_CFG2_DR_MASK_                   (0x07)
/*Date rate*/
#define _REG_CFG2_DATA_Rate_10PSP            (0x0)           
#define _REG_CFG2_DATA_Rate_16PSP            (0x1) //16.6
#define _REG_CFG2_DATA_Rate_50PSP            (0x2)
#define _REG_CFG2_DATA_Rate_60PSP            (0x3)
#define _REG_CFG2_DATA_Rate_400PSP           (0x4)
#define _REG_CFG2_DATA_Rate_1200PSP          (0x5)
#define _REG_CFG2_DATA_Rate_3600PSP          (0x6)
#define _REG_CFG2_DATA_Rate_14400PSP         (0x7)
#define _REG_CFG2_DATA_NREADY_               (0x80)

#define DEV_AD_1259_CMD_REG                 (dev->devRegs->cmdReg)
#define DEV_AD_1259_SPI_SEND_REG            (dev->devRegs->SpiSendReg)
#define DEV_AD_1259_SPI_CLEAR_REG           (dev->devRegs->ClearSpiReg)
#define DEV_AD_1259_INIT_SUCCES_REG         (dev->devRegs->InitSuccessReg)
#define DEV_AD_1259_SEND_CMD_OFFSET         16
#define DEV_AD_1259_CLEAR_FIFO_FLAG         1

#if 0
#define DEV_AD1259_SEND_TO_ADC(cmd) \
do{ \
	DEV_AD_1259_CMD_REG = cmd; \
	while(!(DEV_AD_1259_SPI_SEND_REG & _SPI_SEND_FLAG)); \
	DEV_AD_1259_SPI_CLEAR_REG &= (~_SPI_SEND_FLAG); \
}while(0)

//DATA_REG:DEV_AD_1259_FIFO_DATA_REG/DEV_AD_1259_REALDATA_REG
#define DEV_AD1259_READ_FROM_ADC(DATA_REG) \
({ \
	DEV_AD1259_SEND_TO_ADC(_CMD_START_<< DEV_AD_1259_SEND_CMD_OFFSET); \
	while(!(DEV_AD_1259_SPI_SEND_REG & _SPI_RECV_FLAG)); \
	u32_read_AD1259_OriginData = (uint32_t)DATA_REG; \
	DEV_AD_1259_SPI_CLEAR_REG &= (~_SPI_RECV_FLAG); \
	BitsToF32(&f32_int2float_val, u32_read_AD1259_OriginData); \
	(f32_int2float_val); \
})
#endif
/*RECV*/ 
#define DEV_AD1259RECV_FROM_SAMPREG    0 //0表示数据来源采样寄存器，，
#define DEV_AD1259RECV_FROM_CHKFIFO    1 //1表示校准值FIFO
#define DEV_AD1259RECV_CHKFIFO_NULL   ((dev->devRegs->fifoStausReg & 0x1) == 0x1)
#define DEV_AD1259RECV_CHKFIFO_FULL   ((dev->devRegs->fifoStausReg & 0x2) == 0x2)

/*********************************** 类型定义 **********************************/
typedef struct _PARA_TAG_
{
    /* 对应芯片数据手册中控制寄存器 */
    /* 只读位 */
    uint8_t id;          /* id */
    uint8_t drdy_;       /* 数据就绪 */
    uint8_t extclk;      /* 设备时钟 */

    /* 读写位 */
    uint8_t rbias;       /* 内部参考基准 */
    uint8_t spi;         /* SPI超时 */
    uint8_t flag;        /* 超范围标记 */
    uint8_t chksum;      /* 回读数据带校验 */
    uint8_t sinc2;       /* 滤波选择 */
    uint8_t extref;      /* 参考选择 */
    uint8_t delay;       /* 开始转换的延迟 */
    uint8_t syncout;     /* 同步信号输出使能 */
    uint8_t pulse;       /* 采样模式 */
    uint8_t dr;          /* 数据率 */
} _PARA_T_;         /* PSS ADS1259参数 */


typedef enum _PSS_MCU_NPLC_TYPE_TAG_
{
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_14400SPS          = 0x07,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_3600SPS           = 0x06,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_1200SPS           = 0x05,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_400SPS            = 0x04,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_60SPS             = 0x03,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_50SPS             = 0x02,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_16E6SPS           = 0x01,
    PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_10SPS             = 0x00,
} PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_ENUM;

typedef struct
{
    volatile  uint32_t cmdReg;                /* 命令寄存器 */
    volatile  uint32_t dataReg;               /* 采样数据FIFO寄存器 [23:0]采样值  [31:24] 数据放大倍数*/
    volatile  uint32_t realDataReg;           /* 实时采样数据寄存器*/
    volatile  uint32_t fifoStausReg;          /* FIFO状态寄存器 */
    volatile  uint32_t readDealy;             /* 采样读取延时寄存器 预留 */
    volatile  uint32_t readNum;               /* 采样读取个数寄存器 预留*/
    volatile  uint32_t fifoReset;             /* FIFO清空寄存器 */
    volatile  uint32_t watchReg;              /* 帧测寄存器 */
    volatile  uint32_t ReadBackReg;           /* 回读数据寄存器 */
    volatile  uint32_t SpiSendReg;            /* SPI发送完成寄存器 */
    volatile  uint32_t ClearSpiReg;           /* 清除SPI发送寄存器 */
    volatile  uint32_t InitSuccessReg;        /* 初始化完成寄存器 */
}FpgaAD1259_REGS;


typedef struct {
    volatile        uint32_t    mSyncStart;     // 00 同步启动采样
    volatile        uint32_t    mSyncStop;      // 04 同步关闭采样
    volatile        uint32_t    mTxInterval;    // 08 数据发送间隔
} FpgaAD4000_SYNC_REGS;

/*--------------------------------- 接口声明区 --------------------------------*/

/*********************************** 全局变量 **********************************/

/*********************************** 接口函数 **********************************/


#endif // #ifndef _PSS_MCU_DRVS_ADS1259_H_

#ifdef __cplusplus
}
#endif

