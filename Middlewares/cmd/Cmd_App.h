/*******************************************************************************
 * @file    Cmd_App.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2019/6/23
 * @brief   
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2019/6/23 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __CMD_APP_H__
#define __CMD_APP_H__
#ifdef  __cplusplus
extern  "C"
{
#endif
/*******************************************************************************
 * @addtogroup Cmd_App
 * @{
 * @addtogroup              Exported_Functions
 * @{
 ******************************************************************************/
/**
 * ��ʼ������ӿ�Ӧ��
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int Cmd_AppInit(void);

//lint -esym(522, Cmd_ForwardRemIF)
/**
 * ��ȥ����ת���ӿ�(��������)
 *
 * @param[in]  cli  �����нӿ�
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int Cmd_ForwardRemIF(int cli);

//lint -esym(522, Cmd_ForwardSetIF)
/**
 * ��������ת���ӿ�(ָ������)
 *
 * @param[in]  idx  ת���ӿ�����, (0)ΪĬ�Ͻӿ�
 * @param[in]  cli  �����нӿ�
 *
 * @retval  (== 0) �ɹ�
 * @retval  (!= 0) ʧ��
 */
int Cmd_ForwardSetIF(unsigned idx, int cli);

//lint -esym(522, Cmd_ForwardGetIF)
/**
 * ��ȡ����ת���ӿ�(ָ������)
 *
 * @param[in]  idx  ת���ӿ�����, (0)ΪĬ�Ͻӿ�
 *
 * @retval  ( > 0) CLI �ӿ�
 * @retval  (<= 0) ��Ч������
 */
int Cmd_ForwardGetIF(unsigned idx);

//lint -esym(522, Cmd_ForwardToIdx)
/**
 * �����ֶ�ת��(ע��:δע��������Զ�ת����Ĭ��ת���ӿ�)
 *
 * @param[in]  idx    ת�����Ľӿ�����, (0)ΪĬ��ת���ӿ�, (0xFFFF)Ϊ����ת���ӿ�
 * @param[in]  cli    ת����Դ�����нӿ�
 * @param[in]  argc   �����ַ�������
 * @param[in]  argv   �����ַ�������, ����������, �������������('\n')
 *
 * @retval  ( > 0) �ɹ�, ����ת���������ַ���
 * @retval  (== 0) ���������Ŀ�Ľӿ���Ч
 * @retval  ( < 0) �豸�����쳣
 */
int Cmd_ForwardToIdx(unsigned idx, int cli, int argc, const char * const argv[]);


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __CMD_APP_H__
/*****************************  END OF FILE  **********************************/

