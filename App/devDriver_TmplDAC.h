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

