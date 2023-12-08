/*******************************************************************************
 * @file    devDriver_ADC.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/26
 * @brief   ͨ���豸������� - ADC
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2023/3/26 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DEV_DRIVER_ADC_H__
#define __DEV_DRIVER_ADC_H__
/******************************************************************************/
#include "devDriver_Common.h"
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
 * @addtogroup              Command_Codes
 * @{
 ******************************************************************************/
// devDriver_CTRL()

#define DEV_CMD_ADC_START      DEV_CMD_GRP_DEF(0)   //!< ����ADC����;     ����2:ͨ��;                 �޷���ֵ
#define DEV_CMD_ADC_STOP       DEV_CMD_GRP_DEF(1)   //!< ֹͣADC����;     ����2:ͨ��;                 �޷���ֵ
#define DEV_CMD_ADC_MODE_CFG   DEV_CMD_GRP_DEF(2)   //!< ����ADC����ģʽ; ����2:ͨ��; ����3:ģʽ;     �޷���ֵ
#define DEV_CMD_ADC_RATE_CFG   DEV_CMD_GRP_DEF(3)   //!< ����ADC��������; ����2:ͨ��; ����3:��������; �޷���ֵ
#define DEV_CMD_ADC_FILT_CFG   DEV_CMD_GRP_DEF(4)   //!< ����ADC�˲�����; ����2:ͨ��; ����3:�˲�����; �޷���ֵ
#define DEV_CMD_ADC_SYNC_CFG   DEV_CMD_GRP_DEF(5)   //!< ����ADCͬ������; ����2:ͨ��; ����3:ͬ������; �޷���ֵ
#define DEV_CMD_ADC_GRP_____
#define DEV_CMD_ADC_RECV_DATA  DEV_CMD_GRP_DEF(200) //!< �����Ĵ���Ϊ Recv() ��Դ; ����2:ͨ��;        �޷���ֵ
#define DEV_CMD_ADC_RECV_MEAR  DEV_CMD_GRP_DEF(201) //!< У׼ֵFIFOΪ Recv() ��Դ; ����2:ͨ��;        �޷���ֵ
#define DEV_CMD_ADC_RECV_ORGN  DEV_CMD_GRP_DEF(202) //!< ԭʼֵFIFOΪ Recv() ��Դ; ����2:ͨ��;        �޷���ֵ
#define DEV_CMD_ADC_POINT_CFG  DEV_CMD_GRP_DEF(203) //!< ����ADC��������; ����2:ͨ��; ����3:����;     �޷���ֵ
#define DEV_CMD_ADC_DELAY_CFG  DEV_CMD_GRP_DEF(204) //!< ����ADC������ʱ; ����2:ͨ��; ����3:ʱ��(nS); �޷���ֵ
#define DEV_CMD_ADC_CALB_CFG   DEV_CMD_GRP_DEF(205) //!< ����ADCУ׼����; ����2:ͨ��; ����3:Km; ����4:Bm; ����5:M; �޷���ֵ
#if                           !DEV_CMD_GRP_CHK(205)
#error "The device driver commands code over!"
#endif

#define DEV_CMD_ADC_BIT_____
//#define DEV_CMD_ADC_xxxx     DEV_CMD_BIT_DEF(8)   //!< xxxx; ����2:ͨ��; �޷���ֵ
#if                           !DEV_CMD_BIT_CHK(8)
#error "The device driver commands code over!"
#endif

#define DEV_ARG_ADC_FLUSH_MEAR DEV_CMD_ARG_DEF(0x1) //!< �����ADCУ׼ֵFIFO
#define DEV_ARG_ADC_FLUSH_ORGN DEV_CMD_ARG_DEF(0x2) //!< �����ADCԭʼֵFIFO

#define DEV_ARG_ADC_FLUSH_ALL DEV_CMD_ARG_DEF(0x3) //!< ���ADCԭʼֵ��У׼ֵFIFO



// devDriver_CTRL()
/*******************************************************************************
 * @}
 * @addtogroup              Opt_Codes
 * @{
 ******************************************************************************/

#define DEV_ADC_CHNL(n)         (1ul << (n))
#define DEV_ADC_CHNL_ALL        (0xFFFFFFFFul)


/*******************************************************************************
 * @}
 * @addtogroup              Arg_Codes
 * @{
 ******************************************************************************/

#define DEV_ADC_MODE_NORM       0x00000000ul        //!< ��ͨģʽ;
#define DEV_ADC_MODE_TRIG       0x00000001ul        //!< ����ģʽ; ����4:������

#define DEV_ADC_FILT_NONE       0x00000000ul        //!< ���˲�
#define DEV_ADC_FILT_AVG        0x00000001ul        //!< ����ƽ��    ; ����4:N
#define DEV_ADC_FILT_MID        0x00000002ul        //!< ��ֵƽ��    ; ����4:N; ����5:D
#define DEV_ADC_FILT_REC        0x00000003ul        //!< ������ֵƽ��; ����4:N; ����5:D

#define DEV_ADC_SYNC_NONE       0x00000000ul        //!< ��ͬ��
#define DEV_ADC_SYNC_EN         0x00000001ul        //!< ����ͬ��;     ����4:����ͬ������
#define DEV_ADC_SYNC_DIS        0x00000002ul        //!< ��ֹͬ��;     ����4:��ֹͬ������
#define DEV_ADC_SYNC_INTV       0x00000003ul        //!< ͬ�����;     ����4:ͬ���������(nS)
#define DEV_ADC_SYNC_TXIV       0x00000004ul        //!< ���ݷ��ͼ��; ����4:���ݷ��ͼ��ʱ��(nS)


/*******************************************************************************
 * @}
 * @addtogroup              Error_Codes
 * @{
 ******************************************************************************/


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif
/*****************************  END OF FILE  **********************************/

