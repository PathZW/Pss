/*******************************************************************************
 * @file    devDriver_DAC.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2023/4/2
 * @brief   通用设备驱动框架 - DAC
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

#define DEV_CMD_DAC_START      DEV_CMD_GRP_DEF(0)   //!< 启动DAC;         参数2:通道;                  无返回值
#define DEV_CMD_DAC_STOP       DEV_CMD_GRP_DEF(1)   //!< 停止DAC;         参数2:通道;                  无返回值
#define DEV_CMD_DAC_MODE_CFG   DEV_CMD_GRP_DEF(2)   //!< 配置DAC工作模式; 参数2:通道; 参数3:模式;      无返回值
#define DEV_CMD_DAC_RATE_CFG   DEV_CMD_GRP_DEF(3)   //!< 配置DAC转换速率; 参数2:通道; 参数3:转换速率;  无返回值
#define DEV_CMD_DAC_SYNC_CFG   DEV_CMD_GRP_DEF(4)   //!< 配置DAC同步参数; 参数2:通道; 参数3:同步类型; 无返回值
#define DEV_CMD_DAC_GRP_____
#define DEV_CMD_DAC_SEND_DATA  DEV_CMD_GRP_DEF(200) //!< 数据寄存器为 Send() 目标; 参数2:通道;         无返回值
#define DEV_CMD_DAC_SEND_REAL  DEV_CMD_GRP_DEF(201) //!< 实际值FIFO为 Send() 目标; 参数2:通道;         无返回值
#define DEV_CMD_DAC_SEND_ORGN  DEV_CMD_GRP_DEF(202) //!< 原始值FIFO为 Send() 目标; 参数2:通道;         无返回值
#define DEV_CMD_DAC_CALB_CFG   DEV_CMD_GRP_DEF(203) //!< 配置DAC校准参数; 参数2:通道;  参数3:Km; 参数4:Bm; 参数5:M; 无返回值


#if                           !DEV_CMD_GRP_CHK(203)
#error "The device driver commands code over!"
#endif

#define DEV_CMD_DAC_BIT_____
//#define DEV_CMD_DAC_xxxx     DEV_CMD_BIT_DEF(8)   //!< xxxx; 参数2:通道; 无返回值
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

#define DEV_DAC_MODE_NORM       0x00000000ul        //!< 普通模式;
#define DEV_DAC_MODE_TRIG       0x00000001ul        //!< 触发模式; 参数4:可能有

#define DEV_DAC_SYNC_NONE       0x00000000ul        //!< 无同步
#define DEV_DAC_SYNC_EN         0x00000001ul        //!< 启用同步; 参数4:启用同步参数
#define DEV_DAC_SYNC_DIS        0x00000002ul        //!< 禁止同步; 参数4:禁止同步参数
#define DEV_DAC_SYNC_INTV       0x00000003ul        //!< 同步间隔; 参数4:同步间隔参数(nS)

/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __DEV_DRIVER_DAC_H__
/*****************************  END OF FILE  **********************************/

