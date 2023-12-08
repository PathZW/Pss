/*******************************************************************************
 * @file    devDriver_ADC.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/3/26
 * @brief   通用设备驱动框架 - ADC
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

#define DEV_CMD_ADC_START      DEV_CMD_GRP_DEF(0)   //!< 启动ADC采样;     参数2:通道;                 无返回值
#define DEV_CMD_ADC_STOP       DEV_CMD_GRP_DEF(1)   //!< 停止ADC采样;     参数2:通道;                 无返回值
#define DEV_CMD_ADC_MODE_CFG   DEV_CMD_GRP_DEF(2)   //!< 配置ADC工作模式; 参数2:通道; 参数3:模式;     无返回值
#define DEV_CMD_ADC_RATE_CFG   DEV_CMD_GRP_DEF(3)   //!< 配置ADC采样速率; 参数2:通道; 参数3:采样速率; 无返回值
#define DEV_CMD_ADC_FILT_CFG   DEV_CMD_GRP_DEF(4)   //!< 配置ADC滤波参数; 参数2:通道; 参数3:滤波类型; 无返回值
#define DEV_CMD_ADC_SYNC_CFG   DEV_CMD_GRP_DEF(5)   //!< 配置ADC同步参数; 参数2:通道; 参数3:同步类型; 无返回值
#define DEV_CMD_ADC_GRP_____
#define DEV_CMD_ADC_RECV_DATA  DEV_CMD_GRP_DEF(200) //!< 采样寄存器为 Recv() 来源; 参数2:通道;        无返回值
#define DEV_CMD_ADC_RECV_MEAR  DEV_CMD_GRP_DEF(201) //!< 校准值FIFO为 Recv() 来源; 参数2:通道;        无返回值
#define DEV_CMD_ADC_RECV_ORGN  DEV_CMD_GRP_DEF(202) //!< 原始值FIFO为 Recv() 来源; 参数2:通道;        无返回值
#define DEV_CMD_ADC_POINT_CFG  DEV_CMD_GRP_DEF(203) //!< 配置ADC采样点数; 参数2:通道; 参数3:点数;     无返回值
#define DEV_CMD_ADC_DELAY_CFG  DEV_CMD_GRP_DEF(204) //!< 配置ADC采样延时; 参数2:通道; 参数3:时间(nS); 无返回值
#define DEV_CMD_ADC_CALB_CFG   DEV_CMD_GRP_DEF(205) //!< 配置ADC校准参数; 参数2:通道; 参数3:Km; 参数4:Bm; 参数5:M; 无返回值
#if                           !DEV_CMD_GRP_CHK(205)
#error "The device driver commands code over!"
#endif

#define DEV_CMD_ADC_BIT_____
//#define DEV_CMD_ADC_xxxx     DEV_CMD_BIT_DEF(8)   //!< xxxx; 参数2:通道; 无返回值
#if                           !DEV_CMD_BIT_CHK(8)
#error "The device driver commands code over!"
#endif

#define DEV_ARG_ADC_FLUSH_MEAR DEV_CMD_ARG_DEF(0x1) //!< 仅清除ADC校准值FIFO
#define DEV_ARG_ADC_FLUSH_ORGN DEV_CMD_ARG_DEF(0x2) //!< 仅清除ADC原始值FIFO

#define DEV_ARG_ADC_FLUSH_ALL DEV_CMD_ARG_DEF(0x3) //!< 清除ADC原始值和校准值FIFO



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

#define DEV_ADC_MODE_NORM       0x00000000ul        //!< 普通模式;
#define DEV_ADC_MODE_TRIG       0x00000001ul        //!< 触发模式; 参数4:可能有

#define DEV_ADC_FILT_NONE       0x00000000ul        //!< 无滤波
#define DEV_ADC_FILT_AVG        0x00000001ul        //!< 算术平均    ; 参数4:N
#define DEV_ADC_FILT_MID        0x00000002ul        //!< 中值平均    ; 参数4:N; 参数5:D
#define DEV_ADC_FILT_REC        0x00000003ul        //!< 递推中值平均; 参数4:N; 参数5:D

#define DEV_ADC_SYNC_NONE       0x00000000ul        //!< 无同步
#define DEV_ADC_SYNC_EN         0x00000001ul        //!< 启用同步;     参数4:启用同步参数
#define DEV_ADC_SYNC_DIS        0x00000002ul        //!< 禁止同步;     参数4:禁止同步参数
#define DEV_ADC_SYNC_INTV       0x00000003ul        //!< 同步间隔;     参数4:同步间隔参数(nS)
#define DEV_ADC_SYNC_TXIV       0x00000004ul        //!< 数据发送间隔; 参数4:数据发送间隔时间(nS)


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

