/*******************************************************************************
 * @file    devDriver_FpgaAD4000.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/26
 * @brief   ͨ���豸���� - ADC - FPGA AD4000, ������"devDriver_cfg.h"����.\n
 *          ���á�extern devDriver_t devDriver(FpgaAD4000, x)�� xΪ 1 ~ 4 ����ʹ��.
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
#ifndef __DEV_DRIVER_FpgaAD4000_H__
#define __DEV_DRIVER_FpgaAD4000_H__
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Device_Driver
 * @{
 * @addtogroup ADC
 * @{
 * @addtogroup FpgaAD4000
 * @{
 * @addtogroup              __Config
 * @{
 ******************************************************************************/

#ifndef FpgaAD4000_1_BASE_ADDR
#define FpgaAD4000_1_BASE_ADDR      0x60001000  //!< FPGA AD4000 Chip1 ��ַ
#undef  FpgaAD4000_1_BASE_ADDR
#endif

#ifndef FpgaAD4000_2_BASE_ADDR
#define FpgaAD4000_2_BASE_ADDR      0x60002000  //!< FPGA AD4000 Chip2 ��ַ
#undef  FpgaAD4000_2_BASE_ADDR
#endif

#ifndef FpgaAD4000_3_BASE_ADDR
#define FpgaAD4000_3_BASE_ADDR      0x60003000  //!< FPGA AD4000 Chip3 ��ַ
#undef  FpgaAD4000_3_BASE_ADDR
#endif

#ifndef FpgaAD4000_4_BASE_ADDR
#define FpgaAD4000_4_BASE_ADDR      0x60004000  //!< FPGA AD4000 Chip4 ��ַ
#undef  FpgaAD4000_4_BASE_ADDR
#endif

#ifndef FpgaAD4000_SYNC_BASE_ADDR
#define FpgaAD4000_SYNC_BASE_ADDR   0x60000000  //!< ��Ƭ SYNC ��ַ(ͬ������)
#undef  FpgaAD4000_SYNC_BASE_ADDR
#endif

#ifndef FpgaAD4000_FREQ
#define FpgaAD4000_FREQ      (40 * 1000 * 1000) //!< ʱ��Ƶ��
#define FpgaAD4000_US_TO_CYC(usTime)  ((usTime) * (1000 / (1000*1000*1000 / FpgaAD4000_FREQ)))
#define FpgaAD4000_NS_TO_CYC(nsTime)  ((nsTime)         / (1000*1000*1000 / FpgaAD4000_FREQ) )
#endif

/*Recv Define*/
#define DEV_AD4000RECV_FROM_SAMPREG    0 //0��ʾ������Դ�����Ĵ�������
#define DEV_AD4000RECV_FROM_CHKFIFO    1 //1��ʾУ׼ֵFIFO
#define DEV_AD4000RECV_FROM_ORGFIFO    2 //2��ʾԭʼֵFIFO
#define DEV_AD4000RECV_CHKFIFO_EMPTY   ((dev->devRegs->mFIFOstate & 0x4) == 0x4)
#define DEV_AD4000RECV_ORGFIFO_EMPTY   ((dev->devRegs->mFIFOstate & 0x1) == 1)
/*Recv Define*/

/*ADC_FIFORESET*/
#define DEV_ADC_RESET_CHKFIFOREG      0x2
#define DEV_ADC_RESET_ORGFIFOREG      0x1
#define DEV_ADC_RESET_FIFO_REG        (dev->devRegs->mFIFOreset)
/*ADC_FIFORESET*/

/*ADC MODE �����ʹ���*/
#define DEV_ADC_MODE_NORM_VALUE          1
#define DEV_ADC_MODE_TRIG_VALUE          0

/*ADC FILT*/
#define DEV_ADC_FILT_NONE_VALUE          1
/*ADC FILT*/

/*******************************************************************************
 * @}
 * @addtogroup              Registers
 * @{
 ******************************************************************************/

typedef struct {
    volatile        uint32_t    mCalBm;         // 00 У׼ϵ�� Bm
    volatile        uint32_t    mCalKm;         // 04 У׼ϵ�� Km
    volatile        uint32_t    mCalM10;        // 08 У׼ϵ�� M
    volatile        uint32_t    mSamMode;       // 0C ����ģʽ
    volatile        uint32_t    mSamDlyCyc;     // 10 ������ʱʱ��
    volatile        uint32_t    mSamPoint;      // 14 ��������
    volatile const  uint32_t    mFIFOmear;      // 18 У׼ֵ FIFO
    volatile const  uint32_t    mFIFOorgn;      // 1C ԭʼֵ FIFO
    volatile const  uint32_t    mFIFOstate;     // 20 FIFO ״̬
    volatile        uint32_t    mFIFOreset;     // 24 FIFO ��λ����
    volatile        uint32_t    mChipType;      // 28 AD4000 or AD4001
    volatile        uint32_t    mSamAvgNum;     // 2C �����˲�ƽ������ N
    volatile        uint32_t    mSamOrgVal;     // 30 ԭʼֵʵʱ�Ĵ���
} FpgaAD4000_REGS;

typedef struct {
    volatile        uint32_t    mSyncStart;     // 00 ͬ����������
    volatile        uint32_t    mSyncStop;      // 04 ͬ���رղ���
    volatile        uint32_t    mTxInterval;    // 08 ���ݷ��ͼ��
} FpgaAD4000_SYNC_REGS;

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

