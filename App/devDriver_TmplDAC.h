/*******************************************************************************
 * @file    devDriver_FpgaAD5761.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/26
 * @brief   ͨ���豸���� - DAC - FPGA AD5761, ������"devDriver_cfg.h"����.\n
 *          ���á�extern devDriver_t devDriver(FpgaAD5761, x)�� xΪ 1 ~ 4 ����ʹ��.
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
#define FpgaAD5761_1_BASE_ADDR      0x60000000  //!< FPGA AD5761 Chip1 ��ַ
#undef  FpgaAD5761_1_BASE_ADDR
#endif

#ifndef FpgaAD5761_2_BASE_ADDR
#define FpgaAD5761_2_BASE_ADDR      0x60000000  //!< FPGA AD5761 Chip2 ��ַ
#undef  FpgaAD5761_2_BASE_ADDR
#endif

#ifndef FpgaAD5761_3_BASE_ADDR
#define FpgaAD5761_3_BASE_ADDR      0x60000000  //!< FPGA AD5761 Chip3 ��ַ
#undef  FpgaAD5761_3_BASE_ADDR
#endif

#ifndef FpgaAD5761_4_BASE_ADDR
#define FpgaAD5761_4_BASE_ADDR      0x60000000  //!< FPGA AD5761 Chip4 ��ַ
#undef  FpgaAD5761_4_BASE_ADDR
#endif

#ifndef FpgaAD5761_FREQ
#define FpgaAD5761_FREQ      (40 * 1000 * 1000) // ʱ��Ƶ��
#define FpgaAD5761_US_TO_CYC(usTime)  ((usTime) * (1000 / (1000*1000*1000 / FpgaAD5761_FREQ)))
#define FpgaAD5761_NS_TO_CYC(nsTime)  ((nsTime)         / (1000*1000*1000 / FpgaAD5761_FREQ) )
#endif


/*******************************************************************************
 * @}
 * @addtogroup              Registers
 * @{
 ******************************************************************************/

typedef struct {
    volatile        uint32_t    mFIFOout;       // 00 ��� FIFO д�ӿ�
    volatile const  uint32_t    mFIFOstat;      // 04 FIFO ״̬; BIT0:��; BIT1:��; BIT2:æ
    volatile        uint32_t    mCmdData;       // 08 ���ָ��; д������ֱ�ӷ���
    volatile        uint32_t    mFIFOreset;     // 0C FIFO ��λ; д������λ FIFO
    volatile const  uint32_t    mCurData;       // 10 ��ǰ����; BIT[23..0]:����; BIT24:����
    volatile        uint32_t    mPolarity;      // 14 ����ѡ��; (x)��CHIPx��ȡ����
    volatile        uint32_t    mDataFrom;      // 18 ���������Դ; BIT0:(0)���ָ��,(1)����
    volatile        uint32_t    mFIFOvldNum;    // 1C FIFO ��Ч���ݸ���
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

