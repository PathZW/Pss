/******************************************************************************
 *
 * 文件名  ： PssMcuDrvsFpgaAds1259.c
 * 负责人  ： myk
 * 创建日期： 20211008
 * 版本号  ： v1.0
 * 文件描述： PSS ADS1259驱动实现
 * 其    他： 无
 * 修改日志： 无
 *
 *******************************************************************************/


/*---------------------------------- 预处理区 ---------------------------------*/

/************************************ 头文件 ***********************************/
#include <string.h>
#include "Drv_FpgaAds1259.h"

/* 2.5 / (2 ** 23 - 1) V */
#define _I32_TO_F32_STEP_                    (5.0f/8388607)

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
#define _REG_CFG2_DATA_NREADY_               (0x80)
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

/*----------------------------------- 声明区 ----------------------------------*/

/********************************** 变量声明区 *********************************/

/********************************** 函数声明区 *********************************/
static int CfgAds1259Chip(PSS_MCU_DRVS_ADS1259_T *ads1259, uint8_t dr);
static void BitsToF32(float *val, uint32_t Data);

static void CmdReset(PSS_MCU_DRVS_ADS1259_T *ads1259);
static void CmdSdatac(PSS_MCU_DRVS_ADS1259_T *ads1259);
static void CmdStart(PSS_MCU_DRVS_ADS1259_T *ads1259);

/********************************** 变量实现区 *********************************/
uint32_t vAdcOrigin, iAdcOrigin;
/********************************** 函数实现区 *********************************/
int PssMcuDrvsAds1259Init(PSS_MCU_DRVS_ADS1259_T *ads1259, uint32_t addr)
{
    /* step0: 初始化 */
    ads1259->ad1259Hw = (Ad1259_TypeDef*)addr;

    /* step1: 写入并校验初始配置 */
    if(CfgAds1259Chip(ads1259, PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_3600SPS) == 0)
    {
        /* step2: 初始化成功,FPGA控制开始采样 */
        ads1259->ad1259Hw->InitSuccessReg = 1;
    }
    return -1;
}

int PssMcuDrvsAds1259Update(PSS_MCU_DRVS_ADS1259_T *ads1259)
{
    return 0;
}

int PssMcuDrvsAds1259Read(PSS_MCU_DRVS_ADS1259_T *ads1259,  float *vVal)
{
    Ad1259_TypeDef *ad1259Hw  = ads1259->ad1259Hw;

    /* step0: 启动采集 */
    CmdStart(ads1259);

    /* step1: 等待数据 */
    while(!(ad1259Hw->SpiSendReg & _SPI_RECV_FLAG));
    ads1259->originData = (uint32_t)(ad1259Hw->realDataReg);
    ad1259Hw->ClearSpiReg &= (~_SPI_RECV_FLAG);

    /* 转换为浮点型数据 */
    BitsToF32(vVal, ads1259->originData);
    return ads1259->originData;
}

int PssMcuDrvsAds1259ReadOriginData(PSS_MCU_DRVS_ADS1259_T *ads1259,  uint32_t *vVal)
{
    *vVal = ads1259->originData;
    return 0;
}


int CfgAds1259Chip(PSS_MCU_DRVS_ADS1259_T *ads1259, uint8_t dr)
{
    _PARA_T_ wrPara = {0};
    Ad1259_TypeDef* ad1259Hw = ads1259->ad1259Hw;

    /* step0: 给定芯片的配置 */
    wrPara.rbias  = _REG_CFG0_RBIAS_DISABLE_;
    wrPara.spi    = _REG_CFG0_SPI_TO_ENABLE_;
    wrPara.flag   = _REG_CFG1_FLAG_ENABLE_;
    wrPara.chksum = _REG_CFG1_CHKSUM_ENABLE_;
    wrPara.sinc2  = _REG_CFG1_SINC2_DISABLE_;
    wrPara.extref = _REG_CFG1_EXTREF_EXTERNAL_;
    wrPara.delay  = _REG_CFG1_DELAY_NO_DELAY_;
    wrPara.syncout= _REG_CFG2_SYNCOUT_DISABLE_;
    wrPara.pulse  = _REG_CFG2_MODE_PULSE_;
    wrPara.dr     = dr;

    CmdReset(ads1259);
    CmdSdatac(ads1259);

    uint8_t op1;                                                  /* 操作地址 */
    uint8_t op2 = 0;                                              /* 操作数固定为0 */
    uint8_t rCfg = 0x00;
    uint8_t cfg0 = 0x00;
    uint8_t cfg1 = 0x00;
    uint8_t cfg2 = 0x00;

    /* step1: 读写配置验证 */

    /* 写配置1 0x21 */
    op1 = _CMD_WREG_0;
    cfg0 |= wrPara.rbias  << _REG_CFG0_RBIAS_OFFSET_;
    cfg0 |= wrPara.spi    << _REG_CFG0_SPI_TO_OFFSET_;
    cfg0 |= 0x20;
    ad1259Hw->cmdReg = op1<<16 | op2<<8 | cfg0;

    /* TODO: 超时退出*/
    while(!(ad1259Hw->SpiSendReg & _SPI_SEND_FLAG));
    ad1259Hw->ClearSpiReg &= (~_SPI_SEND_FLAG);

    /* 读配置1 */
    op1 = _CMD_RREG_0;
    ad1259Hw->cmdReg = op1<<16 | op2<<8 | rCfg;

    while(!(ad1259Hw->SpiSendReg & _SPI_RECV_FLAG));
    rCfg = (uint8_t)(ad1259Hw->ReadBackReg);
    ad1259Hw->ClearSpiReg &= (~_SPI_RECV_FLAG);
    if(rCfg != cfg0)
    {
        return -1;
    }

    /* 写配置2 0xC8*/
    op1 = _CMD_WREG_1;
    cfg1 |= wrPara.flag   << _REG_CFG1_FLAG_OFFSET_;
    cfg1 |= wrPara.chksum << _REG_CFG1_CHKSUM_OFFSET_;
    cfg1 |= wrPara.sinc2  << _REG_CFG1_SINC2_OFFSET_;
    cfg1 |= wrPara.extref << _REG_CFG1_EXTREF_OFFSET_;
    cfg1 |= wrPara.delay  << _REG_CFG1_DELAY_OFFSET_;
    ad1259Hw->cmdReg = op1<<16 | op2<<8 | cfg1;
    while(!(ad1259Hw->SpiSendReg & _SPI_SEND_FLAG));
    ad1259Hw->ClearSpiReg &= (~_SPI_SEND_FLAG);

    /* 读配置2 */
    op1 = _CMD_RREG_1;
    cfg0 = 0;
    ad1259Hw->cmdReg = op1<<16 | op2<<8 | rCfg;
    /* TODO: 超时退出*/
    while(!(ad1259Hw->SpiSendReg & _SPI_RECV_FLAG));
    rCfg = (uint8_t)(ad1259Hw->ReadBackReg);
    ad1259Hw->ClearSpiReg &= (~_SPI_RECV_FLAG);
    if(rCfg != cfg1)
    {
        return -1;
    }

    /* 写配置3 0x21*/
    op1 = _CMD_WREG_2;
    cfg2 |= wrPara.syncout<< _REG_CFG2_SYNCOUT_OFFSET_;
    cfg2 |= wrPara.pulse  << _REG_CFG2_MODE_OFFSET_;
    cfg2 |= wrPara.dr     << _REG_CFG2_DR_OFFSET_;
    cfg2 |= _REG_CFG2_DATA_NREADY_;
    ad1259Hw->cmdReg = op1<<16 | op2<<8 | cfg2;
    while(!(ad1259Hw->SpiSendReg & _SPI_SEND_FLAG));
    ad1259Hw->ClearSpiReg &= (~_SPI_SEND_FLAG);

    /* 读配置3 */
    op1 = _CMD_RREG_2;
    cfg0 = 0;
    ad1259Hw->cmdReg = op1<<16 | op2<<8 | rCfg;
    /* TODO: 超时退出*/
    while(!(ad1259Hw->SpiSendReg & _SPI_RECV_FLAG));
    rCfg = (uint8_t)(ad1259Hw->ReadBackReg);
    ad1259Hw->ClearSpiReg &= (~_SPI_RECV_FLAG);

    if(rCfg != cfg2)                                         /* 比较读写配置字节 */
    {
        return -1;
    }

    return 0;
}

void PssMcuDrvsAds1259SetDataRate(PSS_MCU_DRVS_ADS1259_T *ads1259, PSS_MCU_DRVS_M1_ADS1259_DATA_RATE_ENUM dr)
{
    CfgAds1259Chip(ads1259, dr);
}

static void CmdSdatac(PSS_MCU_DRVS_ADS1259_T *ads1259)
{
    Ad1259_TypeDef *ad1259Hw = ads1259->ad1259Hw;
    uint32_t cmdSdatac = 0;

    cmdSdatac |= _CMD_SDATAC_ << 16;
    ad1259Hw->cmdReg = cmdSdatac;
    while(!(ad1259Hw->SpiSendReg & _SPI_SEND_FLAG));
    ad1259Hw->ClearSpiReg &= (~_SPI_SEND_FLAG);
}

static void CmdStart(PSS_MCU_DRVS_ADS1259_T *ads1259)
{
    Ad1259_TypeDef *ad1259Hw = ads1259->ad1259Hw;
    uint32_t CmdStart = 0;
    CmdStart |= _CMD_START_ << 16;
    ad1259Hw->cmdReg = CmdStart;
    while(!(ad1259Hw->SpiSendReg & _SPI_SEND_FLAG));
    ad1259Hw->ClearSpiReg &= (~_SPI_SEND_FLAG);
}

static void CmdReset(PSS_MCU_DRVS_ADS1259_T *ads1259)
{
    Ad1259_TypeDef *ad1259Hw = ads1259->ad1259Hw;
    uint32_t cmdReset = 0;
    cmdReset |= _CMD_RESET_ << 16;
    ad1259Hw->cmdReg = cmdReset;
    while(!(ad1259Hw->SpiSendReg & _SPI_SEND_FLAG));
    ad1259Hw->ClearSpiReg &= (~_SPI_SEND_FLAG);
}

static void BitsToF32(float *val, uint32_t Data)
{
    uint8_t msb = (uint8_t)(Data >> 24);
    uint8_t mid = (uint8_t)(Data >> 16);
    uint8_t lsb = (uint8_t)(Data >> 8);
    uint8_t chksum = (uint8_t)(Data);
    int i32Val = 0;
    int i32Temp = 0;
    uint8_t chksumCompute = 0;
    uint8_t chksumWithoutOOR = 0;

    /* 校验 */
    chksumCompute = msb + mid + lsb + 0x9B;
    /* 处理out-of-range bit */
    chksumCompute &= 0x7f;
    chksumWithoutOOR = 0x7f & chksum;
    if(chksumWithoutOOR != chksumCompute) /* 校验失败 */
    {
        *val = 0.0f;
        return;
    }
    /* 转换为浮点数 */
    i32Temp |= lsb;
    i32Temp |= mid << 8;
    i32Temp |= msb << 16;

    /* 符号扩展 */
    if(0x80 == (0x80 & msb)) /* 符号位为1 */
    {
        i32Temp |= 0xff000000;
    }

    /* 无需大小端转换 */
    i32Val = i32Temp;
    *val = i32Val * _I32_TO_F32_STEP_;
}

