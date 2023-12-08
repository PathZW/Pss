/*******************************************************************************
 * @file    devDriver_DAC.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/4/2
 * @brief   ͨ���豸������� - DAC
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2023/4/2 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __DEV_DRIVER_DAC_H__
#define __DEV_DRIVER_DAC_H__
/******************************************************************************/
#include "devDriver_Common.h"
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup DAC
 * @{
 * @addtogroup              Command_Codes
 * @{
 ******************************************************************************/
// devDriver_CTRL()

#define DEV_CMD_DAC_START      DEV_CMD_GRP_DEF(0)   //!< ����DAC;         ����2:ͨ��;                  �޷���ֵ
#define DEV_CMD_DAC_STOP       DEV_CMD_GRP_DEF(1)   //!< ֹͣDAC;         ����2:ͨ��;                  �޷���ֵ
#define DEV_CMD_DAC_MODE_CFG   DEV_CMD_GRP_DEF(2)   //!< ����DAC����ģʽ; ����2:ͨ��; ����3:ģʽ;      �޷���ֵ
#define DEV_CMD_DAC_RATE_CFG   DEV_CMD_GRP_DEF(3)   //!< ����DACת������; ����2:ͨ��; ����3:ת������;  �޷���ֵ
#define DEV_CMD_DAC_SYNC_CFG   DEV_CMD_GRP_DEF(4)   //!< ����DACͬ������; ����2:ͨ��; ����3:ͬ������; �޷���ֵ
#define DEV_CMD_DAC_GRP_____
#define DEV_CMD_DAC_SEND_DATA  DEV_CMD_GRP_DEF(200) //!< ���ݼĴ���Ϊ Send() Ŀ��; ����2:ͨ��;         �޷���ֵ
#define DEV_CMD_DAC_SEND_REAL  DEV_CMD_GRP_DEF(201) //!< ʵ��ֵFIFOΪ Send() Ŀ��; ����2:ͨ��;         �޷���ֵ
#define DEV_CMD_DAC_SEND_ORGN  DEV_CMD_GRP_DEF(202) //!< ԭʼֵFIFOΪ Send() Ŀ��; ����2:ͨ��;         �޷���ֵ
#define DEV_CMD_DAC_CALB_CFG   DEV_CMD_GRP_DEF(203) //!< ����DACУ׼����; ����2:ͨ��;  ����3:Km; ����4:Bm; ����5:M; �޷���ֵ


#if                           !DEV_CMD_GRP_CHK(203)
#error "The device driver commands code over!"
#endif

#define DEV_CMD_DAC_BIT_____
//#define DEV_CMD_DAC_xxxx     DEV_CMD_BIT_DEF(8)   //!< xxxx; ����2:ͨ��; �޷���ֵ
#if                           !DEV_CMD_BIT_CHK(8)
#error "The device driver commands code over!"
#endif

// devDriver_CTRL()
/*******************************************************************************
 * @}
 * @addtogroup              Opt_Codes
 * @{
 ******************************************************************************/

#define DEV_DAC_CHNL(n)         (1ul << (n))
#define DEV_DAC_CHNL_ALL        (0xFFFFFFFFul)

/*******************************************************************************
 * @}
 * @addtogroup              Arg_Codes
 * @{
 ******************************************************************************/

#define DEV_DAC_MODE_NORM       0x00000000ul        //!< ��ͨģʽ;
#define DEV_DAC_MODE_TRIG       0x00000001ul        //!< ����ģʽ; ����4:������

#define DEV_DAC_SYNC_NONE       0x00000000ul        //!< ��ͬ��
#define DEV_DAC_SYNC_EN         0x00000001ul        //!< ����ͬ��; ����4:����ͬ������
#define DEV_DAC_SYNC_DIS        0x00000002ul        //!< ��ֹͬ��; ����4:��ֹͬ������
#define DEV_DAC_SYNC_INTV       0x00000003ul        //!< ͬ�����; ����4:ͬ���������(nS)

/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __DEV_DRIVER_DAC_H__
/*****************************  END OF FILE  **********************************/

