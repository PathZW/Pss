/*******************************************************************************
 * @file    Drv_FpgaPuls.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/23
 * @brief   Driver for FPGA Puls Modul
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
 *      2023/3/23 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DRV_FPGAPULS_H__
#define __DRV_FPGAPULS_H__
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Drv_FpgaPuls
 * @{
 * @addtogroup              Exported_Constants
 * @{
 ******************************************************************************/


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Macros
 * @{
 ******************************************************************************/

#define FPGA_PULS_ADDR                  0x6000A000      // FPGA Puls ģ���ַ
#define FPGA_PULS_FREQ               (40 * 1000 * 1000) // ʱ��Ƶ��
#define FPGA_PULS_US_TO_NUM(usTime)  ( (usTime) * (1000 / (1000*1000*1000 / FPGA_PULS_FREQ)) )
#define FPGA_PULS_NS_TO_NUM(nsTime)  ( (nsTime)         / (1000*1000*1000 / FPGA_PULS_FREQ)  )

#define FPGA_PULS_NUM_INFINITE          0xFFFFFFFF      // ����������Ϊ����


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Types
 * @{
 ******************************************************************************/

typedef struct {
    volatile        uint32_t    mPulsHiCyc;         // 00 Puls ����ߵ�ƽʱ�Ӹ���
    volatile        uint32_t    mPulsLoCyc;         // 04 Puls ����͵�ƽʱ�Ӹ���
    volatile        uint32_t    mPulsOutNum;        // 08 Puls �������
    volatile        uint32_t    mPulsOutStart;      // 0C Puls �����ʼ
    volatile        uint32_t    mPulsOutStop;       // 10 Puls ���ֹͣ
    volatile        uint32_t    mReserved1[7];      // 14-2C δʹ��
    volatile        uint32_t    mTrigMode;          // 30 Trig ģʽ
    volatile        uint32_t    mTrigOutHiCyc;      // 34 Trig ����ߵ�ƽʱ�Ӹ���
    volatile        uint32_t    mTrigOutLoCyc;      // 38 Trig ����͵�ƽʱ�Ӹ���
    volatile        uint32_t    mTrigOutNum;        // 3C Trig �������
    volatile        uint32_t    mPulsOutDlyCyc;     // 40 Puls �����ʱʱ�Ӹ���
    volatile        uint32_t    mTrigOutDlyCyc;     // 44 Trig �����ʱʱ�Ӹ���
    volatile const  uint32_t    mPulsState;         // 48 Puls ״̬�Ĵ���, (!= 0)��ʾ�������
    volatile        uint32_t    mReserved2[7];      // 4C-64 δʹ��
    volatile        uint32_t    mDCorPULS;          // 68 ֱ��������ģʽ
    volatile        uint32_t    mPulsShutOut;       // 6C Puls OUT �� SHUT
    volatile        uint32_t    mInfiniteOut;       // 70 �������(���һ��Դֵ�Ƿ�һֱ���)
} FpgaPuls_T;

typedef enum {
    FPGA_PULS_CH_1   = 0x1,     // Puls ͨ�� 1
//  FPGA_PULS_CH_2   = 0x2,     // Puls ͨ�� 2
//  FPGA_PULS_CH_3   = 0x4,     // Puls ͨ�� 3
//  FPGA_PULS_CH_4   = 0x8,     // Puls ͨ�� 4
    FPGA_PULS_CH_ALL = 0xF      // Puls ͨ�� All
} FpgaPuls_MSK;

typedef enum {
    FPGA_TRIG_M_SOUR   = 0,     // ����ģʽ: ����
    FPGA_TRIG_M_ACC_RT = 1,     // ����ģʽ: �ӻ�, �����ش���
    FPGA_TRIG_M_ACC_FT = 2,     // ����ģʽ: �ӻ�, �½��ش���
    FPGA_TRIG_M_ACC_DT = 3      // ����ģʽ: �ӻ�, ˫���ش���
} FpgaTrig_MODE;


/*******************************************************************************
 * @}
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
/**
 * FPGA Puls ��ʼ��
 *
 * @param[in]  chMsk        ͨ��ѡ��
 * @param[in]  EventFun     �¼�����ص�����
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaPuls_Init(FpgaPuls_MSK chMsk, void (*EventFun)(uint32_t));

/**
 * ���������ֱ��ģʽ
 *
 * @param[in]  chMsk    ͨ��ѡ��
 * @param[in]  bfPuls   (1)����, (0)ֱ��
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaPuls_ShapSet(FpgaPuls_MSK chMsk, int bfPuls);

/**
 * FPGA Puls ��������������ʱ��
 *
 * @param[in]  chMsk      ͨ��ѡ��
 * @param[in]  widthUS    ������ʱ��, ��΢��(uS)Ϊ��λ
 * @param[in]  periodUS   ��������ʱ��, ��΢��(uS)Ϊ��λ
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaPuls_TimeSet(FpgaPuls_MSK chMsk, uint32_t widthUS, uint32_t periodUS);

/**
 * FPGA Puls �����Ƿ�Ϊ�������
 *
 * @param[in]  chMsk      ͨ��ѡ��
 * @param[in]  bfInfinite (1)��������������, (0)���ָ���������
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaPuls_OutSet(FpgaPuls_MSK chMsk, int bfInfinite);

/**
 * FPGA Puls �����������
 *
 * @param[in]  chMsk      ͨ��ѡ��
 * @param[in]  pulsNum    ����������
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaPuls_OutStart(FpgaPuls_MSK chMsk, uint32_t pulsNum);

/**
 * FPGA Puls ֹͣ�������
 *
 * @param[in]  chMsk    ͨ��ѡ�� 
 * 
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaPuls_OutStop(FpgaPuls_MSK chMsk);

/**
 * FPGA Puls ����������ʱ
 *
 * @param[in]  chMsk     ͨ��ѡ��
 * @param[in]  delayNS   ��ʱʱ��, ������(nS)Ϊ��λ
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaPuls_DelaySet(FpgaPuls_MSK chMsk, uint32_t delayNS);


/**
 * FPGA Puls ��ȡ�Ƿ������������
 *
 * @param[in]  chMsk    ͨ��ѡ�� 
 * 
 * @retval  (!= 0) ���������
 * @retval  (== 0) ���������
 */
int FpgaPuls_BusyGet(FpgaPuls_MSK chMsk);

/**
 * FPGA Puls ���� ShutOut ����
 *
 * @param[in]  chMsk    ͨ��ѡ��
 * @param[in]  outNum   ShutOut ����
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaPuls_ShutOutSet(FpgaPuls_MSK chMsk, uint32_t outNum);

/**
 * FPGA Puls ���ô���ģʽ
 *
 * @param[in]  chMsk      ͨ��ѡ��
 * @param[in]  trigMode   ����ģʽ
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaTrig_ModeSet(FpgaPuls_MSK chMsk, FpgaTrig_MODE trigMode);

/**
 * FPGA Puls ���ô����������
 *
 * @param[in]  trigNum    �����������
 * @param[in]  delayNS    ���������ʱ, ������(nS)Ϊ��λ
 * @param[in]  widthNS    ����������, ������(nS)Ϊ��λ
 * @param[in]  periodNS   ������������, ������(nS)Ϊ��λ
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int FpgaTrig_ModeSet(FpgaPuls_MSK chMsk, FpgaTrig_MODE trigMode);
int FpgaTrig_NumSet(FpgaPuls_MSK chMsk, uint32_t num);
int FpgaTrig_OutSet(FpgaPuls_MSK chMsk, uint32_t delayNS, uint32_t hiNS, uint32_t loNS);

/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __DRV_FPGAPULS_H__
/*****************************  END OF FILE  **********************************/

