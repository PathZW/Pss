/*******************************************************************************
 * @file    Cmd_SPIx.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2021/12/28
 * @brief   SPI �����нӿ�, ֧�� SPI Master �� SPI Slave ģʽ.
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
 * @brief      SPI ����ӿڼ�����ʾ, ����ʱ����
 */


/**
 * @fn         int SPIx_INIT(void)
 * @brief      SPI Ӳ����ʼ��, ��ʹ�� CMSIS-Driver ʱ����
 * @retval     (== 0) �ɹ�
 * @retval     (!= 0) ʧ�� 
 */
/**
 * @fn         int SPIx_SEND_START(const void *data, unsigned num)
 * @brief      SPI ��ʼ��������, ��ʹ�� CMSIS-Driver ʱ����
 * @param[in]  data   ��������ָ��
 * @param[in]  num    �������ݳ���(�ֽ�)
 * @retval     (== 0) �ɹ� 
 * @retval     (!= 0) ʧ�� 
 */
/**
 * @fn         int SPIx_RECV_START(void *data, unsigned num)
 * @brief      SPI ��ʼ��������, ��ʹ�� CMSIS-Driver ʱ����
 * @param[in]  data   ���ջ���ָ��
 * @param[in]  num    ���ջ��泤��(�ֽ�)
 * @retval     (== 0) �ɹ� 
 * @retval     (!= 0) ʧ�� 
 */
/**
 * @fn         void SPIx_IntrComplete(void)
 * @brief      SPI ���ݴ���(���ͻ����)��ɴ�����(�ж�). ��ʹ�� CMSIS-Driver ʱ����
 */
void SPIx_IntrComplete(void);


/**
 * @fn         void SPIx_NSS_INIT(void)
 * @brief      SPI NSS �źų�ʼ��; SPI ��ģʽ, SPIx_MODE_NSS_SW Ϊ 0 ʱ����
 */
/**
 * @fn         void SPIx_NSS_CLR(void)
 * @brief      SSPI NSS �ź���Ϊ��Ч; SPI ��ģʽ, SPIx_MODE_NSS_SW Ϊ 0 ʱ����
 */
/**
 * @fn         void SPIx_NSS_SET(void)
 * @brief      SPI NSS �ź���Ϊ��Ч; SPI ��ģʽ, SPIx_MODE_NSS_SW Ϊ 0 ʱ����
 */


/**
 * SPI ������� �ӻ�����FIFO�ǿ��ź��� �жϴ�����; ʹ��ʱӦ�ý� SPIx_POLL_TICKS ����Ϊ(0).
 */
void SPIx_IntrMISO(void);


/**
 * ��ʼ�� SPI ����ӿ�
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

