/*******************************************************************************
 * @file    emb_prompt.h
 * @author  Linghu(Linghu0060@qq.com)
 * @version v1.0.0
 * @date    2020/5/4
 * @brief   Embedded platform defines for the prompt of system status
 *******************************************************************************
 * @par Last Commit:
 *      \$Author: User $ \n
 *      \$Date: 2015-02-02 11:01:08 +0800 $ \n
 *      \$Rev: 1 $ \n
 *
 * @par Change Logs:
 *      2020/5/4 -- Linghu -- the first version
 ******************************************************************************/
#ifndef __EMB_PROMPT_H__
#define __EMB_PROMPT_H__
/******************************************************************************/
#ifdef  __cplusplus
extern  "C"
{
#endif
/******************************************************************************/
/**
 * @addtogroup Emb_Prompt
 * @{
 * @addtogroup          Exported_Functions
 * @{
 ******************************************************************************/

void SysPrompt_Assert (void);   //!< ϵͳ��ʾ: �������� ---- ��ɫ����
void SysPrompt_Booting(void);   //!< ϵͳ��ʾ: �������� ---- ��ɫ����
void SysPrompt_Upgrade(void);   //!< ϵͳ��ʾ: �����̼� ---- ��ɫ����
void SysPrompt_Error  (void);   //!< ϵͳ��ʾ: ����ָʾ ---- ��ɫ����
void SysPrompt_Warning(void);   //!< ϵͳ��ʾ: ����ָʾ ---- ��ɫ����
void SysPrompt_Initing(void);   //!< ϵͳ��ʾ: ���ڳ�ʼ�� -- ��ɫ����
void SysPrompt_Normal (void);   //!< ϵͳ��ʾ: �������� ---- ��ɫ����

void SysPromptStateInit(void);  //!< ϵͳ״̬��ʾ��ʼ��
void SysPromptStatePoll(void);  //!< ϵͳ״̬��ʾ��ѯ, ÿ100�������һ��


/*****************************  END OF FILE  ***********************************
 * @}
 * @}
 */
#ifdef  __cplusplus
}
#endif
#endif  // __EMB_PROMPT_H__
/*****************************  END OF FILE  **********************************/

