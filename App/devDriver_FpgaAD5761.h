/*******************************************************************************
 * @file    devDriver_FpgaAD5761.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/26
 * @brief   通用设备驱动 - DAC - FPGA AD5761, 可以在"devDriver_cfg.h"配置.\n
 *          引用“extern devDriver_t devDriver(FpgaAD5761, x)” x为 1 ~ 4 即可使用.
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
 *      2023/3/26 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DEV_DRIVER_FpgaAD5761_H__
#define __DEV_DRIVER_FpgaAD5761_H__
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Device_Driver
 * @{
 * @addtogroup DAC
 * @{
 * @addtogroup FpgaAD5761
 * @{
 * @addtogroup              __Config
 * @{
 ******************************************************************************/

#ifndef FpgaAD5761_1_BASE_ADDR
#define FpgaAD5761_1_BASE_ADDR      0x60000000  //!< FPGA AD5761 Chip1 地址
#undef  FpgaAD5761_1_BASE_ADDR
#endif

#ifndef FpgaAD5761_2_BASE_ADDR
#define FpgaAD5761_2_BASE_ADDR      0x60000000  //!< FPGA AD5761 Chip2 地址
#undef  FpgaAD5761_2_BASE_ADDR
#endif

#ifndef FpgaAD5761_3_BASE_ADDR
#define FpgaAD5761_3_BASE_ADDR      0x60000000  //!< FPGA AD5761 Chip3 地址
#undef  FpgaAD5761_3_BASE_ADDR
#endif

#ifndef FpgaAD5761_4_BASE_ADDR
#define FpgaAD5761_4_BASE_ADDR      0x60000000  //!< FPGA AD5761 Chip4 地址
#undef  FpgaAD5761_4_BASE_ADDR
#endif

#ifndef FpgaAD5761_FREQ
#define FpgaAD5761_FREQ      (40 * 1000 * 1000) // 时钟频率
#define FpgaAD5761_US_TO_CYC(usTime)  ((usTime) * (1000 / (1000*1000*1000 / FpgaAD5761_FREQ)))
#define FpgaAD5761_NS_TO_CYC(nsTime)  ((nsTime)         / (1000*1000*1000 / FpgaAD5761_FREQ) )
#endif



/* 5683控制寄存器 */
#define PSS_MCU_DRVS_AD5761_CTRL_REG_OFFSET         (16)
#define PSS_MCU_DRVS_AD5761_CTRL_REG_ADDR           (0x04)
#define PSS_MCU_DRVS_AD5761_WRITE_REG_ADDR          (0x01)

#define PSS_MCU_DRVS_AD5761_NUM_CTRL_OFFSET         (28)
#define PSS_MCU_DRVS_AD5761_0_MASK                  (0x01)
#define PSS_MCU_DRVS_AD5761_1_MASK                  (0x02)
#define PSS_MCU_DRVS_AD5761_2_MASK                  (0x04)
#define PSS_MCU_DRVS_AD5761_ALL_MASK                (PSS_MCU_DRVS_AD5761_0_MASK | PSS_MCU_DRVS_AD5761_1_MASK | PSS_MCU_DRVS_AD5761_2_MASK)

#define PSS_MCU_DRVS_AD5761_CMD_FIFO_FULL           (0x01)
#define PSS_MCU_DRVS_AD5761_CMD_FIFO_EMPTY          (0x02)
#define PSS_MCU_DRVS_AD5761_CMD_FIFO_BUSY           (0x04)

/* 读写位 */
#define PSS_MCU_DRVS_AD5761_CTRL_CV_OFFSET          (9)
#define PSS_MCU_DRVS_AD5761_CTRL_CV_MASK            (0x03)
#define PSS_MCU_DRVS_AD5761_CTRL_CV_ZERO            (0x00)
#define PSS_MCU_DRVS_AD5761_CTRL_CV_MID             (0x01)
#define PSS_MCU_DRVS_AD5761_CTRL_CV_FULL            (0x02)
#define PSS_MCU_DRVS_AD5761_CTRL_OVR_OFFSET         (8)
#define PSS_MCU_DRVS_AD5761_CTRL_OVR_MASK           (0x01)
#define PSS_MCU_DRVS_AD5761_CTRL_OVR_DI             (0x00)
#define PSS_MCU_DRVS_AD5761_CTRL_OVR_EN             (0x01)
#define PSS_MCU_DRVS_AD5761_CTRL_B2Z_OFFSET         (7)
#define PSS_MCU_DRVS_AD5761_CTRL_B2Z_MASK           (0x01)
#define PSS_MCU_DRVS_AD5761_CTRL_B2Z_SB             (0x00)
#define PSS_MCU_DRVS_AD5761_CTRL_B2Z_CB             (0x01)
#define PSS_MCU_DRVS_AD5761_CTRL_ETS_OFFSET         (6)
#define PSS_MCU_DRVS_AD5761_CTRL_ETS_MASK           (0x01)
#define PSS_MCU_DRVS_AD5761_CTRL_ETS_OFF            (0x00)
#define PSS_MCU_DRVS_AD5761_CTRL_ETS_ON             (0x01)
#define PSS_MCU_DRVS_AD5761_CTRL_PV_OFFSET          (3)
#define PSS_MCU_DRVS_AD5761_CTRL_PV_MASK            (0x03)
#define PSS_MCU_DRVS_AD5761_CTRL_PV_ZERO            (0x00)
#define PSS_MCU_DRVS_AD5761_CTRL_PV_MID             (0x01)
#define PSS_MCU_DRVS_AD5761_CTRL_PV_FULL            (0x02)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_OFFSET          (0)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_MASK            (0x07)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_N10_P10         (0x00)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_0_P10           (0x01)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_N5_P5           (0x02)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_0_P5            (0x03)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_N2E5_N7E5       (0x04)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_N3_P3           (0x05)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_0_P16           (0x06)
#define PSS_MCU_DRVS_AD5761_CTRL_RA_0_P20           (0x07)

#define PSS_MCU_DRVS_AD5761_RESET_FLAG              (0x01)

/* AD5761命令 */
#define _CMD_WRITE5761_INPUT_REG_                   (0x01) //通过Command2可以写入DAC_REG
#define _CMD_UPDATA5761_DAC_REG_                    (0x02)
#define _CMD_WRITE5761_DAC_REG_                     (0x03)
#define _CMD_WRITE5761_CTRL_REG_                    (0x04)
#define _CMD_RDBCK5761_INPUT_REG_                   (0x05)

/* AD5761 Register address */
#define _CMD_WRITE_CONTROL_REG_                      (0x4)

#define DEV_AD5761_RESET_FIFO_FLG                    (0x1)

/*send*/
#define DEV_AD5761_SETVAL_NOPULSE       0
#define DEV_AD5761_SETVAL_ISPULSE       1
#define DEV_AD5761_HIGHVAL_VALUE(val)         (val*26214)
#define MIN(a,b) ((a<b)?a:b)

#define DEV_AD5761_VAL_ESTIMATE(val) ((val < 0)?0:(DEV_AD5761_HIGHVAL_VALUE(MIN(val,2.5))))  

/*regVal最后写入REG的值，chipNum通道号，val写入的原始值，REG最终写入的寄存器*/
#define DEV_AD5761_WRITE_INPUTREG_UPDATA(chipNum,val,REG) \
do{ \
	uint32_t regVal; \
	regVal = val; \
	regVal <<= 4; \
	regVal |= (chipNum << PSS_MCU_DRVS_AD5761_NUM_CTRL_OFFSET); \
	regVal |= (_CMD_WRITE5761_DAC_REG_ << 20); \
	REG = regVal; \
}while(0)

/*FIFO state*/
#define DEV_AD5761_FIFOISFULL          		((dev->devRegs->mFIFOstat & 0x1) == 0x1)    
#define DEV_AD5761_FIFOISNULL          		((dev->devRegs->mFIFOstat & 0x2) == 0x2)
#define DEV_AD5761_FIFOISBUSY          		((dev->devRegs->mFIFOstat & 0x4) == 0x4)

/*Data Source*/
#define DEV_AD5761Send_TO_CMD_REG    0
#define DEV_AD5761Send_TO_FIFO       1
/*Data Source*/

/*******************************************************************************
 * @}
 * @addtogroup              Registers
 * @{
 ******************************************************************************/

typedef struct {
	volatile        uint32_t    mFIFOout;       // 00 输出 FIFO 写接口
	volatile const  uint32_t    mFIFOstat;      // 04 FIFO 状态; BIT0:满; BIT1:空; BIT2:忙
	volatile        uint32_t    mCmdData;       // 08 输出指令; 写入数据直接发送
	volatile        uint32_t    mFIFOreset;     // 0C FIFO 复位; 写操作复位 FIFO
	volatile const  uint32_t    mCurData;       // 10 当前数据; BIT[23..0]:数据; BIT24:极性
	volatile        uint32_t    mPolarity;      // 14 极性选择; (x)从CHIPx获取极性
	volatile        uint32_t    mDataFrom;      // 18 输出数据来源; BIT0:(0)输出指令,(1)其他
	volatile        uint32_t    mFIFOvldNum;    // 1C FIFO 有效数据个数
} FpgaAD5761_REGS;

typedef struct _PSS_MCU_DRVS_AD5761_PARA_TAG_
{
	/* 对应芯片数据手册中控制寄存器 */
	/* 只读位 */
	uint8_t sc;                          /* 短路状态 */
	uint8_t bo;                          /* 低压状态 */

	/* 读写位 */
	uint8_t cv;                          /* CLEAR电平 */
	uint8_t ovr;                         /* OVR使能 */
	uint8_t b2c;                         /* 数据极性格式 */
	uint8_t ets;                         /* 150度过温保护 */
	uint8_t pv;                          /* 启动电压 */
	uint8_t ra;                          /* 输出电压范围 */
} PSS_MCU_DRVS_AD5761_PARA_T;       /* 普塞斯 AD5683驱动 */

/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif
/*****************************  END OF FILE  **********************************/

