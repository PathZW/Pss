/*******************************************************************************
 * @file    Cmd_SPIx.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2021/12/28
 * @brief   SPI 命令行接口, 支持 SPI Master 和 SPI Slave 模式.
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
 *      2021/12/28 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __CMD_SPIX_H__
#define __CMD_SPIX_H__
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Cmd_SPIx
 * @{
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
// Function documentation
/**
 * @fn         void SPIx_ACT_LED(void)
 * @brief      SPI 命令接口激活显示, 存在时定义
 */


/**
 * @fn         int SPIx_INIT(void)
 * @brief      SPI 硬件初始化, 不使用 CMSIS-Driver 时定义
 * @retval     (== 0) 成功
 * @retval     (!= 0) 失败 
 */
/**
 * @fn         int SPIx_SEND_START(const void *data, unsigned num)
 * @brief      SPI 开始发送数据, 不使用 CMSIS-Driver 时定义
 * @param[in]  data   发送数据指针
 * @param[in]  num    发送数据长度(字节)
 * @retval     (== 0) 成功 
 * @retval     (!= 0) 失败 
 */
/**
 * @fn         int SPIx_RECV_START(void *data, unsigned num)
 * @brief      SPI 开始接收数据, 不使用 CMSIS-Driver 时定义
 * @param[in]  data   接收缓存指针
 * @param[in]  num    接收缓存长度(字节)
 * @retval     (== 0) 成功 
 * @retval     (!= 0) 失败 
 */
/**
 * @fn         void SPIx_IntrComplete(void)
 * @brief      SPI 数据传输(发送或接收)完成处理函数(中断). 不使用 CMSIS-Driver 时调用
 */
void SPIx_IntrComplete(void);


/**
 * @fn         void SPIx_NSS_INIT(void)
 * @brief      SPI NSS 信号初始化; SPI 主模式, SPIx_MODE_NSS_SW 为 0 时定义
 */
/**
 * @fn         void SPIx_NSS_CLR(void)
 * @brief      SSPI NSS 信号置为无效; SPI 主模式, SPIx_MODE_NSS_SW 为 0 时定义
 */
/**
 * @fn         void SPIx_NSS_SET(void)
 * @brief      SPI NSS 信号置为有效; SPI 主模式, SPIx_MODE_NSS_SW 为 0 时定义
 */


/**
 * SPI 主机侧的 从机发送FIFO非空信号线 中断处理函数; 使用时应该将 SPIx_POLL_TICKS 定义为(0).
 */
void SPIx_IntrMISO(void);


/**
 * 初始化 SPI 命令接口
 */
int Cmd_SPIxInit(void);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __CMD_SPIX_H__
/*****************************  END OF FILE  **********************************/

